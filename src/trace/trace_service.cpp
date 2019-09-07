#include "trace_service.hpp"

TraceService::TraceService() : _position(0)
{
}

bool TraceService::Init(void)
{
    bool retVal = true;
    do
    {
        FRESULT res = f_mount(&_FatFs, "", 0);
        if (res != FR_OK && res != FR_EXIST)
        {
            retVal = false;
            break;
        }

        /* Read info file if exist*/
        res = f_open(&_infoFile, TRACE_FILE_NAME, FA_CREATE_NEW | FA_WRITE );
        if (res != FR_OK)
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

void TraceService::TraceWrite(void)
{
    UINT bytesWritten;
    taskENTER_CRITICAL();
    (void) f_write(&_infoFile, _buffer, BUFFER_SIZE, &bytesWritten);
    taskEXIT_CRITICAL();
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
