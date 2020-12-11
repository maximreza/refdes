/* ****************************************************************************
 * Copyright (C) 2019 Maxim Integrated Products, Inc., All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Maxim Integrated
 * Products, Inc. shall not be used except as stated in the Maxim Integrated
 * Products, Inc. Branding Policy.
 *
 * The mere transfer of this software does not imply any licenses
 * of trade secrets, proprietary technology, copyrights, patents,
 * trademarks, maskwork rights, or any other form of intellectual
 * property whatsoever. Maxim Integrated Products, Inc. retains all
 * ownership rights.
 *
 *************************************************************************** */

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include "mxc_device.h"
#include "mxc_assert.h"
#include "mxc_lock.h"
#include "mxc_sys.h"
#include "mxc_delay.h"
#include "spi_reva.h"
#include "dma.h"

/* **** Functions **** */
int MXC_SPI_Init (mxc_spi_regs_t* spi, int masterMode, int quadModeUsed, int numSlaves,
                  unsigned ssPolarity, unsigned int hz, sys_map_t map)
{
    int spi_num;
    
    spi_num = MXC_SPI_GET_IDX (spi);
    MXC_ASSERT (spi_num >= 0);
    
    if (numSlaves > MXC_SPI_SS_INSTANCES) {
        return E_BAD_PARAM;
    }
    
    // Check if frequency is too high
    if (hz > PeripheralClock) {
        return E_BAD_PARAM;
    }
    
    // Configure GPIO for spi
    if (spi == MXC_SPI0) {
        MXC_GCR->rst1 |= MXC_F_GCR_RST1_SPI3;
        while (MXC_GCR->rst1 & MXC_F_GCR_RST1_SPI3);
        MXC_GCR->pclk_dis1 &= ~(MXC_F_GCR_PCLK_DIS1_SPI3);
        if(map==MAP_A){
        	MXC_GPIO_Config (&gpio_cfg_spi0a);
        }
        else if(map==MAP_B){
        	MXC_GPIO_Config (&gpio_cfg_spi0b);
        }
    }
    else if (spi == MXC_SPI1) {
        MXC_GCR->rst0 |= MXC_F_GCR_RST0_SPI1;
        while (MXC_GCR->rst0 & MXC_F_GCR_RST0_SPI1);
        MXC_GCR->pclk_dis0 &= ~ (MXC_F_GCR_PCLK_DIS0_SPI0);
        MXC_GPIO_Config (&gpio_cfg_spi1);
    }
    else if (spi == MXC_SPI2) {
        MXC_GCR->rst0 |= MXC_F_GCR_RST0_SPI2;
        while (MXC_GCR->rst0 & MXC_F_GCR_RST0_SPI2);
        MXC_GCR->pclk_dis0 &= ~ (MXC_F_GCR_PCLK_DIS0_SPI1);
        MXC_GPIO_Config (&gpio_cfg_spi2);
    }
    else {
        return E_NO_DEVICE;
    }
    
    return MXC_SPI_RevA_Init ((mxc_spi_reva_regs_t*) spi, masterMode, quadModeUsed, numSlaves, ssPolarity, hz);
}

int MXC_SPI_Shutdown (mxc_spi_regs_t* spi)
{
    int spi_num;
    spi_num = MXC_SPI_GET_IDX (spi);
    MXC_ASSERT (spi_num >= 0);
    
    MXC_SPI_RevA_Shutdown ((mxc_spi_reva_regs_t*)spi);
    
    if (spi == MXC_SPI0) {
        MXC_GCR->pclk_dis0 |= (MXC_F_GCR_PCLK_DIS1_SPI3);
    }
    else if (spi == MXC_SPI1) {
        MXC_GCR->pclk_dis0 |= (MXC_F_GCR_PCLK_DIS0_SPI0);
    }
    else if (spi == MXC_SPI2) {
        MXC_GCR->pclk_dis0 |= (MXC_F_GCR_PCLK_DIS0_SPI1);
    }
    else {
        return E_INVALID;
    }

    return E_NO_ERROR;
}

int MXC_SPI_ReadyForSleep (mxc_spi_regs_t* spi)
{
    return MXC_SPI_RevA_ReadyForSleep ((mxc_spi_reva_regs_t*) spi);
}

int MXC_SPI_SetFrequency (mxc_spi_regs_t* spi, unsigned int hz)
{
    return MXC_SPI_RevA_SetFrequency ((mxc_spi_reva_regs_t*) spi, hz);
}

unsigned int MXC_SPI_GetFrequency (mxc_spi_regs_t* spi)
{
    return MXC_SPI_RevA_GetFrequency ((mxc_spi_reva_regs_t*) spi);
}

