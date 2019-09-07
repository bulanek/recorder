#include "_com/trace_com.h"
#include "trace_service.hpp"

static TraceService f_traceService;

void trace_entry_00(TRACE_LEVEL level, const uint16_t line, const uint16_t sourceId )
{
    f_traceService.addEntry_00(level, line, sourceId);
}

void trace_entry_01(TRACE_LEVEL level, const uint16_t line, const uint16_t sourceId , const uint32_t par1)
{
    f_traceService.addEntry_01(level, line, sourceId, par1);
}

void trace_entry_02(TRACE_LEVEL level, const uint16_t line, const uint16_t sourceId , const uint32_t par1, const uint32_t par2)
{
    f_traceService.addEntry_02(level, line, sourceId, par1, par2);
}

void trace_write(void)
{
    f_traceService.TraceWrite();
}

