#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <errno.h>
#include <alsa/asoundlib.h>
#include <signal.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "eq_log.h"
#include "Rk_wake_lock.h"
#include "Rk_socket_app.h"

#define SOC_IS_RK3308           (0x1)
#define SOC_IS_RK3326           (0x2)

#define ROCKCHIP_SOC            SOC_IS_RK3326

#define SAMPLE_RATE 48000
#define CHANNEL 2
#define REC_DEVICE_NAME "fake_record"
#define WRITE_DEVICE_NAME "fake_play"
#define JACK_DEVICE_NAME "fake_jack"
#define JACK2_DEVICE_NAME "fake_jack2"
#define READ_FRAME  1024    //(768)
#define PERIOD_SIZE (1024)  //(SAMPLE_RATE/8)
#define PERIOD_counts (3) //double of delay 3*21.3=64ms
#define BUFFER_SIZE (PERIOD_SIZE * 10) // keep a large buffer_size
#define MUTE_TIME_THRESHOD (1)//seconds
#define MUTE_FRAME_THRESHOD (SAMPLE_RATE * MUTE_TIME_THRESHOD / READ_FRAME)//30 seconds
//#define ALSA_READ_FORMAT SND_PCM_FORMAT_S32_LE
#define ALSA_READ_FORMAT SND_PCM_FORMAT_S16_LE
#define ALSA_WRITE_FORMAT SND_PCM_FORMAT_S16_LE

/*
 * Select different alsa pathways based on device type.
 *  LINE_OUT: LR-Mix(fake_play)->EqDrcProcess(ladspa)->Speaker(real_playback)
 *  HEAD_SET: fake_jack -> Headset(real_playback)
 *  BLUETOOTH: device as bluetooth source.
 */
#define DEVICE_FLAG_LINE_OUT        0x01
#define DEVICE_FLAG_ANALOG_HP       0x02
#define DEVICE_FLAG_DIGITAL_HP      0x03
#define DEVICE_FLAG_BLUETOOTH       0x04
#define DEVICE_FLAG_BLUETOOTH_BSA   0x05

enum BT_CONNECT_STATE{
    BT_DISCONNECT = 0,
    BT_CONNECT_BLUEZ,
    BT_CONNECT_BSA
};

static char g_bt_mac_addr[17];
static enum BT_CONNECT_STATE g_bt_is_connect = BT_DISCONNECT;
static bool g_system_sleep = false;
static char sock_path[] = "/data/bsa/config/bsa_socket";

struct timeval tv_begin, tv_end;
//gettimeofday(&tv_begin, NULL);

extern int set_sw_params(snd_pcm_t *pcm, snd_pcm_uframes_t buffer_size,
                         snd_pcm_uframes_t period_size, char **msg);

