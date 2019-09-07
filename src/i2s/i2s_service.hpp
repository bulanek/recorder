#ifndef I2S_SERVICE_
#define I2S_SERVICE_

#include "recorder_inc.h"

namespace I2S
{
class Service
{
public:
    bool Init(void);
    bool ReceiveData(const uint16_t data);

private:
    bool IsAboveThr(void);
    static const uint16_t  BUFFER_SIZE_WORDS = 0x200U; /**< 1024 bytes (word=2bytes)*/
    static const uint16_t MAX_VALUE_BUFFER = 200U; /**< Later the threshold for value*/
    uint16_t _bufferI2S[BUFFER_SIZE_WORDS];
    uint16_t _bufferPosition;
};
} /* namespace I2S */
#endif // !I2S_SERVICE_

