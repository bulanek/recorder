#ifndef PDMPCM_COM_H
#define PDMPCM_COM_H

#include "recorder_inc.h"

#ifdef __cplusplus
extern "C"
{
#endif

bool pdmpcm_init(void);
void pdmpcm_process(const uint8_t* const pDataPDMIn, int16_t* pDataPCMOut);


/** Init gpio and interrupt. */
#ifndef PDMPCM_INIT
void PDMPCM_INIT(void);
#endif

#ifndef PDMPCM_START 
void PDMPCM_START(void);
#endif

/** Size of pdm buffer for 1 ms */
uint16_t pdmpcm_get_pdm_size_in_bytes(void);
uint16_t pdmpcm_get_pcm_size_in_bytes(void);

uint8_t* pdmpcm_pop_pdm_buffer(void);
int16_t* pdmpcm_pop_pcm_buffer(void);

void pdmpcm_push_pdm_buffer(uint8_t* const pBufferPDM);
void pdmpcm_push_pcm_buffer(int16_t* const pBufferPCM);



#ifdef __cplusplus
}
#endif



#endif /* PDMPCM_COM_H */