int MXC_SPI_SetDataSize (mxc_spi_regs_t* spi, int dataSize)
{
    return MXC_SPI_RevA_SetDataSize ((mxc_spi_reva_regs_t*) spi, dataSize);
}

int MXC_SPI_GetDataSize (mxc_spi_regs_t* spi)
{
    return MXC_SPI_RevA_GetDataSize ((mxc_spi_reva_regs_t*) spi);
}

int MXC_SPI_SetSlave (mxc_spi_regs_t* spi, int ssIdx)
{
    return MXC_SPI_RevA_SetSlave ((mxc_spi_reva_regs_t*) spi, ssIdx);
}

int MXC_SPI_GetSlave (mxc_spi_regs_t* spi)
{
    return MXC_SPI_RevA_GetSlave ((mxc_spi_reva_regs_t*) spi);
}

int MXC_SPI_SetWidth (mxc_spi_regs_t* spi, mxc_spi_width_t spiWidth)
{
    return MXC_SPI_RevA_SetWidth ((mxc_spi_reva_regs_t*) spi, spiWidth);
}

mxc_spi_width_t MXC_SPI_GetWidth (mxc_spi_regs_t* spi)
{
    return (mxc_spi_width_t) MXC_SPI_RevA_GetWidth ((mxc_spi_reva_regs_t*) spi);
}

int MXC_SPI_StartTransmission (mxc_spi_regs_t* spi)
{
    return MXC_SPI_RevA_StartTransmission ((mxc_spi_reva_regs_t*) spi);
}

int MXC_SPI_GetActive (mxc_spi_regs_t* spi)
{
    return MXC_SPI_RevA_GetActive ((mxc_spi_reva_regs_t*) spi);
}

int MXC_SPI_AbortTransmission (mxc_spi_regs_t* spi)
{
    return MXC_SPI_RevA_AbortTransmission ((mxc_spi_reva_regs_t*) spi);
}

unsigned int MXC_SPI_ReadRXFIFO (mxc_spi_regs_t* spi, unsigned char* bytes,
                                 unsigned int len)
{
    return MXC_SPI_RevA_ReadRXFIFO ((mxc_spi_reva_regs_t*) spi, bytes, len);
}

unsigned int MXC_SPI_GetRXFIFOAvailable (mxc_spi_regs_t* spi)
{
    return MXC_SPI_RevA_GetRXFIFOAvailable ((mxc_spi_reva_regs_t*) spi);
}

unsigned int MXC_SPI_WriteTXFIFO (mxc_spi_regs_t* spi, unsigned char* bytes,
                                  unsigned int len)
{
    return MXC_SPI_RevA_WriteTXFIFO ((mxc_spi_reva_regs_t*) spi, bytes, len);
}

unsigned int MXC_SPI_GetTXFIFOAvailable (mxc_spi_regs_t* spi)
{
    return MXC_SPI_RevA_GetTXFIFOAvailable ((mxc_spi_reva_regs_t*) spi);
}

void MXC_SPI_ClearRXFIFO (mxc_spi_regs_t* spi)
{
    MXC_SPI_RevA_ClearRXFIFO ((mxc_spi_reva_regs_t*) spi);
}

void MXC_SPI_ClearTXFIFO (mxc_spi_regs_t* spi)
{
    MXC_SPI_RevA_ClearTXFIFO ((mxc_spi_reva_regs_t*) spi);
}

int MXC_SPI_SetRXThreshold (mxc_spi_regs_t* spi, unsigned int numBytes)
{
    return MXC_SPI_RevA_SetRXThreshold ((mxc_spi_reva_regs_t*) spi, numBytes);
}

unsigned int MXC_SPI_GetRXThreshold (mxc_spi_regs_t* spi)
{
    return MXC_SPI_RevA_GetRXThreshold ((mxc_spi_reva_regs_t*) spi);
}

int MXC_SPI_SetTXThreshold (mxc_spi_regs_t* spi, unsigned int numBytes)
{
    return MXC_SPI_RevA_SetTXThreshold ((mxc_spi_reva_regs_t*) spi, numBytes);
}

unsigned int MXC_SPI_GetTXThreshold (mxc_spi_regs_t* spi)
{
    return MXC_SPI_RevA_GetTXThreshold ((mxc_spi_reva_regs_t*) spi);
}

unsigned int MXC_SPI_GetFlags (mxc_spi_regs_t* spi)
{
    return MXC_SPI_RevA_GetFlags ((mxc_spi_reva_regs_t*) spi);
}

