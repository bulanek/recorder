#ifndef NV_COM_
#define NV_COM_

#include "recorder_inc.h"

typedef enum
{
    NV_MODE_NORMAL,
    NV_MODE_PDM_PCM
}NVMode;


#ifdef __cplusplus
extern "C"
{
#endif

bool nv_init(void);
bool nv_start_pdm(void);
bool nv_stop_pdm(void);

bool nv_start_pcm(void);
bool nv_stop_pcm(void);


bool nv_get_pdm_data(const uint16_t size, uint8_t* pBuffer,  uint16_t* numBytesRead);

/** pData pointer is valid in function only */
bool nv_write_pdm_record_data(const uint16_t* const pData, const uint16_t dataLengthBytes);
bool nv_write_pcm_record_data(const uint16_t* const pData, const uint16_t dataLengthBytes);


/** NV initialization (SPI). */
#ifndef NV_INIT
void NV_INIT(void);
#endif

#ifdef __cplusplus
}
#endif


#endif /* NV_COM_*/