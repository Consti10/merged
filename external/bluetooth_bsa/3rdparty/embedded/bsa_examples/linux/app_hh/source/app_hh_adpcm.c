/*****************************************************************************
 **
 **  Name:           app_hh_adpcm.c
 **
 **  Description:    Bluetooth HID Host application
 **
 **  Copyright (c) 2019, Cypress Semiconductor., All Rights Reserved.
 **  Proprietary and confidential.
 **
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>


#include "app_hh.h"
#include "bsa_trace.h"
#include "app_utils.h"
#include "adpcm/codec.h"
#include "adpcm/Endian.h"

#define AH_ADPCM_SAMPLE_RATE 16000
#define AH_ADPCM_NUM_OF_CHANNELS 1
#define AH_ADPCM_BIT_PER_SAMPLE 16
#define AH_ADPCM_MAX_FRAME_SIZE 6*960
#define AH_ADPCM_HEADER_SIZE 6

#define AH_ADPCM_DEBUG FALSE

/*
 * Use previous predicted ADPCM value and index into step size table
 * from audio report header.
 */
#define AH_ADPCM_INFO_FROM_REPORT_HEADER FALSE

/*
 * Swap decoded data byte.
 */
#define AH_ADPCM_SWAP_ENDIAN FALSE


enum
{
    RIFF_ID = 'RIFF',
    WAVE_ID = 'WAVE',
    FMT_ID = 'fmt ',
    DATA_ID = 'data'
};

typedef struct RIFFHeaderTag
{
    u32 RiffId;
    u32 Size;
    u32 WaveId;
} RIFFHeader;

typedef struct FMTHeaderTag
{
    u32 FmtId;
    u32 ChunkSize;
    u16 Format;
    u16 NumChannels;
    u32 SamplesPerSec;
    u32 AvgBytesPerSec;
    u16 BlockAlign;
    u16 BitsPerSample;
} FMTHeader;

typedef struct DATAHeaderTag
{
    u32 DataId;
    u32 ChunkSize;
} DATAHeader;


static UINT16 adpcm_prev_pred;
static UINT8 adpcm_index;


/*******************************************************************************
 **
 ** Function         app_hh_adpcm_header_parse
 **
 ** Description      This function parse ADPCM header.
 **
 ** Parameters
 **
 ** Returns          0 if successful, otherwise the error number
 **
 *******************************************************************************/
int app_hh_adpcm_header_parse(UINT8 *p_data)
{
    /* header
     * SEQ - 2 bytes
     * ID  - 1 byte
     * Prev PRED - 2 bytes
     * Index - 1 byte
     */
    UINT16 adpcm_seq = 0;
    UINT8 remote_con_id;
    UINT8 temp;

    STREAM_TO_UINT8(temp, p_data);
    adpcm_seq = temp;
    adpcm_seq = adpcm_seq << 8;
    STREAM_TO_UINT8(temp, p_data);
    adpcm_seq = adpcm_seq | temp;
    STREAM_TO_UINT8(remote_con_id, p_data);
    STREAM_TO_UINT8(temp, p_data);
    adpcm_prev_pred = temp;
    adpcm_prev_pred = adpcm_prev_pred << 8;
    STREAM_TO_UINT8(temp, p_data);
    adpcm_prev_pred = adpcm_prev_pred | temp;
    STREAM_TO_UINT8(adpcm_index, p_data);

#if defined(AH_ADPCM_DEBUG) && (AH_ADPCM_DEBUG == TRUE)
    APP_DEBUG1("adpcm_seq:%d, remote_con_id:%d, prev_pred:0x%x, index:%d",
            adpcm_seq, remote_con_id, adpcm_prev_pred, adpcm_index);
#endif

    return 0;
}


/*******************************************************************************
 **
 ** Function         app_hh_adpcm_init
 **
 ** Description      This function initialize ADPCM decoder
 **
 ** Parameters
 **
 ** Returns          0 if successful, otherwise the error number
 **
 *******************************************************************************/
int app_hh_adpcm_init(void)
{
   initDecode68000();

   return 0;
}

