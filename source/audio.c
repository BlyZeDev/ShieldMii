#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include <3ds.h>
#include <citro2d.h>
#include <citro3d.h>

#include "define.h"

typedef struct
{
    u16 type;
    u32 target;
} bcstmRef;

static u8 read8(s8* buffer, size_t* cursorPtr)
{
    u8 value;
    memcpy(&value, buffer + *cursorPtr, 1);
    *cursorPtr += 1;
    return value;
}

static u16 read16(s8* buffer, size_t* cursorPtr)
{
    u16 value;
    memcpy(&value, buffer + *cursorPtr, 2);
    *cursorPtr += 2;
    return value;
}

static u32 read32(s8* buffer, size_t* cursorPtr)
{
    u32 value;
    memcpy(&value, buffer + *cursorPtr, 4);
    *cursorPtr += 4;
    return value;
}

static s8 initAudio(bcstm* audioPtr)
{
    u32 magic = read32(audioPtr->audioBuffer, &audioPtr->cursor);
    audioPtr->isLittleEndian = read16(audioPtr->audioBuffer, &audioPtr->cursor) == 0xFEFF;
    audioPtr->infoOffset = 0;
    audioPtr->dataOffset = 0;

    if (magic != 0x4D545343)
    {
        free(audioPtr->audioBuffer);
        return -1;
    }

    audioPtr->cursor = 0x10;
    u16 sbc = read16(audioPtr->audioBuffer, &audioPtr->cursor);
    audioPtr->cursor += 2;

    for (u16 i = 0; i < sbc; i++)
    {
        u16 sec = read16(audioPtr->audioBuffer, &audioPtr->cursor);
        audioPtr->cursor += 2;
        u32 off = read32(audioPtr->audioBuffer, &audioPtr->cursor);
        audioPtr->cursor += 4;

        if (sec == 0x4000)
            audioPtr->infoOffset = off;
        if (sec == 0x4002)
            audioPtr->dataOffset = off;
    }

    if (audioPtr->dataOffset == 0 || audioPtr->infoOffset == 0)
    {
        free(audioPtr->audioBuffer);
        return -2;
    }

    audioPtr->cursor = audioPtr->infoOffset + 0x20;

    if (read8(audioPtr->audioBuffer, &audioPtr->cursor) != 2)
    {
        free(audioPtr->audioBuffer);
        return -3;
    }

    audioPtr->isLooping = read8(audioPtr->audioBuffer, &audioPtr->cursor);
    audioPtr->channelCount = read8(audioPtr->audioBuffer, &audioPtr->cursor);

    if (audioPtr->channelCount > 2)
    {
        free(audioPtr->audioBuffer);
        return -4;
    }

    audioPtr->cursor = audioPtr->infoOffset + 0x24;
    audioPtr->sampleRate = read32(audioPtr->audioBuffer, &audioPtr->cursor);

    u32 loopPos = read32(audioPtr->audioBuffer, &audioPtr->cursor);
    u32 loopEnd = read32(audioPtr->audioBuffer, &audioPtr->cursor);

    audioPtr->numBlocks = read32(audioPtr->audioBuffer, &audioPtr->cursor);
    audioPtr->blockSize = read32(audioPtr->audioBuffer, &audioPtr->cursor);
    audioPtr->blockSamples = read32(audioPtr->audioBuffer, &audioPtr->cursor);
    audioPtr->cursor += 4;
    audioPtr->lastBlockSamples = read32(audioPtr->audioBuffer, &audioPtr->cursor);
    audioPtr->lastBlockSize = read32(audioPtr->audioBuffer, &audioPtr->cursor);

    audioPtr->loopStart = loopPos / audioPtr->blockSamples;
    audioPtr->loopEnd = (loopEnd % audioPtr->blockSamples ? audioPtr->numBlocks : loopEnd / audioPtr->blockSamples);

    while (read32(audioPtr->audioBuffer, &audioPtr->cursor) != 0x4102);
    audioPtr->cursor -= 4;

    u32 startTable = audioPtr->cursor - 4;
    bcstmRef channelRefs[2] = {0};
    for (u8 i = 0; i < audioPtr->channelCount; i++)
    {
        channelRefs[i].type = read16(audioPtr->audioBuffer, &audioPtr->cursor);
        audioPtr->cursor += 2;
        channelRefs[i].target = startTable + read32(audioPtr->audioBuffer, &audioPtr->cursor);
    }

    for (u8 i = 0; i < audioPtr->channelCount; i++)
    {
        audioPtr->cursor = channelRefs[i].target;
        if (read16(audioPtr->audioBuffer, &audioPtr->cursor) != 0x300) continue;

        audioPtr->cursor += 2;
        audioPtr->cursor += read32(audioPtr->audioBuffer, &audioPtr->cursor) - 8;
        memcpy(audioPtr->adpcmCoefs[i], audioPtr->audioBuffer + audioPtr->cursor, sizeof(u16) * 16);

        audioPtr->cursor += sizeof(u16) * 16;
        memcpy(&(audioPtr->adpcmData[i][0]), audioPtr->audioBuffer + audioPtr->cursor, sizeof(ndspAdpcmData));

        audioPtr->cursor += sizeof(ndspAdpcmData);
        memcpy(&(audioPtr->adpcmData[i][1]), audioPtr->audioBuffer + audioPtr->cursor, sizeof(ndspAdpcmData));

        audioPtr->cursor += sizeof(ndspAdpcmData);
        audioPtr->cursor += 2;
    }

    audioPtr->cursor = audioPtr->dataOffset + 0x20;

    return 0;
}

