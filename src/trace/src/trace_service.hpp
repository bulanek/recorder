#ifndef TRACE_SERVICE_HPP
#define TRACE_SERVICE_HPP

#include "recorder_inc.h"
#include "trace_com.h"
#include "trace_out.hpp"


static_assert(sizeof(TRACE_HEADER) == 12);
const uint16_t TRACE_START_ID = TRACE_START; /**< Identification of trace entry start. */

class TraceService : public TraceIf
{
public:
    TraceService();
    virtual bool Init(const uint16_t arraySize);
    virtual void addEntry_00(TRACE_LEVEL level, const uint16_t line, const uint16_t sourceId);
    virtual void addEntry_01(TRACE_LEVEL level, const uint16_t line, const uint16_t sourceId, const uint32_t param1);
    virtual void addEntry_02(TRACE_LEVEL level, const uint16_t line, const uint16_t sourceId, const uint32_t param1, const uint32_t param2);
    virtual bool TraceWrite(void);

private:
    void addHeader(TRACE_LEVEL level, const uint16_t line, const uint16_t sourceId);
    void addParam(const uint32_t param);
    void positionInc(void);

    uint32_t* _buffer;
    uint16_t _bufferSize;
    uint16_t _position;
};

#endif // !TRACE_SERVICE_HPP

