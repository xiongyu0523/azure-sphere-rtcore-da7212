/*
 * Copyright  2019 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "lib/I2CMaster.h"
#include "fsl_codec_i2c.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

#define MAX_XFER_SIZE   64  
static uint8_t s_xferBuffer[MAX_XFER_SIZE];

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * brief Codec i2c bus initilization.
 *
 * param handle i2c master handle.
 * param i2CInstance instance number of the i2c bus, such as 0 is corresponding to I2C0.
 * param i2cBaudrate i2c baudrate.
 * param i2cSourceClockHz i2c source clock frequency.
 * return kStatus_HAL_I2cSuccess is success, else initial failed.
 */
status_t CODEC_I2C_Init(void *handle, uint32_t i2cInstance, uint32_t i2cBaudrate, uint32_t i2cSourceClockHz)
{
    (void)i2cSourceClockHz;

    I2CMaster *i2cHndl = I2CMaster_Open(i2cInstance);
    I2CMaster_SetBusSpeed(i2cHndl, i2cBaudrate);

    memcpy(handle, &i2cHndl, CODEC_I2C_MASTER_HANDLER_SIZE);
    
    return kStatus_Success;
}

/*!
 * brief Codec i2c de-initilization.
 *
 * param handle i2c master handle.
 * return kStatus_HAL_I2cSuccess is success, else deinitial failed.
 */
status_t CODEC_I2C_Deinit(void *handle)
{
    I2CMaster_Close(*(uint32_t*)handle);
    return kStatus_Success;
}

/*!
 * brief codec i2c send function.
 *
 * param handle i2c master handle.
 * param deviceAddress codec device address.
 * param subAddress register address.
 * param subaddressSize register address width.
 * param txBuff tx buffer pointer.
 * param txBuffSize tx buffer size.
 * return kStatus_HAL_I2cSuccess is success, else send failed.
 */
status_t CODEC_I2C_Send(void *handle,
                        uint8_t deviceAddress,
                        uint32_t subAddress,
                        uint8_t subaddressSize,
                        uint8_t *txBuff,
                        uint8_t txBuffSize)
{
    memcpy(&s_xferBuffer[0], &subAddress, subaddressSize);
    memcpy(&s_xferBuffer[subaddressSize], txBuff, txBuffSize);

    return (I2CMaster_WriteSync(*(uint32_t*)handle, deviceAddress, s_xferBuffer, subaddressSize+ txBuffSize) == ERROR_NONE) ? kStatus_Success : kStatus_Fail;
}

/*!
 * brief codec i2c receive function.
 *
 * param handle i2c master handle.
 * param deviceAddress codec device address.
 * param subAddress register address.
 * param subaddressSize register address width.
 * param rxBuff rx buffer pointer.
 * param rxBuffSize rx buffer size.
 * return kStatus_HAL_I2cSuccess is success, else receive failed.
 */
status_t CODEC_I2C_Receive(void *handle,
                           uint8_t deviceAddress,
                           uint32_t subAddress,
                           uint8_t subaddressSize,
                           uint8_t *rxBuff,
                           uint8_t rxBuffSize)
{
    return (I2CMaster_WriteThenReadSync(*(uint32_t*)handle, deviceAddress, &subAddress, subaddressSize, rxBuff, rxBuffSize) == ERROR_NONE) ? kStatus_Success : kStatus_Fail;
}
