// Copyright 2020 Fuzhou Rockchip Electronics Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <assert.h>
#include <fcntl.h>
#include <getopt.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <inttypes.h>

#include "common/sample_common.h"
#include "rkmedia_api.h"
#include "rkmedia_venc.h"

#include <sys/time.h>
//socket
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

static bool quit = false;
static void sigterm_handler(int sig) {
    fprintf(stderr, "signal %d\n", sig);
    quit = true;
}

static uint64_t lastTimeStamp=0;
static int sockfd;
static struct sockaddr_in address={};
static const int UDP_PACKET_MAX_SIZE=65507;
static const int M_PORT=5600;
static char* M_IP="192.168.0.11";
static uint64_t frameDeltaAvgSum=0;
static uint64_t frameDeltaAvgCount=0;

// this is just a NALU with no content
static __attribute__((unused)) uint8_t fakeNALU[4]={0,0,0,1};
// this is the data for an h264 AUD unit
static __attribute__((unused)) uint8_t EXAMPLE_AUD[6]={0,0,0,1,9,48};

// this thread should run as close to realtime as possible
static void __attribute__((unused)) setThreadParamsMaxRealtime(pthread_t target){
    int policy=SCHED_FIFO;
    struct sched_param param;
    param.sched_priority=sched_get_priority_max(policy);
    int result= pthread_setschedparam(target,policy,&param);
    if(result!=0){
        printf("WARNING cannot set ThreadParamsMaxRealtime\n");
    }
}

static void __attribute__((unused)) setThreadParamsMaxRealtime2(){
    setThreadParamsMaxRealtime(pthread_self());
}


static uint64_t getTimeMs(){
    struct timeval time;
    gettimeofday(&time, NULL);
    uint64_t millis = (time.tv_sec * ((uint64_t)1000)) + ((uint64_t)time.tv_usec / ((uint64_t)1000));
    return millis;
}

static uint64_t __attribute__((unused)) getTimeUs(){
    struct timeval time;
    gettimeofday(&time, NULL);
    uint64_t micros = (time.tv_sec * ((uint64_t)1000*1000)) + ((uint64_t)time.tv_usec);
    return micros;
}


static void createSocket(){
    sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if (sockfd < 0) {
        printf("Cannot create socket\n");
    }
    //Create the address
    address.sin_family = AF_INET;
    address.sin_port = htons(M_PORT);
    inet_pton(AF_INET,M_IP, &address.sin_addr);
}

static void __attribute__((unused)) mySendTo(void* data,int data_length){
    if(data_length>UDP_PACKET_MAX_SIZE){
        printf("Data size exceeds UDP packet size\n");
        // if the data size exceeds the size of one UDP packet, just chunck it - note though that this means
        // that one udp frame now might not contain a whole NALU, but only parts of it. RTP would be better.
        int remaining=data_length;
        int offset=0;
        while(remaining>0){
        	int chunckSize=remaining > UDP_PACKET_MAX_SIZE ? UDP_PACKET_MAX_SIZE : remaining;
        	mySendTo(&((uint8_t*)data)[offset],chunckSize);
        	remaining-=chunckSize;
        	offset+=chunckSize;
        }
        return;
    }
    int result= sendto(sockfd, data, data_length, 0, (struct sockaddr *) &(address),
                       sizeof(struct sockaddr_in));
    if(result<0){
        printf("Cannot send data\n");
    }else{
        //printf("Successfully sent data\n");
    }
}




// send a "empty NALU" to decrease latency (obviosly this works, but we want RTP instead)
static void __attribute__((unused)) sendFakeNALU(){
    //mySendTo(fakeNALU,sizeof(fakeNALU));
    mySendTo(EXAMPLE_AUD,sizeof(EXAMPLE_AUD));
}


