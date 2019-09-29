#ifndef TRACE_SERVICE_HPP
#define TRACE_SERVICE_HPP

#include "recorder_inc.h"
#include "trace_com.h"
#include "trace_out.h"


typedef struct 
{
    uint16_t _traceStart;
    uint16_t _level;
    uint16_t _line;
    uint16_t _sourceId;
} TRACE_HEADER;
static_assert(sizeof(TRACE_HEADER) == 8);
const uint16_t TRACE_START = 0xdead; /**< Identification of trace entry start. */

class TraceService
{
public:
    TraceService();
    bool Init(void);
    void addEntry_00(TRACE_LEVEL level, const uint16_t line, const uint16_t sourceId);
    void addEntry_01(TRACE_LEVEL level, const uint16_t line, const uint16_t sourceId, const uint32_t param1);
    void addEntry_02(TRACE_LEVEL level, const uint16_t line, const uint16_t sourceId, const uint32_t param1, const uint32_t param2);
    bool TraceWrite(void);

private:
    void addHeader(TRACE_LEVEL level, const uint16_t line, const uint16_t sourceId);
    void addParam(const uint32_t param);
    void positionInc(void);
    static const uint16_t BUFFER_SIZE = TRACE_BUFFER_SIZE_BYTES / 4; /**< Trace buffer size. */

    uint32_t _buffer[BUFFER_SIZE];
    uint16_t _position;
};

#endif // !TRACE_SERVICE_HPP

