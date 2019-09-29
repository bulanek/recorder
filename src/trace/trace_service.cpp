#include "trace_service.hpp"
#include "trace_out.h"
#include "trace_hash.hpp"

constexpr const char* pFileName = file_name(__FILE__);

TraceService::TraceService() : _position(0)
{
}

bool TraceService::Init(void)
{
    bool retVal = true;
    do
    {
        retVal = TRACE_INIT(_buffer,sizeof(_buffer));
        if (retVal == true)
        {
            TRACE_00(TRACE_LOG, "Trace initialized");
        }
    } while (0);
    return retVal;
}

void TraceService::addEntry_00(TRACE_LEVEL level, const uint16_t line, const uint16_t sourceId)
{
    taskENTER_CRITICAL();
    this->addHeader(level, line, sourceId);
    taskEXIT_CRITICAL();
}

void TraceService::addEntry_01(TRACE_LEVEL level, const uint16_t line, const uint16_t sourceId, const uint32_t param1)
{
    taskENTER_CRITICAL();
    this->addHeader(level, line, sourceId);
    this->addParam(param1);
    taskEXIT_CRITICAL();
}

void TraceService::addEntry_02(TRACE_LEVEL level, const uint16_t line, const uint16_t sourceId, const uint32_t param1, const uint32_t param2)
{
    taskENTER_CRITICAL();
    this->addHeader(level, line, sourceId);
    this->addParam(param1);
    this->addParam(param2);
    taskEXIT_CRITICAL();
}

bool TraceService::TraceWrite(void)
{
    bool retVal;
    taskENTER_CRITICAL();
    retVal = TRACE_DUMP(_buffer,sizeof(_buffer));
    taskEXIT_CRITICAL();
    return retVal;
}

void TraceService::addHeader(TRACE_LEVEL level, const uint16_t line, const uint16_t sourceId)
{
    TRACE_HEADER header;
    header._traceStart = TRACE_START;
    header._level = level;
    header._line = line;
    header._sourceId = sourceId;
    memcpy(&_buffer[_position], &header._traceStart, 4);
    this->positionInc();
    memcpy(&_buffer[_position], &header._line, 4);
    this->positionInc();
}

void TraceService::addParam(const uint32_t param)
{
    _buffer[_position] = param;
    this->positionInc();
}

void TraceService::positionInc(void)
{
    ++_position;
    _position %= BUFFER_SIZE;
}