void video_packet_cb(MEDIA_BUFFER mb) {
    const char *nalu_type = "Unknown";
    switch (RK_MPI_MB_GetFlag(mb)) {
        case VENC_NALU_IDRSLICE:
            nalu_type = "IDR Slice";
            break;
        case VENC_NALU_PSLICE:
            nalu_type = "P Slice";
            break;
        case VENC_NALU_SPS:
            nalu_type="SPS";
            break;
        case VENC_NALU_PPS:
            nalu_type="PPS";
            break;
        default:
            break;
    }
    printf("Get Video Encoded packet(%s):ptr:%p, fd:%d, size:%zu, mode:%d\n",
           nalu_type, RK_MPI_MB_GetPtr(mb), RK_MPI_MB_GetFD(mb),
           RK_MPI_MB_GetSize(mb), RK_MPI_MB_GetModeID(mb));
    // hm how exactly does one get the sps/pps ?!

    // send out NALU data via udp (raw)
    mySendTo( RK_MPI_MB_GetPtr(mb),RK_MPI_MB_GetSize(mb));
    // send a fake nalu to reduce latency (next frame to determine end of prev. frame issue).
    sendFakeNALU();

    //Consti10: print time to check for fps
    uint64_t ts=getTimeMs();
    uint64_t delta=ts-lastTimeStamp;
    uint64_t buffer_ts=RK_MPI_MB_GetTimestamp(mb);
    //uint64_t mb_latency=ts-(buffer_ts/1000);
    frameDeltaAvgSum+=delta;
    frameDeltaAvgCount++;
    if(frameDeltaAvgCount>100){
        float avgFrameDelta=(float)((double)frameDeltaAvgSum / (double)frameDeltaAvgCount);
        printf("Avg of frame delta: %f\n",avgFrameDelta);
        frameDeltaAvgSum=0;
        frameDeltaAvgCount=0;
    }
    lastTimeStamp=ts;
    printf("Current time %" PRIu64 "(ms), delta %" PRIu64 "(ms) MB ts:%" PRIu64 "\n",ts,delta,buffer_ts);
    RK_MPI_MB_TsNodeDump(mb);
    //Consti10: add end
    RK_MPI_MB_ReleaseBuffer(mb);
}

static RK_CHAR optstr[] = "?:a::h:w:e:d:f:i";
static const struct option long_options[] = {
        {"aiq", optional_argument, NULL, 'a'},
        {"height", required_argument, NULL, 'h'},
        {"width", required_argument, NULL, 'w'},
        {"encode", required_argument, NULL, 'e'},
        {"framerate", required_argument, NULL, 'f'},
        {"device_name", required_argument, NULL, 'd'},
        {"ip_address",required_argument,NULL, 'i'},
        {NULL, 0, NULL, 0},
};

static void print_usage(const RK_CHAR *name) {
    printf("usage example:\n");
#ifdef RKAIQ
    printf("\t%s "
         "[-a | --aiq /oem/etc/iqfiles/] "
         "[-h | --height 1080] "
         "[-w | --width 1920] "
         "[-e | --encode 0] "
         "[-f | --framerate 30]"
         "[-d | --device_name rkispp_scale0]\n",
         name);
  printf("\t-a | --aiq: enable aiq with dirpath provided, eg:-a "
         "/oem/etc/iqfiles/, "
         "set dirpath empty to using path by default, without this option aiq "
         "should run in other application\n");
#else
    printf("\t%s "
           "[-h | --height 1080] "
           "[-w | --width 1920] "
           "[-e | --encode 0] "
           "[-d | --device_name rkispp_scale0]\n",
           name);
#endif
    printf("\t-h | --height: VI height, Default:1080\n");
    printf("\t-w | --width: VI width, Default:1920\n");
    printf("\t-e | --encode: encode type, Default:0, set 0 to use H264, set 1 to "
           "use H265\n");
    printf("\t-d | --device_name set pcDeviceName, Default:rkispp_scale0, "
           "Option:[rkispp_scale0, rkispp_scale1, rkispp_scale2]\n");
}

