#include "_com/tskma_tasks.h"
#include "nv_com.h"
#include "trace_com.h"
#include "trace_out.hpp"
#include "recorder_inc.h"
#include "pdmpcm_com.h"
#include "tskma_service.hpp"


static void task_nv_common(void* pParameters)
{
    TaskQueueNV queue;
    QueueHandle_t xQueue = *(QueueHandle_t*)pParameters;
    TickType_t xTicksToWait = portMAX_DELAY;
    TRACE_00(TRACE_LEVEL_LOG, "Start nv common");

    bool retVal;
    if ((retVal = nv_init()) == false)
    {
        TRACE_00(TRACE_LEVEL_ERROR, "nv init failed");
    }

    NV::ServiceIf* pService = GetNVService();

    for (;; )
    {
        if (xQueueReceive(xQueue, &queue, xTicksToWait) == pdPASS)
        {
            switch (queue._opcode)
            {
            case NV_PDM_START:
                TRACE_00(TRACE_LEVEL_LOG, "NV_PDM_START request");
                if (pService->StartPDM() != true)
                {
                    TRACE_00(TRACE_LEVEL_ERROR, "Start nv pdm failed");
                }
                break;
            case NV_PDM_STOP:
                TRACE_00(TRACE_LEVEL_LOG, "NV_PDM_STOP request");
                if (nv_stop_pdm() != true)
                {
                    TRACE_00(TRACE_LEVEL_ERROR, "Stop nv pdm stopped");
                }
                break;
            case NV_PCM_START:
                TRACE_00(TRACE_LEVEL_LOG, "NV_PCM_START request");
                if (nv_start_pcm() != true)
                {
                    TRACE_00(TRACE_LEVEL_ERROR, "Start nv pcm failed");
                }
                break;
            case NV_PCM_STOP:
                TRACE_00(TRACE_LEVEL_LOG, "NV_PCM_STOP request");
                if (nv_stop_pcm() != true)
                {
                    TRACE_00(TRACE_LEVEL_ERROR, "Stop nv pcm failed");
                }
                break;
            case NV_OPCODE_WRITE_PCM_DATA:
            {
                if (nv_write_pcm_record_data(queue._pData, queue._dataLength) == false)
                {
                    TRACE_00(TRACE_LEVEL_WARN, "Write record pcm data failed");
                }
                //delete queue._pData;
                break;
            }
            case NV_OPCODE_WRITE_TRACE_DATA:
            {
                if (nv_write_trace_data(queue._pDataTrace, queue._dataLength) == false)
                {
                    //TRACE_00(TRACE_LEVEL_WARN, "Write record pcm data failed");
                }
                delete queue._pDataTrace;
                break;
            }
            case NV_OPCODE_STOP_RESET:
                TRACE_00(TRACE_LEVEL_LOG, "NV_STOP_RESET request");
                (void)nv_stop_pdm();
                (void)nv_stop_pcm();
                NVIC_SystemReset();
                break;
            case NV_STOP:
                GetNVService()->Deinit();
                break;
            default:
                TRACE_01(TRACE_LEVEL_ERROR, "wrong opcode: %i",queue._opcode);
                break;/* TODO error*/
            }
        }
    }
}

extern "C" void task_nv(void* pParameters)
{

    //TRACE_00(TRACE_LEVEL_LOG, "Start of nv task");
    task_nv_common(pParameters);
}


