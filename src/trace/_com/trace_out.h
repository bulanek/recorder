#ifndef TRACE_OUT_H
#define TRACE_OUT_H

#include "recorder_cfg.h"

/*! @brief  Trace with no parameters. */
#ifndef TRACE_00
void  TRACE_00( TRACE_LEVEL level, const char msg[]);
#endif

#ifndef TRACE_01
void  TRACE_01( TRACE_LEVEL level, const char msg[], const uint32_t par1);
#endif

#ifndef TRACE_02
void  TRACE_02( TRACE_LEVEL level, const char msg[], const uint32_t par1, const uint32_t par2);
#endif


#endif // !TRACE_OUT_H
