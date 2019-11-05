#include "recorder_inc.h"
#include "_com/tskma_tasks.h"
#include "tskma_service.hpp"
#include "trace_com.h"
#include "trace_out.hpp"

TSKMA::Service f_taskService;

extern "C" bool tskma_initialize(void)
{
    bool retVal = true;
    retVal = f_taskService.Init();
    return retVal;
}

extern "C" void tskma_send_to_uart_irt(const TaskQueueUART* const pQueueData)
{
     BaseType_t xHigherPriorityTaskWoken;
     BaseType_t xYieldNeeded = pdTRUE;

     if (f_taskService.GetTaskInfo()[TSKMA::TASK_UART]._queueHandle == 0 || f_taskService.GetTaskInfo()[TSKMA::TASK_UART]._taskHandle == 0)
     {
         return;
     }

     xYieldNeeded = xTaskResumeFromISR(f_taskService.GetTaskInfo()[TSKMA::TASK_UART]._taskHandle);
     if (xYieldNeeded == pdTRUE)
     {
         portYIELD();
     }
}

extern "C" bool tskma_send_to_uart(const TaskQueueUART* const pQueueData)
{
    BaseType_t err = xQueueSend(f_taskService.GetTaskInfo()[TSKMA::TASK_UART]._queueHandle, pQueueData, (TickType_t)0);
    if (err != pdTRUE)
    {
        TRACE_01(TRACE_LEVEL_ERROR, "Send message failure: %i", err);
        return false;
    }
    return true;
}

extern "C" bool tskma_send_to_nv(const TaskQueueNV* const pQueueData)
{
    BaseType_t err = xQueueSend(f_taskService.GetTaskInfo()[TSKMA::TASK_NV]._queueHandle, pQueueData, (TickType_t)0);
    if (err != pdTRUE)
    {
        TRACE_02(TRACE_LEVEL_ERROR, "Send message failure: %i, opcode: %i", err, pQueueData->_opcode);
        return false;
    }
    return true;
}

extern "C" bool tskma_send_to_nv_irt(const TaskQueueNV* const pQueueData)
{
     BaseType_t xHigherPriorityTaskWoken;
     BaseType_t xYieldNeeded = pdTRUE;

     if (f_taskService.GetTaskInfo()[TSKMA::TASK_NV]._queueHandle == 0 || f_taskService.GetTaskInfo()[TSKMA::TASK_NV]._taskHandle == 0)
     {
         return false;
     }

     xYieldNeeded = xQueueSendFromISR(f_taskService.GetTaskInfo()[TSKMA::TASK_NV]._queueHandle, pQueueData, &xHigherPriorityTaskWoken);
     if (xYieldNeeded == pdTRUE)
     {
         portYIELD();
     }
     return true;
}


extern "C" bool tskma_send_to_pdm_pcm(const TaskQueuePDMPCM* const pQueueData)
{
    BaseType_t err = xQueueSend(f_taskService.GetTaskInfo()[TSKMA::TASK_PDM_PCM]._queueHandle, pQueueData, (TickType_t)0);
    if (err != pdTRUE)
    {
        TRACE_01(TRACE_LEVEL_ERROR, "Send message failure: %i", err);
        return false;
    }
    return true;
}

extern "C" bool tskma_send_to_pdm_pcm_irt(const TaskQueuePDMPCM* const pQueueData)
{
     BaseType_t xHigherPriorityTaskWoken;
     BaseType_t xYieldNeeded = pdTRUE;

     if (f_taskService.GetTaskInfo()[TSKMA::TASK_PDM_PCM]._queueHandle == 0 || f_taskService.GetTaskInfo()[TSKMA::TASK_PDM_PCM]._taskHandle == 0)
     {
         return false;
     }

     xYieldNeeded = xQueueSendFromISR(f_taskService.GetTaskInfo()[TSKMA::TASK_PDM_PCM]._queueHandle, pQueueData, &xHigherPriorityTaskWoken);
     if (xYieldNeeded == pdTRUE)
     {
         portYIELD();
     }
     return true;
}

extern "C" TaskHandle_t tskma_get_uart_task_handle(void)
{
    return f_taskService.GetTaskInfo()[TSKMA::TASK_UART]._taskHandle;
}