void alsa_fake_device_record_open(snd_pcm_t** capture_handle,int channels,uint32_t rate)
{
    snd_pcm_hw_params_t *hw_params;
    snd_pcm_uframes_t periodSize = PERIOD_SIZE;
    snd_pcm_uframes_t bufferSize = BUFFER_SIZE;
    int dir = 0;
    int err;

    err = snd_pcm_open(capture_handle, REC_DEVICE_NAME, SND_PCM_STREAM_CAPTURE, 0);
    if (err)
    {
        eq_err("[EQ_RECORD_OPEN] Unable to open capture PCM device\n");
        exit(1);
    }
    eq_debug("[EQ_RECORD_OPEN] snd_pcm_open\n");
    //err = snd_pcm_hw_params_alloca(&hw_params);

    err = snd_pcm_hw_params_malloc(&hw_params);
    if(err)
    {
        eq_err("[EQ_RECORD_OPEN] cannot allocate hardware parameter structure (%s)\n", snd_strerror(err));
        exit(1);
    }
    eq_debug("[EQ_RECORD_OPEN] snd_pcm_hw_params_malloc\n");

    err = snd_pcm_hw_params_any(*capture_handle, hw_params);
    if(err)
    {
        eq_err("[EQ_RECORD_OPEN] cannot initialize hardware parameter structure (%s)\n", snd_strerror(err));
        exit(1);
    }
    eq_debug("[EQ_RECORD_OPEN] snd_pcm_hw_params_any!\n");

    err = snd_pcm_hw_params_set_access(*capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    // err = snd_pcm_hw_params_set_access(*capture_handle, hw_params, SND_PCM_ACCESS_MMAP_NONINTERLEAVED);
    if (err)
    {
        eq_err("[EQ_RECORD_OPEN] Error setting interleaved mode\n");
        exit(1);
    }
    eq_debug("[EQ_RECORD_OPEN] snd_pcm_hw_params_set_access!\n");

    err = snd_pcm_hw_params_set_format(*capture_handle, hw_params, ALSA_READ_FORMAT);
    if (err)
    {
        eq_err("[EQ_RECORD_OPEN] Error setting format: %s\n", snd_strerror(err));
        exit(1);
    }
    eq_debug("[EQ_RECORD_OPEN] snd_pcm_hw_params_set_format\n");

    err = snd_pcm_hw_params_set_channels(*capture_handle, hw_params, channels);
    if (err)
    {
        eq_debug("[EQ_RECORD_OPEN] channels = %d\n",channels);
        eq_err("[EQ_RECORD_OPEN] Error setting channels: %s\n", snd_strerror(err));
        exit(1);
    }
    eq_debug("[EQ_RECORD_OPEN] channels = %d\n",channels);

    err = snd_pcm_hw_params_set_buffer_size_near(*capture_handle, hw_params, &bufferSize);
    if (err)
    {
        eq_err("[EQ_RECORD_OPEN] Error setting buffer size (%d): %s\n", bufferSize, snd_strerror(err));
        exit(1);
    }
    eq_debug("[EQ_RECORD_OPEN] bufferSize = %d\n",bufferSize);

    err = snd_pcm_hw_params_set_period_size_near(*capture_handle, hw_params, &periodSize, 0);
    if (err)
    {
        eq_err("[EQ_RECORD_OPEN] Error setting period time (%d): %s\n", periodSize, snd_strerror(err));
        exit(1);
    }
    eq_debug("[EQ_RECORD_OPEN] periodSize = %d\n",periodSize);

    err = snd_pcm_hw_params_set_rate_near(*capture_handle, hw_params, &rate, 0/*&dir*/);
    if (err)
    {
        eq_err("[EQ_RECORD_OPEN] Error setting sampling rate (%d): %s\n", rate, snd_strerror(err));
        exit(1);
    }
    eq_debug("[EQ_RECORD_OPEN] Rate = %d\n", rate);

    /* Write the parameters to the driver */
    err = snd_pcm_hw_params(*capture_handle, hw_params);
    if (err < 0)
    {
        eq_err("[EQ_RECORD_OPEN] Unable to set HW parameters: %s\n", snd_strerror(err));
        exit(1);
    }

    eq_debug("[EQ_RECORD_OPEN] Open record device done \n");
    //if(set_sw_params(*capture_handle,bufferSize,periodSize,NULL) < 0)
    //    exit(1);

    if(hw_params)
        snd_pcm_hw_params_free(hw_params);
}

int alsa_fake_device_write_open(snd_pcm_t** write_handle, int channels,
                                 uint32_t write_sampleRate, int device_flag,
                                 int *socket_fd)
{
    snd_pcm_hw_params_t *write_params;
    snd_pcm_uframes_t write_periodSize = PERIOD_SIZE;
    snd_pcm_uframes_t write_bufferSize = BUFFER_SIZE;
    int write_err;
    int write_dir;
    char bluealsa_device[256] = {0};

    if (device_flag == DEVICE_FLAG_ANALOG_HP) {
        eq_debug("[EQ_WRITE_OPEN] Open PCM: %s\n", JACK_DEVICE_NAME);
        write_err = snd_pcm_open(write_handle, JACK_DEVICE_NAME,
                                 SND_PCM_STREAM_PLAYBACK, 0);
    } else if (device_flag == DEVICE_FLAG_DIGITAL_HP) {
        eq_debug("[EQ_WRITE_OPEN] Open PCM: %s\n", JACK2_DEVICE_NAME);
        write_err = snd_pcm_open(write_handle, JACK2_DEVICE_NAME,
                                 SND_PCM_STREAM_PLAYBACK, 0);
    } else if (device_flag == DEVICE_FLAG_BLUETOOTH) {
        sprintf(bluealsa_device, "%s%s", "bluealsa:HCI=hci0,PROFILE=a2dp,DEV=",
                g_bt_mac_addr);
        eq_debug("[EQ_WRITE_OPEN] Open PCM: %s\n", bluealsa_device);
        write_err = snd_pcm_open(write_handle, bluealsa_device,
                                 SND_PCM_STREAM_PLAYBACK, 0);
    } else if (device_flag == DEVICE_FLAG_BLUETOOTH_BSA) {
        *socket_fd = RK_socket_client_setup(sock_path);
        if (*socket_fd < 0) {
            eq_err("[EQ_WRITE_OPEN] Fail to connect server socket\n");
            return -1;
        } else {
            eq_debug("[EQ_WRITE_OPEN] Socket client connected\n");
            return 0;
        }
    } else {
        eq_debug("[EQ_WRITE_OPEN] Open PCM: %s\n", WRITE_DEVICE_NAME);
        write_err = snd_pcm_open(write_handle, WRITE_DEVICE_NAME,
                                 SND_PCM_STREAM_PLAYBACK, 0);
    }

    if (write_err) {
        eq_err("[EQ_WRITE_OPEN] Unable to open playback PCM device\n");
        return -1;
    }
    eq_debug("[EQ_WRITE_OPEN] interleaved mode\n");

    // snd_pcm_hw_params_alloca(&write_params);
    snd_pcm_hw_params_malloc(&write_params);
    eq_debug("[EQ_WRITE_OPEN] snd_pcm_hw_params_alloca\n");

    snd_pcm_hw_params_any(*write_handle, write_params);

    write_err = snd_pcm_hw_params_set_access(*write_handle, write_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    //write_err = snd_pcm_hw_params_set_access(*write_handle,  write_params, SND_PCM_ACCESS_MMAP_NONINTERLEAVED);
    if (write_err) {
        eq_err("[EQ_WRITE_OPEN] Error setting interleaved mode\n");
        goto failed;
    }
    eq_debug( "[EQ_WRITE_OPEN] interleaved mode\n");

    write_err = snd_pcm_hw_params_set_format(*write_handle, write_params, ALSA_WRITE_FORMAT);
    if (write_err) {
        eq_err("[EQ_WRITE_OPEN] Error setting format: %s\n", snd_strerror(write_err));
        goto failed;
    }
    eq_debug("[EQ_WRITE_OPEN] format successed\n");

    write_err = snd_pcm_hw_params_set_channels(*write_handle, write_params, channels);
    if (write_err) {
        eq_err( "[EQ_WRITE_OPEN] Error setting channels: %s\n", snd_strerror(write_err));
        goto failed;
    }
    eq_debug("[EQ_WRITE_OPEN] channels = %d\n", channels);

    write_err = snd_pcm_hw_params_set_rate_near(*write_handle, write_params, &write_sampleRate, 0/*&write_dir*/);
    if (write_err) {
        eq_err("[EQ_WRITE_OPEN] Error setting sampling rate (%d): %s\n", write_sampleRate, snd_strerror(write_err));
        goto failed;
    }
    eq_debug("[EQ_WRITE_OPEN] setting sampling rate (%d)\n", write_sampleRate);

    write_err = snd_pcm_hw_params_set_buffer_size_near(*write_handle, write_params, &write_bufferSize);
    if (write_err) {
        eq_err("[EQ_WRITE_OPEN] Error setting buffer size (%ld): %s\n", write_bufferSize, snd_strerror(write_err));
        goto failed;
    }
    eq_debug("[EQ_WRITE_OPEN] write_bufferSize = %d\n", write_bufferSize);

    write_err = snd_pcm_hw_params_set_period_size_near(*write_handle, write_params, &write_periodSize, 0);
    if (write_err) {
        eq_err("[EQ_WRITE_OPEN] Error setting period time (%ld): %s\n", write_periodSize, snd_strerror(write_err));
        goto failed;
    }
    eq_debug("[EQ_WRITE_OPEN] write_periodSize = %d\n", write_periodSize);

#if 0
    snd_pcm_uframes_t write_final_buffer;
    write_err = snd_pcm_hw_params_get_buffer_size(write_params, &write_final_buffer);
    eq_debug(" final buffer size %ld \n" , write_final_buffer);

    snd_pcm_uframes_t write_final_period;
    write_err = snd_pcm_hw_params_get_period_size(write_params, &write_final_period, &write_dir);
    eq_debug(" final period size %ld \n" , write_final_period);
#endif

    /* Write the parameters to the driver */
    write_err = snd_pcm_hw_params(*write_handle, write_params);
    if (write_err < 0) {
        eq_err("[EQ_WRITE_OPEN] Unable to set HW parameters: %s\n", snd_strerror(write_err));
        goto failed;
    }

    eq_debug("[EQ_WRITE_OPEN] open write device is successful\n");
    if(set_sw_params(*write_handle, write_bufferSize, write_periodSize, NULL) < 0)
        goto failed;

    if(write_params)
        snd_pcm_hw_params_free(write_params);

    return 0;

failed:
    if(write_params)
        snd_pcm_hw_params_free(write_params);

    snd_pcm_close(*write_handle);
    *write_handle = NULL;
    return -1;
}

int set_sw_params(snd_pcm_t *pcm, snd_pcm_uframes_t buffer_size,
                  snd_pcm_uframes_t period_size, char **msg) {

    snd_pcm_sw_params_t *params;
    snd_pcm_uframes_t threshold;
    char buf[256];
    int err;

    //snd_pcm_sw_params_alloca(&params);
    snd_pcm_sw_params_malloc(&params);
    if ((err = snd_pcm_sw_params_current(pcm, params)) != 0) {
        eq_err("[EQ_SET_SW_PARAMS] Get current params: %s\n", snd_strerror(err));
        goto failed;
    }

    /* start the transfer when the buffer is full (or almost full) */
    threshold = (buffer_size / period_size) * period_size;
    if ((err = snd_pcm_sw_params_set_start_threshold(pcm, params, threshold)) != 0) {
        eq_err("[EQ_SET_SW_PARAMS] Set start threshold: %s: %lu\n", snd_strerror(err), threshold);
        goto failed;
    }

    /* allow the transfer when at least period_size samples can be processed */
    if ((err = snd_pcm_sw_params_set_avail_min(pcm, params, period_size)) != 0) {
        eq_err("[EQ_SET_SW_PARAMS] Set avail min: %s: %lu\n", snd_strerror(err), period_size);
        goto failed;
    }

    if ((err = snd_pcm_sw_params(pcm, params)) != 0) {
        eq_err("[EQ_SET_SW_PARAMS] %s\n", snd_strerror(err));
        goto failed;
    }

    if(params)
        snd_pcm_sw_params_free(params);

    return 0;

failed:
    if(params)
        snd_pcm_sw_params_free(params);

    return -1;
}

int is_mute_frame(short *in,unsigned int size)
{
    int i;
    int mute_count = 0;

    if (!size) {
        eq_err("frame size is zero!!!\n");
        return 0;
    }
    for (i = 0; i < size;i ++) {
        if(in[i] != 0)
        return 0;
    }

    return 1;
}

/* Determine whether to enter the energy saving mode according to
 * the value of the environment variable "EQ_LOW_POWERMODE"
 */
bool low_power_mode_check()
{
    char *value = NULL;

    /* env: "EQ_LOW_POWERMODE=TRUE" or "EQ_LOW_POWERMODE=true" ? */
    value = getenv("EQ_LOW_POWERMODE");
    if (value && (!strcmp("TRUE", value) || !strcmp("true", value)))
        return true;

    return false;
}

/* Check device changing. */
int get_device_flag()
{
    int fd = 0, ret = 0;
    char buff[512] = {0};
    int device_flag = DEVICE_FLAG_LINE_OUT;
#if (ROCKCHIP_SOC == SOC_IS_RK3308)
    const char *path = "/sys/devices/platform/ff560000.acodec/rk3308-acodec-dev/dac_output";
#else /* else is RK3326 */
    const char *path = "/sys/class/switch/h2w/state";
#endif
    FILE *pp = NULL; /* pipeline */
    char *bt_mac_addr = NULL;

    if (g_bt_is_connect == BT_CONNECT_BLUEZ)
        return DEVICE_FLAG_BLUETOOTH;
    else if(g_bt_is_connect == BT_CONNECT_BSA)
        return DEVICE_FLAG_BLUETOOTH_BSA;

    fd = open(path, O_RDONLY);
    if (fd < 0) {
        eq_err("[EQ_DEVICE_FLAG] Open %s failed!\n", path);
        return device_flag;
    }

    ret = read(fd, buff, sizeof(buff));
    if (ret <= 0) {
        eq_err("[EQ_DEVICE_FLAG] Read %s failed!\n", path);
        close(fd);
        return device_flag;
    }

#if (ROCKCHIP_SOC == SOC_IS_RK3308)
    if (strstr(buff, "hp out"))
        device_flag = DEVICE_FLAG_ANALOG_HP;
#else /* else is RK3326 */
    if (strstr(buff, "1"))
        device_flag = DEVICE_FLAG_ANALOG_HP;
    else if (strstr(buff, "2"))
        device_flag = DEVICE_FLAG_DIGITAL_HP;
#endif

    close(fd);

    return device_flag;
}

/* Get device name frome device_flag */
const char *get_device_name(int device_flag)
{
    const char *device_name = NULL;

    switch (device_flag) {
        case DEVICE_FLAG_BLUETOOTH:
        case DEVICE_FLAG_BLUETOOTH_BSA:
            device_name = "BLUETOOTH";
            break;
        case DEVICE_FLAG_ANALOG_HP:
            device_name = JACK_DEVICE_NAME;
            break;
        case DEVICE_FLAG_DIGITAL_HP:
            device_name = JACK2_DEVICE_NAME;
            break;
        case DEVICE_FLAG_LINE_OUT:
            device_name = WRITE_DEVICE_NAME;
            break;
        default:
            break;
    }

    return device_name;
}

void *a2dp_status_listen(void *arg)
{
    int ret = 0;
    char buff[100] = {0};
    struct sockaddr_un clientAddr;
    struct sockaddr_un serverAddr;
    int sockfd;
    socklen_t addr_len;
    char *start = NULL;
    snd_pcm_t* audio_bt_handle;
    char bluealsa_device[256] = {0};
    int retry_cnt = 5;

    sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        eq_err("[EQ_A2DP_LISTEN] Create socket failed!\n");
        return NULL;
    }

    serverAddr.sun_family = AF_UNIX;
    strcpy(serverAddr.sun_path, "/tmp/a2dp_master_status");

    system("rm -rf /tmp/a2dp_master_status");
    ret = bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (ret < 0) {
        eq_err("[EQ_A2DP_LISTEN] Bind Local addr failed!\n");
        return NULL;
    }

    while(1) {
        addr_len = sizeof(clientAddr);
        memset(buff, 0, sizeof(buff));
        ret = recvfrom(sockfd, buff, sizeof(buff), 0, (struct sockaddr *)&clientAddr, &addr_len);
        if (ret <= 0) {
            eq_err("[EQ_A2DP_LISTEN]: %s\n", strerror(errno));
            break;
        }
        eq_debug("[EQ_A2DP_LISTEN] Received a message(%s)\n", buff);

        if (strstr(buff, "status:connect:bsa-source")) {
            if (g_bt_is_connect == BT_DISCONNECT) {
                eq_debug("[EQ_A2DP_LISTEN] bsa bluetooth source is connect\n");
                g_bt_is_connect = BT_CONNECT_BSA;
            }
        } else if (strstr(buff, "status:connect")) {
            start = strstr(buff, "address:");
            if (start == NULL) {
                eq_debug("[EQ_A2DP_LISTEN] Received a malformed connect message(%s)\n", buff);
                continue;
            }
            start += strlen("address:");
            if (g_bt_is_connect == BT_DISCONNECT) {
                //sleep(2);
                memcpy(g_bt_mac_addr, start, sizeof(g_bt_mac_addr));
                sprintf(bluealsa_device, "%s%s", "bluealsa:HCI=hci0,PROFILE=a2dp,DEV=",
                        g_bt_mac_addr);
                retry_cnt = 5;
                while (retry_cnt--) {
                    eq_debug("[EQ_A2DP_LISTEN] try open bluealsa device(%d)\n", retry_cnt + 1);
                    ret = snd_pcm_open(&audio_bt_handle, bluealsa_device,
                                       SND_PCM_STREAM_PLAYBACK, 0);
                    if (ret == 0) {
                        snd_pcm_close(audio_bt_handle);
                        g_bt_is_connect = BT_CONNECT_BLUEZ;
                        break;
                    }
                    usleep(600000); //600ms * 5 = 3s.
                }
            }
        } else if (strstr(buff, "status:disconnect")) {
            g_bt_is_connect = BT_DISCONNECT;
        } else if (strstr(buff, "status:suspend")) {
            g_system_sleep = true;
        } else if (strstr(buff, "status:resume")) {
            g_system_sleep = false;
        } else {
            eq_debug("[EQ_A2DP_LISTEN] Received a malformed message(%s)\n", buff);
        }
    }

    close(sockfd);
    return NULL;
}

static void sigpipe_handler(int sig)
{
    eq_info("[EQ] catch the signal number: %d\n", sig);
}

static int signal_handler()
{
    struct sigaction sa;

    /* Install signal handler for SIGPIPE */
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sigpipe_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGPIPE, &sa, NULL) < 0) {
        eq_err("sigaction() failed: %s", strerror(errno));
        return -1;
    }

    return 0;
}

