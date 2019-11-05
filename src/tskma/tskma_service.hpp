#ifndef TSKMA_SERVICE

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

namespace TSKMA
{
/* Priorities for the demo application tasks. */
const uint16_t  UART_TASK_PRIORITY = (tskIDLE_PRIORITY + 0);
#define UART_TASK_NAME "TaskI2S"

const uint16_t FLASH_TASK_PRIORITY = (tskIDLE_PRIORITY + 0);
#define NV_TASK_NAME "TaskNV"

const uint16_t PDM_PCM_TASK_PRIORITY = (tskIDLE_PRIORITY + 0);
#define PDM_PCM_TASK_NAME "TaskPDM_PCM"

const uint16_t UART_TASK_QUEUE_NUM_ITEMS = 10;
const uint16_t NV_TASK_QUEUE_NUM_ITEMS = 20;
const uint16_t PDM_PCM_TASK_QUEUE_NUM_ITEMS = 10;

typedef struct _TaskInfo
{
    TaskHandle_t    _taskHandle;
    QueueHandle_t   _queueHandle;
} TaskInfo;

enum Tasks
{
    TASK_UART = 0,
    TASK_NV,
    TASK_PDM_PCM,
    TASK_NUM
};

class Service
{
public:
    bool Init(void);
    inline const TaskInfo* GetTaskInfo(void)const { return _tasks; }
private:
    TaskInfo _tasks[TASK_NUM];
};
} /* namespace TSKMA*/

#endif // !TSKMA_SERVICE

