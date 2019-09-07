#include "recorder_inc.h"
#include "i2s_com.h"
#include "i2s_service.hpp"

I2S::Service f_I2SService;

extern "C" bool i2s_init(void)
{
    return f_I2SService.Init();
}

extern "C" void i2s_receive_data(const uint16_t data)
{
    f_I2SService.ReceiveData(data);
}
