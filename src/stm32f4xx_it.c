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

/// I2S interrupt
void SPI2_IRQHandler(void)
{
    TaskQueueI2S queue;
    queue._data = (uint16_t)SPI2->DR;
    static int counter = 0;
    if (counter++ % 10000 == 0)
    {
        tskma_send_to_i2s_irt(&queue);
    }
}

void EXTI0_IRQHandler(void)
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

