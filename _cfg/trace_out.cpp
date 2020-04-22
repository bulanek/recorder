#include "trace_com.h"
#include "trace_hash.hpp"
#include "trace_out.hpp"
#include "recorder_cfg.h"
#include "recorder_inc.h"


#define TRACE_FILE_NAME "trc_log.bin"
FATFS   g_FatFs;				                /**< File system object for each logical drive */
FIL     g_infoFile;


static TraceIf* pTrace = nullptr;

bool TRACE_INIT(void)
{
    bool retVal = true;
    do
    {
        pTrace = TraceFactory::GetTraceFactory().CreateTraceObj();
        if (pTrace == nullptr)
        {
            retVal = false;
            break;
        }
        if ((retVal = pTrace->Init(TRACE_BUFFER_SIZE_BYTES / 4)) == false)
        {
            break;
        }

        //FRESULT res = f_mount(&g_FatFs, "", 0);
        //if (res != FR_OK && res != FR_EXIST)
        //{
        //    retVal = false;
        //    break;
        //}

        ///* Read info file if exist*/
        //res = f_open(&g_infoFile, TRACE_FILE_NAME, FA_CREATE_NEW | FA_WRITE );
        //if (res != FR_OK)
        //{
        //    retVal = false;
        //    break;
        //}
    } while (0);
    return retVal;
}

bool TRACE_DUMP(const uint32_t pBuffer[], const uint16_t size)
{
    UINT bytesWritten;
    bool retVal = true;
    //if (f_write(&g_infoFile, pBuffer, size, &bytesWritten) != FR_OK)
    //{
    //    retVal = false;
    //}
    return retVal;
}

uint32_t TRACE_GET_TIMESTAMP(void)
{
    return 0; /* TODO*/
}

void TRACE_00(const TRACE_LEVEL level, const char pMsg [], const  std::experimental::source_location& location)
{
    pTrace->addEntry_00(level, location.line(), COMPILE_TIME_CRC16_STR(location.file_name()));
    printf("Trace in %s, level %i, line %i: %s\n", location.file_name(), level, location.line(), pMsg);
}

void TRACE_01(const TRACE_LEVEL level, const char pMsg[], const uint32_t par1, const  std::experimental::source_location& location)
{
    pTrace->addEntry_01(level, location.line(), COMPILE_TIME_CRC16_STR(location.file_name()), par1);
    printf("Trace in %s, level %i, line %i: ", location.file_name(), level, location.line());
    printf(pMsg, par1);
    printf("\n");
}

void TRACE_02(const TRACE_LEVEL level, const char pMsg[], const uint32_t par1, const uint32_t par2, const  std::experimental::source_location& location)
{
    pTrace->addEntry_02(level, location.line(), COMPILE_TIME_CRC16_STR(location.file_name()), par1, par2);
    printf("Trace in %s, level %i, line %i: ", location.file_name(), level, location.line());
    printf(pMsg, par1, par2);
    printf("\n");
}

