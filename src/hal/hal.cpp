#include "recorder_cfg.h"
#include "_com/hal_com.h"
#include "trace_com.h"
#include "trace_out.hpp"

bool hal_init(void)
{
    bool retVal = true;
    do
    {
        //retVal = HAL_RTC_INIT();
        //if (retVal == false)
        //{
        //    TRACE_00(TRACE_LEVEL_ERROR, "Failed to send data to nv task");
        //    break;
        //}

        //SystemCoreClockUpdate();
        retVal = TRACE_INIT();
        if (retVal == false)
        {
            break;
        }
        retVal = SysTick_Config(SystemCoreClock / 1000U) == 0 ? true : false;
        if (retVal == false)
        {
            //TRACE_00(TRACE_LEVEL_ERROR, "Systick config failed");
            break;
        }
        NVIC_SetPriorityGrouping(0);
        HAL_PDM_PCM_MODE_INIT();
        HAL_INIT_CONFIG_UART();
    } while (0);
    return retVal;
}

HalMode hal_get_mode(void)
{
    HalMode mode;
    if (GPIOA->IDR & GPIO_IDR_ID0)
    {
        mode = HAL_MODE_PDM_PCM;
    }
    else
    {
        mode = HAL_MODE_RECORD;
    }
    return mode;
}

