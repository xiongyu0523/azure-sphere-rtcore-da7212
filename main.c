/* Copyright (c) Codethink Ltd. All rights reserved.
   Licensed under the MIT License. */
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>

#include "VectorTable.h"
#include "CPUFreq.h"
#include "NVIC.h"
#include "UART.h"
#include "Print.h"
#include "I2S.h"

#include "fsl_codec_common.h"
#include "fsl_dialog7212.h"
#include "fsl_codec_adapter.h"

#include "music.h"

UART *debug;

static codec_handle_t codecHandle;
static da7212_config_t da7212Config = {
    .i2cConfig = {.codecI2CInstance = MT3620_UNIT_ISU2, .codecI2CSourceClock = 0},
    .dacSource = kDA7212_DACSourceInputStream,
    .slaveAddress = DA7212_ADDRESS,
    .protocol = kDA7212_BusI2S,
    .format = {.mclk_HZ = 16000000U, .sampleRate = 16000, .bitWidth = 16},
    .isMaster = true,
};
static codec_config_t codecConfig = { 
    .codecDevType = kCODEC_DA7212, 
    .codecDevConfig = &da7212Config 
};

#define AUDIO_IN_BUF_SIZE (16000 * 2) // a ring buffer to hold last second of audio 
static uint16_t AudioInRingBuffer[AUDIO_IN_BUF_SIZE / 2];
static uint32_t AudioInRingBufferIndex = 0;

static bool recvCallback(uint16_t* data, uintptr_t size)
{
    if (AudioInRingBufferIndex + size <= AUDIO_IN_BUF_SIZE) {
        memcpy(&AudioInRingBuffer[AudioInRingBufferIndex / 2], data, size);
        AudioInRingBufferIndex += size;
        if (AudioInRingBufferIndex >= AUDIO_IN_BUF_SIZE) {
            AudioInRingBufferIndex = 0;
        }
    } else {
        uint32_t remain = AUDIO_IN_BUF_SIZE - AudioInRingBufferIndex;
        uint32_t wrap = size - remain;
        memcpy(&AudioInRingBuffer[AudioInRingBufferIndex / 2], data, remain);
        memcpy(&AudioInRingBuffer[0], &data[remain], wrap);
        AudioInRingBufferIndex = wrap;
    }

    return true;
}

static bool sendCallback(uint16_t* data, uintptr_t size)
{
    //static uint32_t index = 0;
    //static bool done = false;

    //if (!done) {

    //    if (index + size < MUSIC_LEN) {
    //        memcpy(data, &music[index], size);
    //        index += size;
    //    } else {
    //        uint32_t remain = MUSIC_LEN - index;
    //        memcpy(data, &music[index], remain);
    //        memset(&data[remain / 2], 0x00, size - remain);
    //        done = true;
    //    }

    //    return true;
    //}

    //return false;

    uintptr_t chunk = (sizeof(int16_t) * 2);
    if (size % chunk) {
        while (1);
        return false;
    }
    
    for (uint32_t i = 0; i < size / 2; i += 2) {
        data[i] = 0x7619;
    }

    for (uint32_t i = 1; i < size / 2; i += 2) {
        data[i] = 0x0888;
    }

    return true;
}

// helper for debug
bool start = false;

_Noreturn void RTCoreMain(void)
{
    while (start == false);

    VectorTableInit();
    CPUFreq_Set(197600000);

    I2S *i2sHndl = I2S_Open(MT3620_UNIT_I2S0, 16000000);

    I2S_Input(i2sHndl, I2S_FORMAT_I2S, 1, 16, 16000, recvCallback);
    I2S_Output(i2sHndl, I2S_FORMAT_I2S, 2, 16, 16000, sendCallback);
    CODEC_Init(&codecHandle, &codecConfig);

    for (;;) {
        ;
    }
}
