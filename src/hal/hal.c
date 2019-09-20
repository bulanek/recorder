#include "recorder_inc.h"
#include "_com/hal_com.h"
#include "trace_com.h"

bool hal_init(void)
{
    bool retVal = true;
    do
    {
        //retVal = HAL_RTC_INIT();
        //if (retVal == false)
        //{
        //    TRACE_00(TRACE_ERROR, "Failed to send data to nv task");
        //    break;
        //}

        SystemCoreClockUpdate();
        retVal = SysTick_Config(SystemCoreClock / 1000U) == 0 ? true : false;
        NVIC_SetPriorityGrouping(0);

        HAL_PDM_PCM_MODE_INIT();
        HAL_INIT_CONFIG_UART();
    } while (0);
    return retVal;
}

HalMode hal_get_mode(void)
{
    HalMode mode = HAL_MODE_RECORD;
    if (GPIOA->IDR & GPIO_IDR_ID0)
    {
        mode = HAL_MODE_PDM_PCM;
    }
    return mode;
}

