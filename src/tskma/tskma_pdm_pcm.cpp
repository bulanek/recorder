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
    uint16_t pcmSizeWord = pdmpcm_get_pcm_size_in_word();
    uint16_t pdmSizeWord = pdmpcm_get_pdm_size_in_word();
    uint16_t decimation = 64;
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
                    uint16_t numMsPDMSamples = queue._sizePdmDataWord / pdmSizeWord;

                    static int16_t* pPcmBuffer = static_cast<int16_t*>(pvPortMalloc(2 * pcmSizeWord * numMsPDMSamples));
                    if (pPcmBuffer == nullptr)
                    {
                        TRACE_00(TRACE_LEVEL_ERROR, "failed alloc pcm buffer");
                        break;
                    }

                    pdmpcm_process(reinterpret_cast<uint8_t*>(queue._pdmDataPointer), pPcmBuffer, 1);// numMsPDMSamples* pcmSizeWord);

                    nvQueue._opcode = NV_OPCODE_WRITE_PCM_DATA;
                    nvQueue._pData = reinterpret_cast<uint16_t*>(pPcmBuffer);
                    nvQueue._dataLengthBytes = 2 * pcmSizeWord * numMsPDMSamples;
                    static unsigned int counter = 0;
                    if (++counter % 200 == 0) tskma_send_to_nv(&nvQueue);
                }
                break;
            default:
                ASSERT("wrong opcode");
                break;
            }
            }
        }
    }
}