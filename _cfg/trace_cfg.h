#ifndef TRACE_CFG_H
#define TRACE_CFG_H

#ifdef __cplusplus

#include "trace_hash.hpp"

#define TRACE_00(level, msg) { \
trace_entry_00(level, __LINE__, COMPILE_TIME_CRC16_STR(file_name(__FILE__))); \
if (level == TRACE_ERROR) printf(msg); \
}

#define TRACE_01(level, msg, par1) { \
trace_entry_01(level, __LINE__,COMPILE_TIME_CRC16_STR(file_name(__FILE__)), par1); \
if (level == TRACE_ERROR) printf(msg, par1); \
}

#define TRACE_02(level, msg, par1,par2) {\
trace_entry_02(level, __LINE__,COMPILE_TIME_CRC16_STR(file_name(__FILE__)), par1, par2); \
if (level == TRACE_ERROR) printf(msg, par1, par2); \
}


#endif /* __cplusplus */

#endif // !TRACE_CFG_H
