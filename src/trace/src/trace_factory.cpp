#include "trace_com.h"
#include "trace_service.hpp"

TraceFactory& TraceFactory::GetTraceFactory(void)
{
    static TraceFactory factory;
    return factory;
}

TraceIf* TraceFactory::CreateTraceObj(void)
{
    TraceService* pService = new TraceService();
    return static_cast<TraceIf*>(pService);
}

