#ifndef TRACE_OUT_H
#define TRACE_OUT_H

#include "recorder_cfg.h"
#include <experimental/source_location>
#include "trace_com.h"

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus*/

/*! @brief  Trace with no parameters. */
#ifndef TRACE_00
    void TRACE_00(const TRACE_LEVEL level, const char* pMsg,  const  std::experimental::source_location& location = std::experimental::source_location::current());
#endif

#ifndef TRACE_01
    void TRACE_01(const TRACE_LEVEL level, const char* pMsg, const uint32_t par1,  const  std::experimental::source_location& location = std::experimental::source_location::current());
#endif

#ifndef TRACE_02
    void TRACE_02(const TRACE_LEVEL level, const char* pMsg, const uint32_t par1, const uint32_t par2, const  std::experimental::source_location& location = std::experimental::source_location::current());
#endif

/** Initialize trace buffer (e.g. read from nv memory). */
#ifndef TRACE_INIT
bool TRACE_INIT(void);
#endif 

#ifndef TRACE_DUMP
bool TRACE_DUMP(const uint32_t pBuffer[], const uint16_t size);
#endif

#ifndef TRACE_GET_TIMESTAMP
uint32_t TRACE_GET_TIMESTAMP(void);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus*/


/** Size of trace buffer in bytes */
#ifndef TRACE_BUFFER_SIZE_BYTES
#define TRACE_BUFFER_SIZE_BYTES 0x400
#endif

#endif // !TRACE_OUT_H

