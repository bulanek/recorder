#include "_com/pdmpcm_com.h"
#include "trace_out.hpp"
#include "trace_com.h"


uint16_t f_gain = 1U;
uint16_t f_pdmBufLenWord, f_pcmBufLenWord;
int16_t* f_pBufferPCMWord;
uint8_t* f_pBufferPDMBytes;

uint32_t f_pdmSamplingHz = 0; /* in pdmpcm_init*/
const uint32_t f_decimationFactor = 128U;
uint32_t f_pcmSamplingHz = 0; /* in pdmpcm_init */ 

static_assert(sizeof(WaveFormatHeader) == 44, "Size of WaveFormatHeader not 44");

bool pdmpcm_init(void)
{
    bool retVal = true;
    do
    {
        f_pdmSamplingHz = (((uint32_t)16000) * RECORDER_I2S_PLLCFGR_PLLN)/ RECORDER_PLLCFGR_PLLM / RECORDER_I2S_PLLCFGR_PLLR ; /* i2s clock here*/

        f_pdmSamplingHz /= (32U * (2U * RECORDER_I2S_I2SSPR_DIV + RECORDER_I2S_I2SSPR_ODD)); /* See RefMan: 20.4.4 Clock generator*/

        f_pdmSamplingHz *= 1000U;

        f_pcmSamplingHz = f_pdmSamplingHz * 16U / f_decimationFactor;

        //f_pcmSamplingHz /= 2;

        TPDMFilter_InitStruct filterPDM;
        filterPDM.LP_HZ = 1000;// f_pcmSamplingHz / 2U;
        filterPDM.HP_HZ = 30;
        filterPDM.Fs = f_pcmSamplingHz;
        filterPDM.Out_MicChannels = filterPDM.In_MicChannels = 1U;
        filterPDM.Decimation = f_decimationFactor;
        Open_PDM_Filter_Init(&filterPDM);
    } while (0);
    return retVal;
}

void pdmpcm_process( const uint8_t* const pDataPDMIn, int16_t* pDataPCMOut, uint16_t numOutSamples)
{
    Open_PDM_Filter_128((uint8_t *) pDataPDMIn, pDataPCMOut, f_gain, numOutSamples);
}

uint16_t pdmpcm_get_decimation(void) { return f_decimationFactor; }

uint16_t pdmpcm_get_pdm_size_in_word(void)
{
    return f_pdmBufLenWord;
}

uint16_t pdmpcm_get_pcm_size_samples(const uint16_t pdmSizeBufferWord)
{
    uint16_t pcmSizeBytes = pdmSizeBufferWord * 2;
    pcmSizeBytes /= (f_decimationFactor / 8 );
    return pcmSizeBytes;
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
    pDataBuffer->_subchunk2Size = numDataBytes;// numOutSamples* pDataBuffer->_numChannels* pDataBuffer->_bitsPerSample / 8;
}

