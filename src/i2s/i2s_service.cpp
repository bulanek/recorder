#include "recorder_inc.h"

#include "i2s_service.hpp"
#include "tskma_tasks.h"

#include "i2s_com.h"
#include "trace_out.hpp"
#include "trace_com.h"
#include "nv_com.h"




namespace I2S {
Service::Service()
{
}

bool Service::Init(void)
{
    I2S_INIT_MIC();
    return true;
}

bool Service::ReceiveData(const uint16_t data)
{
    bool retVal = true;
    _bufferI2S[_bufferPosition] = data;
    ++_bufferPosition;
    do
    {
        //if (_bufferPosition >= BUFFER_SIZE_WORDS)
        //{
        //    _bufferPosition = 0;
        //    //if (this->IsAboveThr() == true)
        //    //{
        //    //uint16_t* pData = (uint16_t*)pvPortMalloc(BUFFER_SIZE_WORDS * 2);
        //    uint16_t pData[BUFFER_SIZE_WORDS];
        //    if (pData == nullptr)
        //    {
        //        retVal = false;
        //        TRACE_00(TRACE_LEVEL_ERROR, "Failed to allocate memory for data");
        //        break;
        //    }
        //    memcpy(pData, _bufferI2S, BUFFER_SIZE_WORDS * 2);
        //    TaskQueueNV taskQueue;
        //    taskQueue._pData = pData;
        //    taskQueue._dataLengthBytes = BUFFER_SIZE_WORDS * 2;
        //    taskQueue._opcode = NV_OPCODE_WRITE_PDM;
        //    nv_write_pdm_record_data(pData, BUFFER_SIZE_WORDS * 2);
        //    //if ((retVal = tskma_send_to_nv(&taskQueue)) != true)
        //    //{
        //    //    TRACE_00(TRACE_LEVEL_ERROR, "Failed to send data to nv task");
        //    //    break;
        //    //}
        //    //}
        //}
    } while (0);
    return retVal;
}

bool Service::IsAboveThr(void)
{
    uint32_t value = 0;
    for (int i = 0; i < BUFFER_SIZE_WORDS; i++)
    {
        value += _bufferI2S[i];
    }
    return value >= MAX_VALUE_BUFFER ? true : false;
}

} /* namespace I2S */