void MXC_SPI_ClearFlags (mxc_spi_regs_t* spi)
{
    MXC_SPI_RevA_ClearFlags ((mxc_spi_reva_regs_t*) spi);
}

void MXC_SPI_EnableInt (mxc_spi_regs_t* spi, unsigned int mask)
{
    MXC_SPI_RevA_EnableInt ((mxc_spi_reva_regs_t*) spi, mask);
}

void MXC_SPI_DisableInt (mxc_spi_regs_t* spi, unsigned int mask)
{
    MXC_SPI_RevA_DisableInt ((mxc_spi_reva_regs_t*) spi, mask);
}

int MXC_SPI_MasterTransaction (mxc_spi_req_t* req)
{
    return MXC_SPI_RevA_MasterTransaction ((mxc_spi_reva_req_t*) req);
}

int MXC_SPI_MasterTransactionAsync (mxc_spi_req_t* req)
{
    return MXC_SPI_RevA_MasterTransactionAsync ((mxc_spi_reva_req_t*) req);
}

int MXC_SPI_MasterTransactionDMA (mxc_spi_req_t* req)
{
    int reqselTx = -1;
    int reqselRx = -1;

    int spi_num;

    spi_num = MXC_SPI_GET_IDX (req->spi);
    MXC_ASSERT (spi_num >= 0);
    
    if (req->txData != NULL) {
        switch (spi_num) {
        case 0:
            reqselTx = MXC_DMA_REQUEST_SPI0TX;
            break;

        case 1:
            reqselTx = MXC_DMA_REQUEST_SPI1TX;
            break;

        case 2:
            reqselTx = MXC_DMA_REQUEST_SPI2TX;
            break;

        default:
            return E_BAD_PARAM;            
        }
    }

    if (req->rxData != NULL) {
        switch (spi_num) {
        case 0:
            reqselRx = MXC_DMA_REQUEST_SPI0RX;
            break;

        case 1:
            reqselRx = MXC_DMA_REQUEST_SPI1RX;
            break;

        case 2:
            reqselRx = MXC_DMA_REQUEST_SPI2RX;
            break;

        default:
            return E_BAD_PARAM;            
        }
    }

    return MXC_SPI_RevA_MasterTransactionDMA ((mxc_spi_reva_req_t*) req, reqselTx, reqselRx);
}

int MXC_SPI_SlaveTransaction (mxc_spi_req_t* req)
{
    return MXC_SPI_RevA_SlaveTransaction ((mxc_spi_reva_req_t*) req);
}

int MXC_SPI_SlaveTransactionAsync (mxc_spi_req_t* req)
{
    return MXC_SPI_RevA_SlaveTransactionAsync ((mxc_spi_reva_req_t*) req);
}

int MXC_SPI_SlaveTransactionDMA (mxc_spi_req_t* req)
{
    int reqselTx = -1;
    int reqselRx = -1;

    int spi_num;

    spi_num = MXC_SPI_GET_IDX (req->spi);
    MXC_ASSERT (spi_num >= 0);
    
    if (req->txData != NULL) {
        switch (spi_num) {
        case 0:
            reqselTx = MXC_DMA_REQUEST_SPI0TX;
            break;

        case 1:
            reqselTx = MXC_DMA_REQUEST_SPI1TX;
            break;

        case 2:
            reqselTx = MXC_DMA_REQUEST_SPI2TX;
            break;

        default:
            return E_BAD_PARAM;   
        }
    }

    if (req->rxData != NULL) {
        switch (spi_num) {
        case 0:
            reqselRx = MXC_DMA_REQUEST_SPI0RX;
            break;

        case 1:
            reqselRx = MXC_DMA_REQUEST_SPI1RX;
            break;

        case 2:
            reqselRx = MXC_DMA_REQUEST_SPI2RX;
            break;

        default:
            return E_BAD_PARAM;   
        }
    }

    return MXC_SPI_RevA_SlaveTransactionDMA ((mxc_spi_reva_req_t*) req, reqselTx, reqselRx);
}

int MXC_SPI_SetDefaultTXData (mxc_spi_regs_t* spi, unsigned int defaultTXData)
{
    return MXC_SPI_RevA_SetDefaultTXData ((mxc_spi_reva_regs_t*) spi, defaultTXData);
}

void MXC_SPI_AbortAsync (mxc_spi_regs_t* spi)
{
    MXC_SPI_RevA_AbortAsync ((mxc_spi_reva_regs_t*) spi);
}

void MXC_SPI_AsyncHandler (mxc_spi_regs_t* spi)
{
    MXC_SPI_RevA_AsyncHandler ((mxc_spi_reva_regs_t*) spi);
}
