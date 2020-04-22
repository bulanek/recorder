#include <stdlib.h>
#include "recorder_inc.h"
#include "tskma_service.hpp"
#include "_com/tskma_tasks.h"
#include "hal_com.h"
#include "trace_com.h"
#include "trace_out.hpp"
#include "nv_com.h"
#include "pdmpcm_com.h"
#include "i2s_com.h"

static_assert(STRLEN_STATIC(UART_TASK_NAME) + 1 < configMAX_TASK_NAME_LEN, "Wrong size of task name size, I2S");
static_assert(STRLEN_STATIC(NV_TASK_NAME) + 1 < configMAX_TASK_NAME_LEN, "Wrong size of task name size, NV");

void timerf13Calback(TimerHandle_t xTimer)
{
    disk_timerproc();
}

namespace TSKMA
{
bool Service::Init(void)
{
    bool retVal = true;
    HalMode mode = hal_get_mode();
    typedef void(*TaskNVFunc)(void*);
    TaskNVFunc funcNV = task_nv;/* task_nv;*/
    do
    {
        TimerHandle_t timer = xTimerCreate("Timer_ff13", pdMS_TO_TICKS(1U), pdTRUE, (void*)0, timerf13Calback);
        if (timer == NULL)
        {
            retVal = false;
            break;
        }
        if (xTimerStart(timer, 0) != pdPASS)
        {
            retVal = false;
            break;
        }


        if ((retVal = pdmpcm_init()) != true)
        {
            TRACE_00(TRACE_LEVEL_ERROR, "pdmpcm init failed");
            break;
        }

        if (mode == HAL_MODE_RECORD)
        {
            if ((retVal = i2s_init()) == false)
            {
                TRACE_00(TRACE_LEVEL_ERROR, "i2s init failed");
                break;
            }
        }

        retVal = (_tasks[TASK_UART]._queueHandle = xQueueCreate(UART_TASK_QUEUE_NUM_ITEMS, sizeof(TaskQueueUART))) == 0 ? false : true;
        if (retVal == false) break;
        retVal = xTaskCreate(task_uart, UART_TASK_NAME, 1000, &_tasks[TASK_UART]._queueHandle, UART_TASK_PRIORITY, &_tasks[TASK_UART]._taskHandle) == pdPASS ? true : false;
        if (retVal == false) break;


        retVal = (_tasks[TASK_NV]._queueHandle = xQueueCreate(NV_TASK_QUEUE_NUM_ITEMS, sizeof(TaskQueueNV))) == 0 ? false : true;
        if (retVal == false) break;
        retVal = xTaskCreate(funcNV, NV_TASK_NAME, 1000, &_tasks[TASK_NV]._queueHandle, FLASH_TASK_PRIORITY, &_tasks[TASK_NV]._taskHandle) == pdPASS ? true : false;
        if (retVal == false) break;

        switch (mode)
        {
        case HAL_MODE_RECORD:
        {
            TRACE_00(TRACE_LEVEL_LOG, "Start in PDM mode");
        }
        break;
        case HAL_MODE_PDM_PCM:
            TRACE_00(TRACE_LEVEL_LOG, "Start in PCM mode");
            break;
        default:
            TRACE_00(TRACE_LEVEL_ERROR, "Unexpected mode");
            retVal = false;
        }
    } while (0);
    return retVal;
}
} /* namespace TSKMA */

