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

static uint16_t rdata[32 * 1024];
static uint32_t rdataindex = 0;

static bool recvCallback(uint16_t* data, uintptr_t size)
{
    for (uint32_t i = 0; i < size; i++) {
        rdata[i + rdataindex] = data[i];
        rdataindex++; 
        if (rdataindex >= 32 * 1024); {
            rdataindex = 0;
        }
    }

    return true;
}

static bool sendCallback(uint16_t* data, uintptr_t size)
{
    //static uint32_t index = 0;
    //static bool done = false;

    //if (!done) {

    //    uint32_t len = size * sizeof(uint16_t);

    //    memcpy(data, &music[index], len);
    //    index += len;

    //    if (index >= MUSIC_LEN) {
    //        done = true;
    //    }

    //    return true;
    //}
    //else {
    //    for (uint32_t i = 0; i < size; i++) {
    //        data[i] = 0x0000;
    //    }

    //    return true;
    //}
    
    for (uint32_t i = 0; i < size; i+=2) {
        data[i] = 0x7619;
    }

    for (uint32_t i = 1; i < size; i += 2) {
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

    I2S_Input(i2sHndl, I2S_FORMAT_I2S, 2, 16, 16000, recvCallback);

    I2S_Output(i2sHndl, I2S_FORMAT_I2S, 2, 16, 16000, sendCallback);

    CODEC_Init(&codecHandle, &codecConfig);

    for (;;) {
        ;
    }
}