#if defined(AH_ADPCM_DEBUG) && (AH_ADPCM_DEBUG == TRUE)
static FILE *outputFile_1, *outputFile_2, *outputFile_3;
#endif

static CodecState state;

/*******************************************************************************
 **
 ** Function         app_hh_adpcm_decode
 **
 ** Description      This function processes HID reports containing ADPCM encoded
 **                  data.
 **
 ** Parameters
 **
 ** Returns          0 if successful, otherwise the error number
 **
 *******************************************************************************/
int app_hh_adpcm_decode(tAPP_HH_DEVICE *p_hh_dev, UINT8 *p_data, UINT16 length, UINT16 packet_count)
{
    int numSamples;
    int outBufferSize;
    ssize_t dummy;

#if defined(AH_ADPCM_SWAP_ENDIAN) && (AH_ADPCM_SWAP_ENDIAN == TRUE)
    int i;
    UINT8 temp;
#endif

#if defined(AH_ADPCM_DEBUG) && (AH_ADPCM_DEBUG == TRUE)
    APP_DEBUG1("app_hh_adpcm_decode length:%d, packet_count:%d", length, packet_count);
    APP_DUMP("ADPCM Data", p_data, length);
#endif

    numSamples = (length - 7) * 2;
    if(numSamples < 256)
    {
        APP_ERROR1("length is wrong length:%d, numSamples:%d", length, numSamples);
        return -1;
    }
    outBufferSize = sizeof(RIFFHeader) + sizeof(FMTHeader) + sizeof(DATAHeader) + numSamples * 2;
#if defined(AH_ADPCM_DEBUG) && (AH_ADPCM_DEBUG == TRUE)
    APP_DEBUG1("outBufferSize:%d, numSamples:%d", outBufferSize, numSamples);
#endif

    p_data++;
    app_hh_adpcm_header_parse(p_data);

    u8* outFileBuf = (u8*) malloc(outBufferSize);
    u8* decoded;

    RIFFHeader* riffHeader = (RIFFHeader*) outFileBuf;

    endianWriteU32Big(&riffHeader->RiffId, RIFF_ID);
    endianWriteU32Little(&riffHeader->Size, outBufferSize - sizeof(RIFFHeader) + 4);
    endianWriteU32Big(&riffHeader->WaveId, WAVE_ID);

    FMTHeader* fmtHeader = (FMTHeader*) (riffHeader + 1);

    endianWriteU32Big(&fmtHeader->FmtId, FMT_ID);
    endianWriteU32Little(&fmtHeader->ChunkSize, sizeof(FMTHeader) - 8);
    endianWriteU16Little(&fmtHeader->Format, 1);
    endianWriteU16Little(&fmtHeader->NumChannels, AH_ADPCM_NUM_OF_CHANNELS);
    endianWriteU32Little(&fmtHeader->SamplesPerSec, AH_ADPCM_SAMPLE_RATE);
    endianWriteU32Little(&fmtHeader->AvgBytesPerSec, 16000 * 2);
    endianWriteU16Little(&fmtHeader->BlockAlign, 2);
    endianWriteU16Little(&fmtHeader->BitsPerSample, AH_ADPCM_BIT_PER_SAMPLE);

    DATAHeader* dataHeader = (DATAHeader*) (fmtHeader + 1);

    endianWriteU32Big(&dataHeader->DataId, DATA_ID);
    endianWriteU32Little(&dataHeader->ChunkSize, numSamples * 2);

    if(packet_count == 0)
    {
        memset(&state, 0, sizeof(state));
    }

#if defined(AH_ADPCM_DEBUG) && (AH_ADPCM_DEBUG == TRUE)
    APP_DEBUG1("adpcm_prev_pred:%d, adpcm_index:%d", adpcm_prev_pred, adpcm_index);
#endif

#if defined(AH_ADPCM_INFO_FROM_REPORT_HEADER) && (AH_ADPCM_INFO_FROM_REPORT_HEADER == TRUE)
    state.valprev = (INT16)adpcm_prev_pred;
    state.index = (INT16)adpcm_index;
#endif

#if defined(AH_ADPCM_DEBUG) && (AH_ADPCM_DEBUG == TRUE)
    APP_DEBUG1("state.valprev:%d, state.index:%d", state.valprev, state.index);
#endif

    p_data = p_data + AH_ADPCM_HEADER_SIZE;

#if defined(AH_ADPCM_DEBUG) && (AH_ADPCM_DEBUG == TRUE)
    APP_DEBUG1("numSamples:%d", numSamples);
    APP_DUMP("ADPCM Data encoded", p_data, numSamples / 2 );

    if(packet_count == 0)
    {
        /* saving decoded data as it is */
        outputFile_1 = fopen("encoded", "wb");
        fwrite(p_data, 1, numSamples/2, outputFile_1);
    }
    else if( (0 < packet_count) && (packet_count < 30))
    {
        fwrite(p_data, 1, numSamples/2, outputFile_1);
    }
    else if( packet_count == 30)
    {
        fclose(outputFile_1);
    }
#endif

    decode68000(&state, p_data, numSamples, (s16*) (dataHeader + 1));

    decoded = (u8*) (dataHeader + 1);

#if defined(AH_ADPCM_DEBUG) && (AH_ADPCM_DEBUG == TRUE)
    APP_DEBUG1("numSamplesx2:%d", numSamples * 2);
    APP_DUMP("ADPCM Data Decoded", decoded, numSamples * 2);

    if(packet_count == 0)
    {
        /* saving decoded data as it is */
        outputFile_2 = fopen("decoded", "wb");
        fwrite(decoded, 1, AH_ADPCM_NUM_OF_CHANNELS * numSamples * 2, outputFile_2);
    }
    else if( (0 < packet_count) && (packet_count < 30))
    {
        fwrite(decoded, 1, AH_ADPCM_NUM_OF_CHANNELS * numSamples * 2, outputFile_2);
    }
    else if( packet_count == 30)
    {
        fclose(outputFile_2);
    }
#endif

#if defined(AH_ADPCM_SWAP_ENDIAN) && (AH_ADPCM_SWAP_ENDIAN == TRUE)
    /* Convert to endian ordering. */
    for( i = 0 ; i < (numSamples * 2) ; i = i+2 )
    {
        temp = decoded[i];
        decoded[i] = decoded[i+1];
        decoded[i+1] = temp;
    }
#endif

#if defined(AH_ADPCM_DEBUG) && (AH_ADPCM_DEBUG == TRUE)
    APP_DEBUG1("after swap:%d", numSamples * 2);
    APP_DUMP("ADPCM Data Decoded after swap", decoded, numSamples * 2);

    if(packet_count == 0)
    {
        /* saving decoded data as it is */
        outputFile_3 = fopen("decoded_swaped", "wb");
        fwrite(decoded, 1, AH_ADPCM_NUM_OF_CHANNELS * numSamples * 2, outputFile_3);
    }
    else if( (0 < packet_count) && (packet_count < 30))
    {
        fwrite(decoded, 1, AH_ADPCM_NUM_OF_CHANNELS * numSamples * 2, outputFile_3);
    }
    else if( packet_count == 30)
    {
        fclose(outputFile_3);
    }
#endif

#ifdef PCM_ALSA
    app_alsa_playback_write(decoded, AH_ADPCM_NUM_OF_CHANNELS * numSamples * 2);
#else /* PCM_ALSA */

#if defined(AH_ADPCM_DEBUG) && (AH_ADPCM_DEBUG == TRUE)
    APP_DEBUG1("write audio data only:%d", AH_ADPCM_NUM_OF_CHANNELS * numSamples * 2);
#endif
    /* write the decoded data to file */
    dummy = write(p_hh_dev->audio.fd, decoded, AH_ADPCM_NUM_OF_CHANNELS * numSamples * 2);
    (void)dummy;
#endif /* !PCM_ALSA */

#if defined(AH_ADPCM_DEBUG) && (AH_ADPCM_DEBUG == TRUE)
    APP_DEBUG1("packet_count:%d", packet_count);
#endif

    free(outFileBuf);

    return 0;
}
