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
    TickType_t xTicksToWait = 0;
    TRACE_00(TRACE_LEVEL_LOG, "Start nv common");

    for (;; )
    {
        if (xQueueReceive(xQueue, &queue, xTicksToWait) == pdPASS)
        {
            switch (queue._opcode)
            {
            case NV_PDM_START:
                TRACE_00(TRACE_LEVEL_LOG, "NV_PDM_START request");
                if (nv_start_pdm() != true)
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
            case NV_OPCODE_WRITE_PDM:
            {
                if (nv_write_pdm_record_data(queue._pData, queue._dataLengthBytes) == false)
                {
                    TRACE_00(TRACE_LEVEL_WARN, "Write record pdm data failed");
                }
                vPortFree(queue._pData);
                break;
            }
            case NV_OPCODE_WRITE_PCM:
            {
                if (nv_write_pcm_record_data(queue._pData, queue._dataLengthBytes) == false)
                {
                    TRACE_00(TRACE_LEVEL_WARN, "Write record pcm data failed");
                }
                vPortFree(queue._pData);
                break;
            }
            case NV_OPCODE_STOP_RESET:
                TRACE_00(TRACE_LEVEL_LOG, "NV_STOP_RESET request");
                (void)nv_stop_pdm();
                (void)nv_stop_pcm();
                NVIC_SystemReset();
                break;

            default:
                ASSERT("wrong opcode");
                break;/* TODO error*/
            }
        }
    }
}

extern "C" void task_nv(void* pParameters)
{

    TRACE_00(TRACE_LEVEL_LOG, "Start of nv task");
    if (nv_start_pdm() == false)
    {
        TRACE_00(TRACE_LEVEL_ERROR, "nv init failed");
        vTaskDelete(NULL);
    	return;
    }

    task_nv_common(pParameters);
}

static void getPDMtoPCMData(void)
{
    bool isNewFile = false;
    do
    {
        isNewFile = false;
        TaskQueuePDMPCM queuePDMPCM;
        queuePDMPCM._opcode = PDM_PCM_GET_PCM_DATA;
        queuePDMPCM._pdmDataPointer = pdmpcm_pop_pdm_buffer();
        queuePDMPCM._sizePdmDataBytes = pdmpcm_get_pdm_size_in_bytes();
        uint16_t numReadBytes;
        if (queuePDMPCM._pdmDataPointer == NULL)
        {
            TRACE_00(TRACE_LEVEL_ERROR, "No pdm data");
            break;
        }
        if (nv_get_pdm_data(queuePDMPCM._sizePdmDataBytes, queuePDMPCM._pdmDataPointer, &numReadBytes) == false)
        {
            TRACE_00(TRACE_LEVEL_ERROR, "send to pdm pcm task failed");
            pdmpcm_push_pdm_buffer(queuePDMPCM._pdmDataPointer);
            break;
        }

        if (numReadBytes == queuePDMPCM._sizePdmDataBytes)
        {
            if (tskma_send_to_pdm_pcm(&queuePDMPCM) == false)
            {
                TRACE_00(TRACE_LEVEL_ERROR, "send to pdm pcm task failed");
                break;
            }
        }
        if (numReadBytes != queuePDMPCM._sizePdmDataBytes) /* here got to the next pdm file*/
        {
            TRACE_00(TRACE_LEVEL_WARN, "End of file, next file start");
            pdmpcm_push_pdm_buffer(queuePDMPCM._pdmDataPointer);
            if (nv_stop_pcm() == false)
            {
                TRACE_00(TRACE_LEVEL_ERROR, "nv_stop_pcm failed");
                break;
            }
            if (nv_start_pcm() == false)
            {
                TRACE_00(TRACE_LEVEL_ERROR, "nv_start_pcm failed");
                break;
            }
            isNewFile = true;
        }
    } while (isNewFile == true);
}


extern "C" void task_nv_pcm(void* pParameters)
{
    TRACE_00(TRACE_LEVEL_LOG,"Start of nv task for pcm");
    
    if (nv_start_pcm() == false)
    {
        TRACE_00(TRACE_LEVEL_ERROR, "nv init failed");
        vTaskDelete(NULL);
        return;
    }


    getPDMtoPCMData();

    TaskQueueNV queue;
    QueueHandle_t xQueue = *(QueueHandle_t*)pParameters;
    TickType_t xTicksToWait = 0;

    for (;; )
    {
        if (xQueueReceive(xQueue, &queue, xTicksToWait) == pdPASS)
        {
            switch (queue._opcode)
            {
            case NV_PCM_START:
                if (nv_start_pcm() != true)
                {
                    TRACE_00(TRACE_LEVEL_ERROR, "Start nv pdm failed");
                }
                break;
            case NV_OPCODE_WRITE_PCM_DATA:
                if (nv_write_pcm_record_data(queue._pData, queue._dataLengthBytes) == false)
                {
                    TRACE_00(TRACE_LEVEL_ERROR, "Write pcm data failed");
                }
                pdmpcm_push_pcm_buffer(reinterpret_cast<int16_t*>(queue._pData));
                getPDMtoPCMData();
                break;
            case NV_OPCODE_STOP_RESET:
                (void)nv_stop_pcm();
                NVIC_SystemReset();
                break;
            default:
                TRACE_01(TRACE_LEVEL_ERROR, "Unexpected nv opcode %i", queue._opcode);
                break;
            }
        }
    }
}
