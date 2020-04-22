#include "recorder_inc.h"

#include "i2s_service.hpp"
#include "tskma_tasks.h"

#include "i2s_com.h"
#include "trace_out.hpp"
#include "trace_com.h"
#include "nv_com.h"

namespace I2S {
Service::Service()
{
}

bool Service::Init(void)
{
    I2S_INIT_MIC();
    return true;
}

} /* namespace I2S */

