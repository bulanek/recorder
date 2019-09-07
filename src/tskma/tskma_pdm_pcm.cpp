#include "./_com/tskma_tasks.h"

#include "recorder_inc.h"
#include "i2s_com.h"
#include "trace_com.h"
#include "pdmpcm_com.h"


extern "C" void task_pdm_pcm(void* pParameters)
{
    TaskQueuePDMPCM queue;
    QueueHandle_t xQueue = *(QueueHandle_t*)pParameters;
    TickType_t xTicksToWait = 0;

    /* pdmpcm_init() in nv task */;

    for (;; )
    {
        if (xQueueReceive(xQueue, &queue, xTicksToWait) == pdPASS)
        {
            switch (queue._opcode)
            {
            case PDM_PCM_GET_PCM_DATA:
            {
                int16_t* pBufferPCM = pdmpcm_pop_pcm_buffer();
                if (pBufferPCM != nullptr)
                {
                    pdmpcm_process(queue._pdmDataPointer, pBufferPCM);
                    pdmpcm_push_pdm_buffer(queue._pdmDataPointer);
                }
                TaskQueueNV nvQueue;
                nvQueue._opcode = NV_OPCODE_WRITE_PCM_DATA;
                nvQueue._pData = reinterpret_cast<uint16_t*>(pBufferPCM);
                nvQueue._dataLengthBytes = pdmpcm_get_pcm_size_in_bytes();
                tskma_send_to_nv(&nvQueue);
            }
            break;
            default:
                ASSERT("wrong opcode");
                break;
            }
        }
    }
}