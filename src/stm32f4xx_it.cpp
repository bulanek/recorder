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
    if ((DMA1->LISR & DMA_LISR_TCIF3) != 0)
    {
        DMA1->LIFCR = DMA_LIFCR_CTCIF3;
    }
    if ((DMA1->LISR & DMA_LISR_TEIF3) != 0 || DMA1->LISR & DMA_LISR_DMEIF3)
    {
        if ((DMA1->LISR & DMA_LISR_TEIF3) != 0)
        {
            DMA1->LIFCR = DMA_LIFCR_CTEIF3;
        }
        if ((DMA1->LISR & DMA_LISR_DMEIF3) != 0)
        {
            DMA1->LIFCR = DMA_LIFCR_CDMEIF3;
        }
        if ((DMA1->LISR & DMA_LISR_FEIF3) != 0)
        {
            DMA1->LIFCR = DMA_LIFCR_CFEIF3;
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
    taskQueuePdmPcm._sizePdmDataWord = i2s_get_buffer_size_word();
    taskQueuePdmPcm._opcode = PDM_PCM_GET_PCM_DATA;
    tskma_send_to_pdm_pcm_irt(&taskQueuePdmPcm);
}

extern "C" void USART2_IRQHandler(void)
{
    TaskHandle_t uartTaskHandle = tskma_get_uart_task_handle();
    BaseType_t xYieldRequired = xTaskResumeFromISR(uartTaskHandle);
    // We should switch context so the ISR returns to a different task.
    // NOTE:  How this is done depends on the port you are using.  Check
    // the documentation and examples for your port.
    portYIELD_FROM_ISR(xYieldRequired);
}

