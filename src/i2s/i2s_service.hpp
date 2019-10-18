#ifndef I2S_SERVICE_
#define I2S_SERVICE_

#include "recorder_inc.h"

namespace I2S
{
class Service
{
public:
    Service();
    bool Init(void);
    bool ReceiveData(const uint16_t data);
    inline uint16_t* GetBuffer1(void) { return _bufferForDMA1; }
    inline uint16_t* GetBuffer2(void) { return _bufferForDMA2; }
    static const uint16_t  BUFFER_SIZE_WORDS = 0x500U; /**< 1024 bytes (word=2bytes)*/

private:
    bool IsAboveThr(void);
    static const uint16_t MAX_VALUE_BUFFER = 200U; /**< Later the threshold for value*/
    uint16_t _bufferI2S[BUFFER_SIZE_WORDS];
    uint16_t _bufferPosition;

    uint16_t _bufferForDMA1[BUFFER_SIZE_WORDS];
    uint16_t _bufferForDMA2[BUFFER_SIZE_WORDS];
};
} /* namespace I2S */
#endif // !I2S_SERVICE_