static void playAudio(bcstm* audioPtr)
{
    audioPtr->currentBlock = 0;
    for (u8 i = 0; i < audioPtr->channelCount; i++)
    {
        audioPtr->channel[i] = 0;

        while (audioPtr->channel[i] < 24 && ((audioPtr->activeChannels >> audioPtr->channel[i]) & 1))
        {
            audioPtr->channel[i]++;
        }

        if (audioPtr->channel[i] == 24) return;

        audioPtr->activeChannels |= 1 << audioPtr->channel[i];
        ndspChnWaveBufClear(audioPtr->channel[i]);
        
        static float mix[16];
        ndspChnSetFormat(audioPtr->channel[i], NDSP_FORMAT_ADPCM | NDSP_3D_SURROUND_PREPROCESSED);
        ndspChnSetRate(audioPtr->channel[i], audioPtr->sampleRate);
        
        if (audioPtr->channelCount == 1)
        {
            mix[0] = mix[1] = 0.5f;
        }
        else if (audioPtr->channelCount == 2)
        {
            if (i == 0)
            {
                mix[0] = 0.8f;
                mix[1] = 0.0f;
                mix[2] = 0.2f;
                mix[3] = 0.0f;
            }
            else
            {
                mix[0] = 0.0f;
                mix[1] = 0.8f;
                mix[2] = 0.0f;
                mix[3] = 0.2f;
            }
        }
        ndspChnSetMix(audioPtr->channel[i], mix);
        ndspChnSetAdpcmCoefs(audioPtr->channel[i], audioPtr->adpcmCoefs[i]);

        for (u8 j = 0; j < BUFFER_COUNT; j++)
        {
            memset(&(audioPtr->waveBuf[i][j]), 0, sizeof(ndspWaveBuf));
            audioPtr->waveBuf[i][j].status = NDSP_WBUF_DONE;
            audioPtr->bufferData[i][j] = linearAlloc(audioPtr->blockSize);
        }
    }
}

