#include "_com/pdmpcm_com.h"
#include "trace_out.hpp"
#include "trace_com.h"

TPDMFilter_InitStruct f_filter;

uint16_t f_gain;
uint16_t f_pdmBufLenWord, f_pcmBufLenWord;
int16_t* f_pBufferPCMWord;
uint8_t* f_pBufferPDMBytes;

uint32_t f_pdmSamplingHz = 192000;
uint32_t f_decimationFactor = 64;
uint32_t f_pcmSamplingHz = f_pdmSamplingHz / f_decimationFactor;

static_assert(sizeof(WaveFormatHeader) == 44, "Size of WaveFormatHeader not 44");

bool pdmpcm_init(void)
{
    bool retVal = true;
    do
    {
        f_pdmBufLenWord = f_pdmSamplingHz / 1000;
        f_pcmBufLenWord = f_pdmBufLenWord  / f_decimationFactor;
        f_gain = 1;

        f_filter.LP_HZ = f_pcmSamplingHz / 2;
        f_filter.HP_HZ = 10;
        f_filter.Fs = f_pcmSamplingHz;
        f_filter.Out_MicChannels = f_filter.In_MicChannels = 1U;
        f_filter.nSamples = f_pcmBufLenWord;
        f_filter.Decimation = f_decimationFactor;
        Open_PDM_Filter_Init(&f_filter);

        f_pBufferPCMWord = static_cast<int16_t*>(pvPortMalloc(f_pcmBufLenWord));
        f_pBufferPDMBytes = static_cast<uint8_t*>(pvPortMalloc(f_pdmBufLenWord * 2));
        if (f_pBufferPCMWord == NULL || f_pBufferPDMBytes == NULL)
        {
            TRACE_02(TRACE_LEVEL_ERROR, "Zero length of pcm (%i) or pdm (%i) buffers", f_pcmBufLenWord, f_pdmBufLenWord );
            retVal = false;
        }
    } while (0);
    return retVal;
}

void pdmpcm_process( const uint8_t* const pDataPDMIn, int16_t* pDataPCMOut, uint16_t numSamples)
{
    f_filter.nSamples = numSamples;
    Open_PDM_Filter_64((uint8_t *) pDataPDMIn, pDataPCMOut, f_gain, &f_filter);
}

uint16_t pdmpcm_get_pdm_size_in_word(void)
{
    return f_pdmBufLenWord;
}
uint16_t pdmpcm_get_pcm_size_in_word(void)
{
    return f_pcmBufLenWord;
}

uint8_t* pdmpcm_pop_pdm_buffer(void)
{
    taskENTER_CRITICAL();
    uint8_t* pBuffer = f_pBufferPDMBytes;
    f_pBufferPDMBytes = NULL;
    taskEXIT_CRITICAL();
    return pBuffer;
}

int16_t* pdmpcm_pop_pcm_buffer(void)
{
    taskENTER_CRITICAL();
    int16_t* pBuffer = f_pBufferPCMWord;
    f_pBufferPCMWord = NULL;
    taskEXIT_CRITICAL();
    return pBuffer;
}

void pdmpcm_push_pdm_buffer(uint8_t* const pBufferPDM)
{
    taskENTER_CRITICAL();
    f_pBufferPDMBytes = pBufferPDM;
    taskEXIT_CRITICAL();
}

void pdmpcm_push_pcm_buffer(int16_t* const pBufferPCM)
{
    taskENTER_CRITICAL();
    f_pBufferPCMWord = pBufferPCM;
    taskEXIT_CRITICAL();
}

void fillWaveHeader(const uint32_t numDataBytes, WaveFormatHeader* const pDataBuffer)
{
    char chunkId[] = { 'R','I','F','F' };
    memcpy(pDataBuffer->_chunkId, chunkId, 4);
    char format[] = { 'W','A','V','E' };
    memcpy(pDataBuffer->_format, format, 4);
    char subchunkId[] = { 'f','m','t',' ' };
    memcpy(pDataBuffer->_subChunkId, subchunkId, 4);
    pDataBuffer->_subChunkSize = 16;
    pDataBuffer->_audioFormat = 1;
    pDataBuffer->_numChannels = 1;
    pDataBuffer->_sampleRate = f_pcmSamplingHz;
    pDataBuffer->_bitsPerSample = 16;
    pDataBuffer->_byteRate = pDataBuffer->_sampleRate * pDataBuffer->_numChannels * pDataBuffer->_bitsPerSample / 8;
    pDataBuffer->_blockAlign = pDataBuffer->_numChannels * pDataBuffer->_bitsPerSample / 8;
    char subchunk2Id[] = { 'd','a','t','a' };
    memcpy(pDataBuffer->_subchunk2ID, subchunk2Id, 4);
    pDataBuffer->_subchunk2Size = numDataBytes;// numSamples* pDataBuffer->_numChannels* pDataBuffer->_bitsPerSample / 8;
}

