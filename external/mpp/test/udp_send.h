//
// Created by consti10 on 16.09.21.
//

#ifndef MERGED_UDP_SEND_H
#define MERGED_UDP_SEND_H

#include <sys/time.h>
//socket
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

static uint64_t lastTimeStamp=0;
static int sockfd;
static struct sockaddr_in address={};
static const int UDP_PACKET_MAX_SIZE=65507;
static const int M_PORT=5600;
static char* M_IP="192.168.0.12";
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
    mySendTo(fakeNALU,sizeof(fakeNALU));
    //mySendTo(EXAMPLE_AUD,sizeof(EXAMPLE_AUD));
}

#endif //MERGED_UDP_SEND_H
