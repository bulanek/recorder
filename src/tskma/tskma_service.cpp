#include <stdlib.h>
#include "recorder_inc.h"
#include "tskma_service.hpp"
#include "_com/tskma_tasks.h"
#include "hal_com.h"
#include "trace_com.h"

static_assert(STRLEN_STATIC(I2S_TASK_NAME) + 1 < configMAX_TASK_NAME_LEN, "Wrong size of task name size, I2S");
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
    TaskNVFunc funcNV = task_nv;

    do
    {
        switch (mode)
        {
        case HAL_MODE_RECORD:
        {
            retVal = (_tasks[TASK_I2S]._queueHandle = xQueueCreate(I2S_TASK_QUEUE_NUM_ITEMS, sizeof(TaskQueueI2S))) == 0 ? false : true;
            if (retVal == false) break;
            retVal = xTaskCreate(task_i2s, I2S_TASK_NAME, 1000, &_tasks[TASK_I2S]._queueHandle, I2S_TASK_PRIORITY, &_tasks[TASK_I2S]._taskHandle) == pdPASS ? true : false;
            if (retVal == false) break;

        }
        break;
        case HAL_MODE_PDM_PCM:
            funcNV = task_nv_pcm;
            retVal = (_tasks[TASK_PDM_PCM]._queueHandle = xQueueCreate(PDM_PCM_TASK_QUEUE_NUM_ITEMS, sizeof(TaskQueuePDMPCM))) == 0 ? false : true;
            if (retVal == false) break;
            retVal = xTaskCreate(task_pdm_pcm, PDM_PCM_TASK_NAME, 1000, &_tasks[TASK_PDM_PCM]._queueHandle, PDM_PCM_TASK_PRIORITY, &_tasks[TASK_PDM_PCM]._taskHandle) == pdPASS ? true : false;
            if (retVal == false) break;
            break;
        default:
            TRACE_00(TRACE_ERROR, "Unexpected mode");
            retVal = false;
        }

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
        retVal = (_tasks[TASK_NV]._queueHandle = xQueueCreate(NV_TASK_QUEUE_NUM_ITEMS, sizeof(TaskQueueNV))) == 0 ? false : true;
        if (retVal == false) break;
        retVal = xTaskCreate(funcNV, NV_TASK_NAME, 1000, &_tasks[TASK_NV]._queueHandle, FLASH_TASK_PRIORITY, &_tasks[TASK_NV]._taskHandle) == pdPASS ? true : false;
        if (retVal == false) break;

    } while (0);
    return retVal;
}
} /* namespace TSKMA */