int main()
{
    int err;
    snd_pcm_t *capture_handle, *write_handle;
    short buffer[READ_FRAME * 2];
    unsigned int sampleRate, channels;
    int mute_frame_thd, mute_frame;
    /* LINE_OUT is the default output device */
    int device_flag, new_flag;
    pthread_t a2dp_status_listen_thread;
    struct rk_wake_lock* wake_lock;
    bool low_power_mode = low_power_mode_check();
    char *silence_data = (char *)calloc(READ_FRAME * 2 * 2, 1);//2ch 16bit
    int socket_fd = -1;

    wake_lock = RK_wake_lock_new("eq_drc_process");

    if(signal_handler() < 0) {
        eq_err("[EQ] Install signal_handler for SIGPIPE failed\n");
        return -1;
    }

    /* Create a thread to listen for Bluetooth connection status. */
    pthread_create(&a2dp_status_listen_thread, NULL, a2dp_status_listen, NULL);

repeat:
    capture_handle = NULL;
    write_handle = NULL;
    err = 0;
    memset(buffer, 0, sizeof(buffer));
    sampleRate = SAMPLE_RATE;
    channels = CHANNEL;
    mute_frame_thd = (int)MUTE_FRAME_THRESHOD;
    mute_frame = 0;
    /* LINE_OUT is the default output device */
    device_flag = DEVICE_FLAG_LINE_OUT;
    new_flag = DEVICE_FLAG_LINE_OUT;

    eq_debug("\n==========EQ/DRC process release version 1.23===============\n");
    alsa_fake_device_record_open(&capture_handle, channels, sampleRate);

    err = alsa_fake_device_write_open(&write_handle, channels, sampleRate, device_flag, &socket_fd);
    if(err < 0) {
        eq_err("first open playback device failed, exit eq\n");
        return -1;
    }

    RK_acquire_wake_lock(wake_lock);

    while (1) {
        err = snd_pcm_readi(capture_handle, buffer , READ_FRAME);
        if (err != READ_FRAME)
            eq_err("====[EQ] read frame error = %d===\n",err);

        if (err < 0) {
            if (err == -EPIPE)
                eq_err("[EQ] Overrun occurred: %d\n", err);

            err = snd_pcm_recover(capture_handle, err, 0);
            // Still an error, need to exit.
            if (err < 0) {
                eq_err("[EQ] Error occured while recording: %s\n", snd_strerror(err));
                usleep(200 * 1000);
                if (capture_handle)
                    snd_pcm_close(capture_handle);
                if (write_handle)
                    snd_pcm_close(write_handle);
                goto repeat;
            }
        }

        if (g_system_sleep)
            mute_frame = mute_frame_thd;
        else if(low_power_mode && is_mute_frame(buffer, channels * READ_FRAME))
            mute_frame ++;
        else
            mute_frame = 0;

        if(mute_frame >= mute_frame_thd) {
            //usleep(30*1000);
            /* Reassign to avoid overflow */
            mute_frame = mute_frame_thd;
            if (write_handle) {
                snd_pcm_close(write_handle);
                RK_release_wake_lock(wake_lock);
                eq_err("[EQ] %d second no playback,close write handle for you!!!\n ", MUTE_TIME_THRESHOD);
                write_handle = NULL;
            }
            continue;
        }

        new_flag = get_device_flag();
        if (new_flag != device_flag) {
            eq_debug("\n[EQ] Device route changed, frome\"%s\" to \"%s\"\n\n",
                   get_device_name(device_flag), get_device_name(new_flag));
            device_flag = new_flag;
            if (write_handle) {
                snd_pcm_close(write_handle);
                write_handle = NULL;
            }
        }

        while (write_handle == NULL && socket_fd < 0) {
            RK_acquire_wake_lock(wake_lock);
            err = alsa_fake_device_write_open(&write_handle, channels, sampleRate, device_flag, &socket_fd);
            if (err < 0 || (write_handle == NULL && socket_fd < 0)) {
                eq_err("[EQ] Route change failed! Using default audio path.\n");
                device_flag = DEVICE_FLAG_LINE_OUT;
                g_bt_is_connect = BT_DISCONNECT;
            }

            if (low_power_mode) {
                int i, num = PERIOD_counts;
                eq_debug("[EQ] feed mute data %d frame\n", num);
                for (i = 0; i < num; i++) {
                    if(write_handle != NULL) {
                        err = snd_pcm_writei(write_handle, silence_data, READ_FRAME);
                        if(err != READ_FRAME)
                            eq_err("====[EQ] write frame error = %d, not %d\n", err, READ_FRAME);
                    } else if (socket_fd >= 0) {
                        err = RK_socket_send(socket_fd, silence_data, READ_FRAME * 4); //2ch 16bit
                        if(err != (READ_FRAME * 4))
                            eq_err("====[EQ] write frame error = %d, not %d\n", err, READ_FRAME * 4);
                    }
                }
            }
        }

        if(write_handle != NULL) {
            //usleep(30*1000);
            err = snd_pcm_writei(write_handle, buffer, READ_FRAME);
            if(err != READ_FRAME)
                eq_err("====[EQ] write frame error = %d===\n",err);

            if (err < 0) {
                if (err == -EPIPE)
                    eq_err("[EQ] Underrun occurred from write: %d\n", err);

                err = snd_pcm_recover(write_handle, err, 0);
                if (err < 0) {
                    eq_err( "[EQ] Error occured while writing: %s\n", snd_strerror(err));
                    usleep(200 * 1000);

                    if (write_handle) {
                        snd_pcm_close(write_handle);
                        write_handle = NULL;
                    }

                    if (device_flag == DEVICE_FLAG_BLUETOOTH)
                        g_bt_is_connect = BT_DISCONNECT;
                }
            }
        }else if (socket_fd >= 0) {
            if (g_bt_is_connect == BT_CONNECT_BSA) {
                err = RK_socket_send(socket_fd, (char *)buffer, READ_FRAME * 4);
                if (err != READ_FRAME * 4 && -EAGAIN != err)
                    eq_err("====[EQ] write frame error = %d===\n", err);

                if (err < 0 && -EAGAIN != err) {
                    if (socket_fd >= 0) {
                        eq_err("[EQ] socket send err: %d, teardown client socket\n", err);
                        RK_socket_client_teardown(socket_fd);
                        socket_fd = -1;
                    }

                    g_bt_is_connect = BT_DISCONNECT;
                }
            } else {
                if(socket_fd >= 0){
                    eq_debug("[EQ] bsa bt source disconnect, teardown client socket\n");
                    RK_socket_client_teardown(socket_fd);
                    socket_fd = -1;
                }
            }
        }
    }

error:
    eq_debug("=== [EQ] Exit eq ===\n");
    if (capture_handle)
        snd_pcm_close(capture_handle);

    if (write_handle)
        snd_pcm_close(write_handle);

    if (socket_fd >= 0)
        RK_socket_client_teardown(socket_fd);

    pthread_cancel(a2dp_status_listen_thread);
    pthread_join(a2dp_status_listen_thread, NULL);

    return 0;
}
