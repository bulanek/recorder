#include "trace_service.hpp"
#include "trace_out.hpp"
#include "trace_hash.hpp"

constexpr const char* pFileName = file_name(__FILE__);

TraceService::TraceService() : _buffer(nullptr), _bufferSize(0), _position(0)
{}

bool TraceService::Init(const uint16_t arraySize)
{
    bool retVal = true;
    do
    {
        _buffer = static_cast<uint32_t*>(pvPortMalloc(arraySize * 4));
        _bufferSize = arraySize;
        if (_buffer == nullptr)
        {
            retVal = false;
            break;
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
    header._traceStart = TRACE_START_ID;
    header._level = level;
    header._line = line;
    header._sourceId = sourceId;
    header._timeStamp = TRACE_GET_TIMESTAMP();
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
    _position %= _bufferSize;
}

