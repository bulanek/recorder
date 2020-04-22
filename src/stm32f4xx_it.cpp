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
#include "pdmpcm_com.h"

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
        while (1) {};
    }
    if ((DMA1->LISR & DMA_LISR_TCIF3) == 0)
    {
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

    volatile uint16_t ndtr = DMA1_Stream3->NDTR;
    static uint16_t pcmSize = pdmpcm_get_pcm_size_samples(i2s_get_buffer_size_word());

    static const int numBuffers = 4;
    static int16_t* ppPcmBuffers[numBuffers];
    static int counter = 10;
    if (counter == 10)
    {
        counter = 0;
        for (size_t i = 0; i < numBuffers; i++)
        {
            ppPcmBuffers[i] = new int16_t[pcmSize];
            ASSERT(ppPcmBuffers[i] != nullptr, " pPcmBuffer = nullptr");
        }
    }

    static uint16_t size = pcmSize;
    pdmpcm_process((uint8_t*)pData, ppPcmBuffers[counter], size);// numMsPDMSamples* pcmSizeWord);
    TaskQueueNV nvQueue;

    nvQueue._opcode = NV_OPCODE_WRITE_PCM_DATA;
    nvQueue._pData = reinterpret_cast<uint16_t*>(ppPcmBuffers[counter]);
    nvQueue._dataLength = size;
    ++counter;
    counter %= numBuffers;

    tskma_send_to_nv_irt(&nvQueue);

    if ((DMA1->LISR & DMA_LISR_TCIF3) != 0)
    {
        DMA1->LIFCR = DMA_LIFCR_CTCIF3;
    }
}

#ifdef FF13_DMA
extern "C" void DMA1_Stream5_IRQHandler(void)
{
    if ((DMA1->HISR & DMA_HISR_TCIF5) != 0)
    {
        DMA1->HIFCR = DMA_HIFCR_CTCIF5;
    }
	volatile uint32_t num = DMA1_Stream5->NDTR;
    if ((DMA1->HISR & DMA_HISR_TEIF5) != 0 
		|| (DMA1->HISR & DMA_HISR_DMEIF5) != 0 
		|| (DMA1->HISR & DMA_HISR_FEIF5) != 0)
    {
        if ((DMA1->HISR & DMA_HISR_TEIF5) != 0)
        {
            DMA1->HIFCR = DMA_HIFCR_CTEIF5;
        }
        if ((DMA1->HISR & DMA_HISR_DMEIF5) != 0)
        {
            DMA1->HIFCR = DMA_HIFCR_CDMEIF5;
        }
        if ((DMA1->HISR & DMA_HISR_FEIF5) != 0)
        {
            DMA1->HIFCR = DMA_HIFCR_CFEIF5;
        }
		//while (1);
    }
	BaseType_t resume = xTaskResumeFromISR(tskma_get_nv_task_handle());
    portYIELD_FROM_ISR(resume);
}
#endif /* FF13_DMA */

static char dataChar;
extern "C" void USART2_IRQHandler(void)
{
    TaskHandle_t uartTaskHandle = tskma_get_uart_task_handle();
    TaskQueueUART uartQueue;
    uartQueue._opcode = UART_RCV;
    dataChar = (char)USART2->DR;

    uartQueue._data = reinterpret_cast<uint8_t*>(&dataChar);
    tskma_send_to_uart_irt(&uartQueue);
    //BaseType_t xYieldRequired = xTaskResumeFromISR(uartTaskHandle);
    // We should switch context so the ISR returns to a different task.
    // NOTE:  How this is done depends on the port you are using.  Check
    // the documentation and examples for your port.
    //portYIELD_FROM_ISR(xYieldRequired);
}

