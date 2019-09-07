#ifndef TSKMA_TASKS_
#define TSKMA_TASKS_

#include "recorder_inc.h"

typedef enum
{
    PDM_PCM_GET_PCM_DATA
}PDMPCMOpcode;

typedef enum
{
    NV_PDM_START,
    NV_PDM_STOP,
    NV_PCM_START,
    NV_PCM_STOP,
    NV_OPCODE_WRITE_PDM,
    NV_OPCODE_WRITE_PCM,
    NV_OPCODE_GET_PDM_DATA,
    NV_OPCODE_WRITE_PCM_DATA,
    NV_OPCODE_STOP_RESET
} NVOpcode;


typedef struct _TaskQueueI2S
{
    uint16_t        _data;
} TaskQueueI2S;

typedef struct _TaskQueuePDMPCM
{
    PDMPCMOpcode    _opcode;
    uint8_t*        _pdmDataPointer;
    uint16_t        _sizePdmDataBytes;
} TaskQueuePDMPCM;

typedef struct _TaskQueueNV
{
    uint16_t*        _pData;
    uint16_t         _dataLengthBytes;
    NVOpcode         _opcode;
} TaskQueueNV;

#ifdef __cplusplus
extern "C"
{
#endif

bool tskma_initialize(void);

void task_i2s(void* pParameters);
void task_nv(void* pParameters);
void task_nv_pcm(void* pParameters);
void task_pdm_pcm(void* pParameters);


void tskma_send_to_i2s_irt(const TaskQueueI2S* const pQueueData);
bool tskma_send_to_nv_irt(const TaskQueueNV* const pQueueData);
bool tskma_send_to_nv(const TaskQueueNV* const pQueueData);
bool tskma_send_to_i2s(const TaskQueueI2S* const pQueueData);
bool tskma_send_to_pdm_pcm(const TaskQueuePDMPCM* const pQueueData);



#ifdef __cplusplus
}
#endif

#endif /* TSKMA_TASKS_ */
