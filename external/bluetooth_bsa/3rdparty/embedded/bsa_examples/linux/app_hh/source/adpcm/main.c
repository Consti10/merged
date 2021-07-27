
// Example program for encoding/decoding .WAV <-> .ADPCM
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "codec.h"

#include "Endian.h"

#pragma warning (disable : 4996)

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



int main_encode(char* inFileName, char* outFileName)
{
    printf("main_encode %s\n", inFileName);

    FILE* f = fopen(inFileName, "rb");
    fseek(f, 0, SEEK_END);
    int inFileSize = ftell(f);
    fseek(f, 0, SEEK_SET);
    u8* inFileBuf = (u8*) malloc(inFileSize);
    fread(inFileBuf, 1, inFileSize, f);
    fclose(f);

    RIFFHeader* riffHeader = (RIFFHeader*) inFileBuf;

    if (endianReadU32Big(&riffHeader->RiffId) != RIFF_ID)
    {
        printf("RiffId error! 0x%x\n", endianReadU32Big(&riffHeader->RiffId));
        return -1;
    }
    if (endianReadU32Big(&riffHeader->WaveId) != WAVE_ID)
    {
        printf("WaveId error! 0x%x\n", endianReadU32Big(&riffHeader->WaveId));
        return -1;
    }

    int waveSize = endianReadU32Little(&riffHeader->Size) - 4;
    printf("waveSize %d\n", waveSize);

    FMTHeader* fmtHeader = 0;
    DATAHeader* dataHeader = 0;

    u8* data = (u8*) (riffHeader + 1);
    while (waveSize && !(fmtHeader && dataHeader))
    {
        u32 id = endianReadU32Big((u32*) data);
        if (id == FMT_ID)
            fmtHeader = (FMTHeader*) data;
        else if (id == DATA_ID)
            dataHeader = (DATAHeader*) data;

        u32 chunkSize = endianReadU32Little((u32*) (data + 4));
        data += chunkSize + 8;
        waveSize -= chunkSize + 8;
    }

    if (!(fmtHeader && dataHeader))
    {
        printf("!(fmtHeader && dataHeader)\n");
        return -1;
    }

    printf("BitsPerSample:%d, NumChannels:%d, Format:%d\n",
                endianReadU16Little(&fmtHeader->BitsPerSample),
                endianReadU16Little(&fmtHeader->NumChannels),
                endianReadU16Little(&fmtHeader->Format));

    printf("SamplesPerSec:%d, AvgBytesPerSec:%d, BlockAlign:%d\n",
                endianReadU32Little(&fmtHeader->SamplesPerSec),
                endianReadU32Little(&fmtHeader->AvgBytesPerSec),
                endianReadU16Little(&fmtHeader->BlockAlign));

    if (endianReadU16Little(&fmtHeader->BitsPerSample) != 16
        || endianReadU16Little(&fmtHeader->NumChannels) != 1
        || endianReadU16Little(&fmtHeader->Format) != 1)
        return -1;

    s16* samples = (s16*) (dataHeader + 1);
    u32 numSamples = endianReadU32Little(&dataHeader->ChunkSize) / 2;

    u8* adpcmData = (u8*) malloc(numSamples / 2);

    CodecState state;
    memset(&state, 0, sizeof(state));
    encode(&state, samples, numSamples, adpcmData);

    FILE* outputFile = fopen(outFileName, "wb");
    fwrite(adpcmData, 1, numSamples / 2, outputFile);
    fclose(outputFile);

    return 0;
}

int main_decode(char* inFileName, char* outFileName)
{
    printf("main_decode\n");
    FILE* f = fopen(inFileName, "rb");
    fseek(f, 0, SEEK_END);
    int inFileSize = ftell(f);
    fseek(f, 0, SEEK_SET);
    u8* inFileBuf = (u8*) malloc(inFileSize);
    fread(inFileBuf, 1, inFileSize, f);
    fclose(f);

    int numSamples = inFileSize * 2;

    int outBufferSize = sizeof(RIFFHeader) + sizeof(FMTHeader) + sizeof(DATAHeader) + numSamples * 2;

    printf("sizeof(RIFFHeader):%d, sizeof(FMTHeader):%d, sizeof(DATAHeader):%d, numSamples:%d\n",
            sizeof(RIFFHeader), sizeof(FMTHeader), sizeof(DATAHeader), numSamples * 2);

    u8* outFileBuf = (u8*) malloc(outBufferSize);

    RIFFHeader* riffHeader = (RIFFHeader*) outFileBuf;

    endianWriteU32Big(&riffHeader->RiffId, RIFF_ID);
    endianWriteU32Little(&riffHeader->Size, outBufferSize - sizeof(RIFFHeader) + 4);
    endianWriteU32Big(&riffHeader->WaveId, WAVE_ID);

    FMTHeader* fmtHeader = (FMTHeader*) (riffHeader + 1);

    endianWriteU32Big(&fmtHeader->FmtId, FMT_ID);
    endianWriteU32Little(&fmtHeader->ChunkSize, sizeof(FMTHeader) - 8);
    endianWriteU16Little(&fmtHeader->Format, 1);
    endianWriteU16Little(&fmtHeader->NumChannels, 1);
    endianWriteU32Little(&fmtHeader->SamplesPerSec, 16000);
    endianWriteU32Little(&fmtHeader->AvgBytesPerSec, 16000 * 2);
    endianWriteU16Little(&fmtHeader->BlockAlign, 2);
    endianWriteU16Little(&fmtHeader->BitsPerSample, 16);

    DATAHeader* dataHeader = (DATAHeader*) (fmtHeader + 1);

    endianWriteU32Big(&dataHeader->DataId, DATA_ID);
    endianWriteU32Little(&dataHeader->ChunkSize, numSamples * 2);

    CodecState state;
    memset(&state, 0, sizeof(state));
    printf("encoded numSamples:%d, inFileSize:%d\n", numSamples, inFileSize);
    decode68000(&state, inFileBuf, numSamples, (s16*) (dataHeader + 1));
    printf("decoded numSamples:%d, outBufferSize:%d\n", numSamples, outBufferSize);

    FILE* outputFile = fopen(outFileName, "wb");
    fwrite(outFileBuf, 1, outBufferSize, outputFile);
    fclose(outputFile);

    u8* decoded;
    decoded = (u8*) (dataHeader + 1);
    FILE* outputFile_2 = fopen("output_wo_hd", "wb");
    fwrite(decoded, 1, numSamples * 2, outputFile_2);
    fclose(outputFile_2);

    return 0;
}

int main(int argc, char** argv)
{
    int result;
    if (argc != 4)
    {
        printf("Usage: <encode|decode> <input file> <output file>\n");
        return 0;
    }

    initDecode68000();

    if (!strcmp(argv[1], "encode"))
    {
        result = main_encode(argv[2], argv[3]);
        if(result < 0)
        {
            printf("failure \n");
        }
        else
        {
            printf("success \n");
        }
    }
    else if (!strcmp(argv[1], "decode"))
    {
        result = main_decode(argv[2], argv[3]);
        if(result < 0)
        {
            printf("failure \n");
        }
        else
        {
            printf("success \n");
        }
    }
    else
    {
        printf("bad args\n");
    }

    return 0;
}
