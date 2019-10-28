/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @author  Ac6
  * @version V1.0
  * @date    02-Feb-2015
  * @brief   Default Interrupt Service Routines.
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#ifdef USE_RTOS_SYSTICK
#include <cmsis_os.h>
#endif

#include "recorder_inc.h"
#include "trace_com.h"
#include "trace_out.hpp"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            	  	    Processor Exceptions Handlers                         */
/******************************************************************************/

#include "recorder_inc.h"

#include "tskma_tasks.h"
#include "i2s_com.h"

/// I2S interrupt
extern "C" void SPI2_IRQHandler(void)
{
    TaskQueueI2S queue;
    queue._data = (uint16_t)SPI2->DR;
    static int counter = 0;
    if (counter++ % 10000 == 0)
    {
        tskma_send_to_i2s_irt(&queue);
    }
}

extern "C" void EXTI0_IRQHandler(void)
{
    NVIC_ClearPendingIRQ(EXTI0_IRQn);
    EXTI->PR = EXTI_PR_PR0;
    bool retVal;
    TaskQueueNV taskQueue;
    taskQueue._opcode = NV_OPCODE_STOP_RESET;
    if ((retVal = tskma_send_to_nv_irt(&taskQueue)) != true)
    {
        return;
    }
}

extern "C" void DMA1_Stream3_IRQHandler(void)
{
    static int counter = 0;
    static int counterTE = 0;
    static int counterDME = 0;
    static int counterFE = 0;
    static volatile DMA_TypeDef* dma1 = DMA1;
    static volatile DMA_Stream_TypeDef* pStreamInt = DMA1_Stream3;
    //NVIC_ClearPendingIRQ(DMA1_Stream3_IRQn);
    if ((DMA1->LISR & DMA_LISR_TCIF3) != 0)
    {
        DMA1->LIFCR = DMA_LIFCR_CTCIF3;
        ++counter;
    }
    if ((DMA1->LISR & DMA_LISR_TEIF3) != 0 || DMA1->LISR & DMA_LISR_DMEIF3)
    {
        if ((DMA1->LISR & DMA_LISR_TEIF3) != 0)
        {
            DMA1->LIFCR = DMA_LIFCR_CTEIF3;
            ++counterTE;
        }
        if ((DMA1->LISR & DMA_LISR_DMEIF3) != 0)
        {
            DMA1->LIFCR = DMA_LIFCR_CDMEIF3;
            ++counterDME;
        }
        if ((DMA1->LISR & DMA_LISR_FEIF3) != 0)
        {
            DMA1->LIFCR = DMA_LIFCR_CFEIF3;
            ++counterFE;
        }
        return;
    }
    uint16_t* pData;
    if (DMA1_Stream3->CR & DMA_SxCR_CT)
    {
        pData = reinterpret_cast<uint16_t*>(DMA1_Stream3->M1AR);
    }
    else
    {
        pData = reinterpret_cast<uint16_t*>(DMA1_Stream3->M0AR);
    }

    TaskQueuePDMPCM taskQueuePdmPcm;
    taskQueuePdmPcm._pdmDataPointer = pData;
    taskQueuePdmPcm._sizePdmDataWord = i2s_get_buffer_size_word() ;
    taskQueuePdmPcm._opcode = PDM_PCM_GET_PCM_DATA;
    tskma_send_to_pdm_pcm_irt(&taskQueuePdmPcm);
}

