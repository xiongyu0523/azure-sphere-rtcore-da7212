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
#include "GPIO.h"

#include "fsl_codec_common.h"
#include "fsl_dialog7212.h"
#include "fsl_codec_adapter.h"

#include "music.h"

#define GPIO_BUTTON_A   12
#define GPIO_BUTTON_B   13
#define GPIO_PLAY_R     45
#define GPIO_PLAY_G     46
#define GPIO_PLAY_B     47
#define RED_ON()        GPIO_Write(GPIO_PLAY_R, 0)
#define RED_OFF()       GPIO_Write(GPIO_PLAY_R, 1)
#define GREEN_ON()      GPIO_Write(GPIO_PLAY_G, 0)
#define GREEN_OFF()     GPIO_Write(GPIO_PLAY_G, 1)
#define BLUE_ON()       GPIO_Write(GPIO_PLAY_B, 0)
#define BLUE_OFF()      GPIO_Write(GPIO_PLAY_B, 1)
#define AUDIO_BUF_SAMPLE (5 * 16000) // audio buffer to hold last 5 seconds of microphone input 

static bool startRecord = false;
static bool startPlay = false;

static uint16_t AudioBuffer[AUDIO_BUF_SAMPLE];
static uint32_t AudioBufferIndex = 0;

//bool start = false;

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

static bool recvCallback(uint16_t* data, uintptr_t size)
{
    bool isRecordDone = false;
    uint32_t sample2recv = size / 2;

    if (startRecord) {
        if (AudioBufferIndex + sample2recv <= AUDIO_BUF_SAMPLE) {
            memcpy(&AudioBuffer[AudioBufferIndex], data, sample2recv * 2);
            AudioBufferIndex += sample2recv;
            if (AudioBufferIndex == AUDIO_BUF_SAMPLE) {
                isRecordDone = true;
            }
        } else {
            uint32_t freeSample = AUDIO_BUF_SAMPLE - AudioBufferIndex;
            memcpy(&AudioBuffer[AudioBufferIndex], data, freeSample * 2);
            isRecordDone = true;
        }

        if (isRecordDone) {
            AudioBufferIndex = 0;
            startRecord = false;
            RED_OFF();
            GREEN_ON();
        }
    }

    return true;
}

static bool sendCallback(uint16_t* data, uintptr_t size)
{
    bool isPlayDone = false;
    uint32_t sample2send = size / 2 / 2;

    if (startPlay) {

        if (AudioBufferIndex + sample2send <= AUDIO_BUF_SAMPLE) {

            for (uint32_t i = 0; i < sample2send; i++) {
                data[2 * i] = AudioBuffer[AudioBufferIndex + i];
                data[2 * i + 1] = AudioBuffer[AudioBufferIndex + i];
            }

            AudioBufferIndex += sample2send;
            if (AudioBufferIndex == AUDIO_BUF_SAMPLE) {
                isPlayDone = true;
            }
        } else {

            uint32_t restSample = AUDIO_BUF_SAMPLE - AudioBufferIndex;

            for (uint32_t i = 0; i < restSample; i++) {
                data[2 * i] = AudioBuffer[AudioBufferIndex + i];
                data[2 * i + 1] = AudioBuffer[AudioBufferIndex + i];
            }

            memset(&data[2 * restSample], 0, size - 2 * restSample);
            isPlayDone = true;
        }

        if (isPlayDone) {
            AudioBufferIndex = 0;
            startPlay = false;
            BLUE_OFF();
        }
    } else {
        memset(data, 0, size);
    }

    return true;    
}

_Noreturn void RTCoreMain(void)
{
    bool gpiostate;

    //while (start == false);

    VectorTableInit();
    CPUFreq_Set(197600000);

    GPIO_ConfigurePinForInput(GPIO_BUTTON_A);
    GPIO_ConfigurePinForInput(GPIO_BUTTON_B);
    GPIO_ConfigurePinForOutput(GPIO_PLAY_R);
    GPIO_ConfigurePinForOutput(GPIO_PLAY_G);
    GPIO_ConfigurePinForOutput(GPIO_PLAY_B);
    GPIO_Write(GPIO_PLAY_R, 1);
    GPIO_Write(GPIO_PLAY_G, 1);
    GPIO_Write(GPIO_PLAY_B, 1);

    I2S *i2sHndl = I2S_Open(MT3620_UNIT_I2S0, 16000000);
    I2S_Input(i2sHndl, I2S_FORMAT_I2S, 1, 16, 16000, recvCallback);
    I2S_Output(i2sHndl, I2S_FORMAT_I2S, 2, 16, 16000, sendCallback);
    CODEC_Init(&codecHandle, &codecConfig);

    for (;;) {
        
        // Press button A to start recording for 5 seconds
        GPIO_Read(GPIO_BUTTON_A, &gpiostate);
        if (!gpiostate) {
            if (!startRecord && !startPlay) {
                startRecord = true;
                RED_ON();
            }
        }

        // Press Button B to replay recorded voice
        GPIO_Read(GPIO_BUTTON_B, &gpiostate);
        if (!gpiostate) {
            if (!startRecord && !startPlay) {
                startPlay = true;
                GREEN_OFF();
                BLUE_ON();
            }
        }
    }
}
