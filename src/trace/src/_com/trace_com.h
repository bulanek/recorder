#ifndef TRACE_COM_H
#define TRACE_COM_H

#include "recorder_cfg.h"

typedef enum _TRACE_LEVEL
{
    TRACE_LEVEL_LOG,
    TRACE_LEVEL_WARN,
    TRACE_LEVEL_ERROR
} TRACE_LEVEL;

#ifndef TRACE_START
#define TRACE_START 0xdead
#endif
typedef struct 
{
    uint16_t _traceStart;
    uint16_t _level;
    uint16_t _line;
    uint16_t _sourceId;
    uint32_t _timeStamp;
} TRACE_HEADER;

#ifdef __cplusplus

class TraceIf
{
public:
    virtual bool Init(const uint16_t arraySize) = 0;
    virtual void addEntry_00(TRACE_LEVEL level, const uint16_t line, const uint16_t sourceId) = 0;
    virtual void addEntry_01(TRACE_LEVEL level, const uint16_t line, const uint16_t sourceId, const uint32_t param1) = 0;
    virtual void addEntry_02(TRACE_LEVEL level, const uint16_t line, const uint16_t sourceId, const uint32_t param1, const uint32_t param2) = 0;
    virtual bool TraceWrite(void) = 0;
};

class TraceFactory
{
public:
    static TraceFactory& GetTraceFactory();
    TraceIf* CreateTraceObj();
private:
};

#endif /* __cplusplus */


#endif // !TRACE_COM_H

