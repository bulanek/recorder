#ifndef TRACE_COM_H
#define TRACE_COM_H

#include "recorder_inc.h"

typedef enum _TRACE_LEVEL
{
    TRACE_LOG,
    TRACE_WARN,
    TRACE_ERROR
} TRACE_LEVEL;

#ifdef __cplusplus
extern "C"
{
#endif

void trace_entry_00(TRACE_LEVEL level, const uint16_t line, const uint16_t sourceId) ;
void trace_entry_01(TRACE_LEVEL level, const uint16_t line, const uint16_t sourceId , const uint32_t par1);
void trace_entry_02(TRACE_LEVEL level, const uint16_t line, const uint16_t sourceId , const uint32_t par1, const uint32_t par2);

void trace_write(void);

#ifdef __cplusplus
}
#endif

#endif // !TRACE_COM_H
