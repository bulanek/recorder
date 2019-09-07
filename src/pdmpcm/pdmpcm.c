#include "_com/pdmpcm_com.h"

TPDMFilter_InitStruct f_filter;

uint16_t f_gain;
uint16_t f_pdmBufLen, f_pcmBufLen;
int16_t* f_pBufferPCM;
uint8_t* f_pBufferPDM;

bool pdmpcm_init(void)
{
    bool retVal = true;
    do
    {
        PDMPCM_INIT();

        uint32_t pdmSamplingHz = 2000000 / 16;
        uint16_t decimationFactor = 64;
        uint32_t pcmSamplingHz = pdmSamplingHz / decimationFactor;
        f_pdmBufLen = pdmSamplingHz / 1000;
        f_pcmBufLen = f_pdmBufLen / decimationFactor;
        f_gain = 1;

        f_filter.LP_HZ = pcmSamplingHz / 2;
        f_filter.HP_HZ = 10;
        f_filter.Fs = pcmSamplingHz;
        f_filter.Out_MicChannels = f_filter.In_MicChannels = 1U;
        f_filter.nSamples = f_pcmBufLen;
        f_filter.Decimation = decimationFactor;
        Open_PDM_Filter_Init(&f_filter);

        f_pBufferPCM = pvPortMalloc(f_pcmBufLen);
        f_pBufferPDM = pvPortMalloc(f_pdmBufLen);
        if (f_pBufferPCM == NULL || f_pBufferPDM == NULL)
        {
            retVal = false;
        }
    } while (0);
    return retVal;
}

void pdmpcm_process( const uint8_t* const pDataPDMIn, int16_t* pDataPCMOut)
{
    Open_PDM_Filter_64((uint8_t *) pDataPDMIn, pDataPCMOut, f_gain, &f_filter);
}

uint16_t pdmpcm_get_pdm_size_in_bytes(void)
{
    return f_pdmBufLen;
}
uint16_t pdmpcm_get_pcm_size_in_bytes(void)
{
    return f_pcmBufLen;
}

uint8_t* pdmpcm_pop_pdm_buffer(void)
{
    taskENTER_CRITICAL();
    uint8_t* pBuffer = f_pBufferPDM;
    f_pBufferPDM = NULL;
    taskEXIT_CRITICAL();
    return pBuffer;
}

int16_t* pdmpcm_pop_pcm_buffer(void)
{
    taskENTER_CRITICAL();
    int16_t* pBuffer = f_pBufferPCM;
    f_pBufferPCM = NULL;
    taskEXIT_CRITICAL();
    return pBuffer;
}

void pdmpcm_push_pdm_buffer(uint8_t* const pBufferPDM)
{
    taskENTER_CRITICAL();
    f_pBufferPDM = pBufferPDM;
    taskEXIT_CRITICAL();
}

void pdmpcm_push_pcm_buffer(int16_t* const pBufferPCM)
{
    taskENTER_CRITICAL();
    f_pBufferPCM = pBufferPCM;
    taskEXIT_CRITICAL();
}