int main(int argc, char *argv[]) {
    // I think this also affects child processes, but not sure - doesn't make a difference 
    // on this system though.
    //setThreadParamsMaxRealtime2();
    RK_U32 u32Width = 1920;
    RK_U32 u32Height = 1080;
    RK_U32 encode_type = 0;
    RK_U32 m_framerate=30;
    RK_CHAR *device_name = "rkispp_m_bypass";
    RK_CHAR *iq_file_dir = NULL;
    RK_S32 s32CamId = 0;
    
    int ret = 0;
    int c;
    while ((c = getopt_long(argc, argv, optstr, long_options, NULL)) != -1) {
        const char *tmp_optarg = optarg;
        switch (c) {
            case 'a':
                if (!optarg && NULL != argv[optind] && '-' != argv[optind][0]) {
                    tmp_optarg = argv[optind++];
                }
                if (tmp_optarg) {
                    iq_file_dir = (char *)tmp_optarg;
                } else {
                    iq_file_dir = "/oem/etc/iqfiles";
                }
                break;
            case 'h':
                u32Height = atoi(optarg);
                break;
            case 'w':
                u32Width = atoi(optarg);
                break;
            case 'e':
                encode_type = atoi(optarg);
                break;
            case 'd':
                device_name = optarg;
                break;
            case 'f':
                m_framerate=atoi(optarg);
                break;
            case 'i':
                M_IP=optarg;
                break;
            case '?':
            default:
                print_usage(argv[0]);
                return 0;
        }
    }
    // consti10: with isp bypass we can use FBC0, else we have to use NV12
    IMAGE_TYPE_E m_image_type=IMAGE_TYPE_NV12;
    if(strcmp(device_name,"rkispp_m_bypass")==0){
      m_image_type=IMAGE_TYPE_FBC0;
    }

    createSocket();

    printf("device_name: %s\n", device_name);
    printf("#height: %d\n", u32Height);
    printf("#width: %d\n", u32Width);
    printf("#framerate: %d\n", m_framerate);
    printf("#encode_type: %d\n", encode_type);
    printf("#ip_address: %s\n",M_IP);
    printf("#image type:%d\n",(int)m_image_type);

    if (iq_file_dir) {
#ifdef RKAIQ
        printf("#Aiq xml dirpath: %s\n", iq_file_dir);
    rk_aiq_working_mode_t hdr_mode = RK_AIQ_WORKING_MODE_NORMAL;
    RK_BOOL fec_enable = RK_FALSE;
    int fps = m_framerate;
    SAMPLE_COMM_ISP_Init(s32CamId,hdr_mode, fec_enable, iq_file_dir);
    SAMPLE_COMM_ISP_Run(s32CamId);
    SAMPLE_COMM_ISP_SetFrameRate(s32CamId,fps);
#endif
    }

    RK_MPI_SYS_Init();
    VI_CHN_ATTR_S vi_chn_attr;
    vi_chn_attr.pcVideoNode = device_name;
    vi_chn_attr.u32BufCnt = 6; // should be >= 4 | Consti10: was 4 by default
    vi_chn_attr.u32Width = u32Width;
    vi_chn_attr.u32Height = u32Height;
    vi_chn_attr.enPixFmt = m_image_type;
    vi_chn_attr.enWorkMode = VI_WORK_MODE_NORMAL;
    // did I accidentally remove this ?!
    // somewhere it says VI_CHN_BUF_TYPE_DMA = 0, // Default , so perhaps I should have been using MMAP ?!
    //vi_chn_attr.enBufType = VI_CHN_BUF_TYPE_MMAP;
    printf("Consti10: Used vi_chn_attr.enBufType:%d\n",vi_chn_attr.enBufType);
    ret = RK_MPI_VI_SetChnAttr(0, 0, &vi_chn_attr);
    ret |= RK_MPI_VI_EnableChn(0, 0);
    if (ret) {
        printf("ERROR: create VI[0] error! ret=%d\n", ret);
        return 0;
    }

    VENC_CHN_ATTR_S venc_chn_attr;
    switch (encode_type) {
        case 0:
            venc_chn_attr.stVencAttr.enType = RK_CODEC_TYPE_H264;
            venc_chn_attr.stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;
            // Consti10: isn't it best to manually set the level ?!
            venc_chn_attr.stVencAttr.stAttrH264e.u32Level = 51;
            break;
        case 1:
            venc_chn_attr.stVencAttr.enType = RK_CODEC_TYPE_H265;
            venc_chn_attr.stRcAttr.enRcMode = VENC_RC_MODE_H265CBR;
            break;
        default:
            venc_chn_attr.stVencAttr.enType = RK_CODEC_TYPE_H264;
            venc_chn_attr.stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;
            break;
    }
    venc_chn_attr.stVencAttr.imageType = m_image_type;
    venc_chn_attr.stVencAttr.u32PicWidth = u32Width;
    venc_chn_attr.stVencAttr.u32PicHeight = u32Height;
    venc_chn_attr.stVencAttr.u32VirWidth = u32Width;
    venc_chn_attr.stVencAttr.u32VirHeight = u32Height;
    // Consti10: use baseline instead of main ?
    //venc_chn_attr.stVencAttr.u32Profile = 77;
    venc_chn_attr.stVencAttr.u32Profile = 66;
    // Consti10: if we use 1 as GOP, we obviously increase bit rate a lot, but each frame can be decoded independently.
    // however, we probably want to make this user-configurable later. For latency testing though it is nice to have it at 1,
    // since it makes everything more consistently
    venc_chn_attr.stRcAttr.stH264Cbr.u32Gop = 1;
    //venc_chn_attr.stRcAttr.stH264Cbr.u32BitRate = 1920 * 1080 * 30 / 14;
    //1920 * 1080 * 30 / 14 == 4443428.57143 (4.4 MBit/s)
    // use a fixed X MBit/s as bit rate  
    venc_chn_attr.stRcAttr.stH264Cbr.u32BitRate = 5*1000*1000;
    venc_chn_attr.stRcAttr.stH264Cbr.fr32DstFrameRateDen = 0;
    venc_chn_attr.stRcAttr.stH264Cbr.fr32DstFrameRateNum = m_framerate;
    venc_chn_attr.stRcAttr.stH264Cbr.u32SrcFrameRateDen = 0;
    venc_chn_attr.stRcAttr.stH264Cbr.u32SrcFrameRateNum = m_framerate;
    ret = RK_MPI_VENC_CreateChn(0, &venc_chn_attr);
    if (ret) {
        printf("ERROR: create VENC[0] error! ret=%d\n", ret);
        return 0;
    }

    MPP_CHN_S stEncChn;
    stEncChn.enModId = RK_ID_VENC;
    stEncChn.s32DevId = 0;
    stEncChn.s32ChnId = 0;
    ret = RK_MPI_SYS_RegisterOutCb(&stEncChn, video_packet_cb);
    if (ret) {
        printf("ERROR: register output callback for VENC[0] error! ret=%d\n", ret);
        return 0;
    }

    MPP_CHN_S stSrcChn;
    stSrcChn.enModId = RK_ID_VI;
    stSrcChn.s32DevId = 0;
    stSrcChn.s32ChnId = 0;
    MPP_CHN_S stDestChn;
    stDestChn.enModId = RK_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = 0;
    ret = RK_MPI_SYS_Bind(&stSrcChn, &stDestChn);
    if (ret) {
        printf("ERROR: Bind VI[0] and VENC[0] error! ret=%d\n", ret);
        return 0;
    }

    printf("%s initial finish\n", __func__);
    signal(SIGINT, sigterm_handler);

    while (!quit) {
        usleep(100);
    }

    if (iq_file_dir) {
#ifdef RKAIQ
        SAMPLE_COMM_ISP_Stop(s32CamId); // isp aiq stop before vi streamoff
#endif
    }

    printf("%s exit!\n", __func__);
    // unbind first
    ret = RK_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
    if (ret) {
        printf("ERROR: UnBind VI[0] and VENC[0] error! ret=%d\n", ret);
        return 0;
    }
    // destroy venc before vi
    ret = RK_MPI_VENC_DestroyChn(0);
    if (ret) {
        printf("ERROR: Destroy VENC[0] error! ret=%d\n", ret);
        return 0;
    }
    // destroy vi
    ret = RK_MPI_VI_DisableChn(0, 0);
    if (ret) {
        printf("ERROR: Destroy VI[0] error! ret=%d\n", ret);
        return 0;
    }

    return 0;
}