static void fillBuffers(bcstm* audioPtr)
{
    for (u8 bufIndex = 0; bufIndex < BUFFER_COUNT; bufIndex++)
    {
        if (audioPtr->waveBuf[0][bufIndex].status != NDSP_WBUF_DONE) continue;
        if (audioPtr->channelCount == 2 && audioPtr->waveBuf[1][bufIndex].status != NDSP_WBUF_DONE) continue;

        if (audioPtr->isLooping && audioPtr->currentBlock == audioPtr->loopEnd)
        {
            audioPtr->currentBlock = audioPtr->loopStart;
            audioPtr->cursor = audioPtr->dataOffset + 0x20 + audioPtr->blockSize * audioPtr->channelCount * audioPtr->loopStart;
        }

        if (!audioPtr->isLooping && audioPtr->currentBlock == audioPtr->loopEnd)
        {
            audioPtr->stop = true;
            return;
        }

        for (u8 channelIndex = 0; channelIndex < audioPtr->channelCount; channelIndex++)
        {
            ndspWaveBuf* buf = &(audioPtr->waveBuf[channelIndex][bufIndex]);
            memset(buf, 0, sizeof(ndspWaveBuf));
            buf->data_adpcm = audioPtr->bufferData[channelIndex][bufIndex];
            memcpy(buf->data_adpcm, audioPtr->audioBuffer + audioPtr->cursor, (audioPtr->currentBlock == audioPtr->numBlocks - 1) ? audioPtr->lastBlockSize : audioPtr->blockSize);
            audioPtr->cursor += (audioPtr->currentBlock == audioPtr->numBlocks - 1) ? audioPtr->lastBlockSize : audioPtr->blockSize;
            DSP_FlushDataCache(buf->data_adpcm, audioPtr->blockSize);

            if (audioPtr->currentBlock == 0)
                buf->adpcm_data = &(audioPtr->adpcmData[channelIndex][0]);
            else if (audioPtr->currentBlock == audioPtr->loopStart)
                buf->adpcm_data = &(audioPtr->adpcmData[channelIndex][1]);

            buf->nsamples = (audioPtr->currentBlock == audioPtr->numBlocks - 1) ? audioPtr->lastBlockSamples : audioPtr->blockSamples;

            ndspChnWaveBufAdd(audioPtr->channel[channelIndex], buf);
        }
        audioPtr->currentBlock++;
    }
}

static void updateAudio(bcstm* audioPtr)
{
    u32 currentTime = svcGetSystemTick();
    if (currentTime - audioPtr->lastTime > 1e8)
    {
        fillBuffers(audioPtr);
        audioPtr->lastTime = currentTime;
    }
}

static void audioThread(void* dataPtr)
{
    bcstm* audioPtr = (bcstm*)dataPtr;

    s8 result = initAudio(audioPtr);
    if (result < 0) return;

    playAudio(audioPtr);
    while (!audioPtr->stop)
    {
        updateAudio(audioPtr);
    }

    for (u8 i = 0; i < audioPtr->channelCount; i++)
    {
        ndspChnWaveBufClear(audioPtr->channel[i]);
        ndspChnReset(audioPtr->channel[i]);
        audioPtr->activeChannels &= ~(1 << audioPtr->channel[i]);

        for (u8 j = 0; j < BUFFER_COUNT; j++)
        {
            linearFree(audioPtr->bufferData[i][j]);
        }
    }

    free(audioPtr->audioBuffer);
}

static size_t loadBcstmFile(const char* path, s8** bufferPtr)
{
    size_t size;

    FILE* filePtr = fopen(path, "rb");
    fseek(filePtr, 0, SEEK_END);
    size = ftell(filePtr);
    fseek(filePtr, 0, SEEK_SET);

    if (size != 0)
    {
        *bufferPtr = calloc(1, size);
        fread(*bufferPtr, 1, size, filePtr);
    }

    fclose(filePtr);
    return size;
}

bcstm* loadBcstm(const char* path)
{
    bcstm* audioPtr = (bcstm*)calloc(1, sizeof(bcstm));

    audioPtr->audioSize = loadBcstmFile(path, &audioPtr->audioBuffer);
    if (audioPtr->audioSize == 0)
    {
        free(audioPtr);
        return NULL;
    }

    audioPtr->cursor = 0;
    audioPtr->lastTime = 0;
    audioPtr->currentBlock = 0;
    audioPtr->stop = false;
    audioPtr->activeChannels = 0;

    return audioPtr;
}

void playBcstm(bcstm* audioPtr)
{
    audioPtr->playingThread = threadCreate(audioThread, audioPtr, 0x1000, 0x3F, -1, false);
}

void stopBcstm(bcstm* audioPtr)
{
    if (audioPtr->playingThread)
    {
        audioPtr->stop = true;
        threadJoin(audioPtr->playingThread, U64_MAX);
        threadFree(audioPtr->playingThread);
    }
}

void freeBcstm(bcstm** audioPtrPtr)
{
    bcstm* audioPtr = *audioPtrPtr;

    free(audioPtr);
    *audioPtrPtr = NULL;
}