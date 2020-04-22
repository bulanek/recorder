#include "./_com/tskma_tasks.h"

#include "recorder_inc.h"
#include "i2s_com.h"
#include "trace_com.h"
#include "trace_out.hpp"
#include "pdmpcm_com.h"



extern "C" void task_pdm_pcm(void* pParameters)
{
    TaskQueuePDMPCM queue;
    QueueHandle_t xQueue = *(QueueHandle_t*)pParameters;
    TickType_t xTicksToWait = portMAX_DELAY;

    /* pdmpcm_init() in nv task */;
    uint16_t pcmSize = i2s_get_buffer_size_word() / pdmpcm_get_decimation();
    int16_t* pPcmBuffer = static_cast<int16_t*>(pvPortMalloc(pcmSize));
    ASSERT(pPcmBuffer != nullptr, "null pPcmBuffer");

    TaskQueueNV nvQueue;
    for (;; )
    {
        if (xQueueReceive(xQueue, &queue, xTicksToWait) == pdPASS)
        {
            switch (queue._opcode)
            {
            case PDM_PCM_GET_PCM_DATA:
            {
                {
                    pdmpcm_process(reinterpret_cast<uint8_t*>(queue._pdmDataPointer), pPcmBuffer, pcmSize);// numMsPDMSamples* pcmSizeWord);

                    nvQueue._opcode = NV_OPCODE_WRITE_PCM_DATA;
                    nvQueue._pData = reinterpret_cast<uint16_t*>(pPcmBuffer);
                    nvQueue._dataLength = pcmSize;
                }
                tskma_send_to_nv(&nvQueue);

                break;
            default:
                TRACE_01(TRACE_LEVEL_ERROR, "wrong opcode %i",queue._opcode);
                break;
            }
            }
        }
    }
}