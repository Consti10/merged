
#ifndef codec_h
#define codec_h

#include "Types.h"

typedef struct
{
    int valprev;
    int index;
}CodecState;

void encode(CodecState* state, s16* input, int numSamples, u8* output);
void decode(CodecState* state, u8* input, int numSamples, s16* output);

void initDecode68000();
void decode68000(CodecState* state, u8* input, int numSamples, s16* output);


#endif
