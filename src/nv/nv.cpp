#include "nv_com.h"
#include "nv_service.hpp"

NV::Service f_NVService;

extern "C" bool nv_init(void)
{
    return f_NVService.Init();
}

extern "C" bool nv_start_pdm(void)
{
    return f_NVService.StartPDM();
}

extern "C" bool nv_stop_pdm(void)
{
    return f_NVService.StopPDM();
}

extern "C" bool nv_start_pcm(void)
{
    return f_NVService.StartPCM();
}
extern "C" bool nv_stop_pcm(void)
{
    return f_NVService.StopPCM();
}

extern "C" bool nv_get_pdm_data(const uint16_t size, uint8_t* pBuffer, uint16_t* numBytesRead)
{
    return f_NVService.GetPDMData(size, pBuffer, *numBytesRead);
}

extern "C" bool nv_write_pdm_record_data(const uint16_t* const pData, const uint16_t dataLengthBytes)
{
    return f_NVService.WritePDMRecordData(pData, dataLengthBytes);
}

extern "C" bool nv_write_pcm_record_data(const uint16_t* const pData, const uint16_t dataLengthBytes)
{
    return f_NVService.WritePCMRecordData(pData, dataLengthBytes);
}

