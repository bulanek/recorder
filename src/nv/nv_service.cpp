#include "nv_service.hpp"
#include "_com/nv_com.h"
#include "tskma_tasks.h"
#include "pdmpcm_com.h"
#include "trace_com.h"
#include "trace_out.hpp"

namespace NV
{
bool Service::Init(void)
{
    bool retVal = true;
    do
    {
        /* NV_INIT(); Note BoBu while f_open called */
        FRESULT res = f_mount(&_FatFs, "", 0);
        if (res != FR_OK && res != FR_EXIST)
        {
            retVal = false;
            break;
        }

        /* Read info file if exist*/
        res = f_open(&_infoFile, NV_FILE_INFO, FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
        if (res != FR_OK)
        {
            TRACE_01(TRACE_LEVEL_ERROR, "Failed open info file: %i", res);
            retVal = false;
            break;
        }
        UINT numReadBytes;
        res = f_read(&_infoFile, &_configInfo, sizeof(_configInfo), &numReadBytes);
        if (res != FR_OK || numReadBytes != sizeof(_configInfo))
        {
            _configInfo._filePDMCounter = 0;
            _configInfo._filePCMCounter = 0;
        }
        if (res != FR_OK)
        {
            break;
        }
        res = f_close(&_infoFile);
        if (res != FR_OK)
        {
            retVal = false;
            break;
        }
    } while (0);
    return retVal;
}

bool Service::StartPDM(void)
{
    bool retVal = true;
    FRESULT res;
    UINT bytesWritten;
    do
    {
        res = f_open(&_infoFile, NV_FILE_INFO, FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
        if (res != FR_OK)
        {
            retVal = false;
            break;
        }
        UINT numReadBytes;
        res = f_read(&_infoFile, &_configInfo, sizeof(_configInfo), &numReadBytes);
        if (res != FR_OK)
        {
            retVal = false;
            break;
        }

        res = f_lseek(&_infoFile, 0);
        if (res != FR_OK)
        {
            retVal = false;
            break;
        }

        ++_configInfo._filePDMCounter;

        SetFileName(PDM, _configInfo._filePDMCounter);
        res = f_open(&_pdmpcmFile._FilePDM, _pdmpcmFile._fileNamePDM, FA_OPEN_APPEND | FA_OPEN_ALWAYS | FA_WRITE);
        if (res != FR_OK)
        {
            retVal = false;
            break;
        }

        res = f_write(&_infoFile, &_configInfo, sizeof(_configInfo), &bytesWritten);
        if (res != FR_OK)
        {
            retVal = false;
            break;
        }
        res = f_close(&_infoFile);
        if (res != FR_OK)
        {
            retVal = false;
            break;
        }
        _pdmStarted = true;

    } while (0);
    return retVal;
}

bool Service::StopPDM(void)
{
    bool retVal = true;
    FRESULT res;
    do
    {
        res = f_close(&_pdmpcmFile._FilePDM);
        if (res != FR_OK)
        {
            retVal = false;
            break;
        }
    } while (0);
    return retVal;
}

bool Service::StartPCM(void)
{
    bool retVal = true;
    FRESULT res;
    UINT bytesWritten;

    do
    {
        res = f_open(&_infoFile, NV_FILE_INFO, FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
        if (res != FR_OK)
        {
            retVal = false;
            TRACE_01(TRACE_LEVEL_ERROR, "Failed open info file: %i", res);
            break;
        }
        UINT numReadBytes;
        res = f_read(&_infoFile, &_configInfo, sizeof(_configInfo), &numReadBytes);
        if (res != FR_OK)
        {
            retVal = false;
            TRACE_01(TRACE_LEVEL_ERROR, "Failed read info file: %i", res);
            break;
        }

        res = f_lseek(&_infoFile, 0);
        if (res != FR_OK)
        {
            TRACE_01(TRACE_LEVEL_ERROR, "Failed lseek info file: %i", res);
            retVal = false;
            break;
        }

        if (_configInfo._filePCMCounter == 0)
        {
            ++_configInfo._filePCMCounter;
        }

        SetFileName(PCM, _configInfo._filePCMCounter);
        res = f_open(&_pdmpcmFile._FilePCM, _pdmpcmFile._fileNamePCM,  FA_CREATE_ALWAYS |  FA_WRITE);
        if (res != FR_OK)
        {
            retVal = false;
            TRACE_01(TRACE_LEVEL_ERROR, "Failed open pcm file: %i", res);
            break;
        }
        /* write header*/
        WaveFormatHeader header;
        fillWaveHeader(0, &header);
        res = f_write(&_pdmpcmFile._FilePCM, &header, sizeof(WaveFormatHeader), &bytesWritten);
        if (res != FR_OK)
        {
            retVal = false;
            TRACE_01(TRACE_LEVEL_ERROR, "Failed write pcm file: %i", res);
            break;
        }

        SetFileName(PDM, _configInfo._filePCMCounter);
        res = f_open(&_pdmpcmFile._FilePDM, _pdmpcmFile._fileNamePDM, FA_READ);
        if (res != FR_OK)
        {
            retVal = false;
            TRACE_01(TRACE_LEVEL_ERROR, "Failed open config file: %i", res);
            break;
        }

        res = f_write(&_infoFile, &_configInfo, sizeof(_configInfo), &bytesWritten);
        if (res != FR_OK)
        {
            TRACE_01(TRACE_LEVEL_ERROR, "Failed write config file: %i", res);
            retVal = false;
            break;
        }
        res = f_close(&_infoFile);
        if (res != FR_OK)
        {
            TRACE_01(TRACE_LEVEL_ERROR, "Failed close config file: %i", res);
            retVal = false;
            break;
        }
        TRACE_01(TRACE_LEVEL_LOG, "Start of writing pcm file with index %i", _configInfo._filePCMCounter);
    } while (0);
    return retVal;
}

bool Service::StopPCM(void)
{
    bool retVal = true;
    FRESULT res;
    UINT bytesWritten;
    do
    {
        /* update header on size*/
        WaveFormatHeader header;
        fillWaveHeader(_pdmpcmFile._FilePCM.fptr, &header);
        res = f_lseek(&_pdmpcmFile._FilePCM, 0);
        if (res != FR_OK)
        {
            retVal = false;
            break;
        }
        res = f_write(&_pdmpcmFile._FilePCM, &header, sizeof(WaveFormatHeader), &bytesWritten);
        if (res != FR_OK)
        {
            retVal = false;
            break;
        }

        res = f_close(&_pdmpcmFile._FilePCM);
        if (res != FR_OK)
        {
            retVal = false;
            break;
        }

        res = f_open(&_infoFile, NV_FILE_INFO, FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
        if (res != FR_OK)
        {
            retVal = false;
            break;
        }

        res = f_lseek(&_infoFile, 0);
        if (res != FR_OK)
        {
            retVal = false;
            break;
        }
        ++_configInfo._filePCMCounter; /* Just only succesfull full conversion write change PCM file counter.*/

        res = f_write(&_infoFile, &_configInfo, sizeof(_configInfo), &bytesWritten);
        if (res != FR_OK)
        {
            retVal = false;
            break;
        }
        res = f_close(&_infoFile);
        if (res != FR_OK)
        {
            retVal = false;
            break;
        }
    } while (0);
    return retVal;
}

bool Service::GetPDMData(const uint16_t size, uint8_t* pBuffer, uint16_t& numBytesRead)
{
    bool retVal = true;
    UINT numBytesReadInt;
    FRESULT res = f_read(&_pdmpcmFile._FilePDM, pBuffer, size, &numBytesReadInt);
    numBytesRead = numBytesReadInt;
    if (res != FR_OK)
    {
        retVal = false;
    }
    return retVal;
}

bool Service::WritePDMRecordData(const uint16_t* const pData, const uint16_t dataLengthBytes)
{
    bool retVal = true;
    UINT numBytesWritten;
    do
    {
        if (_pdmStarted == false)
        {
            retVal = false;
            break;
        }
        FRESULT res = f_write(&_pdmpcmFile._FilePDM, pData, dataLengthBytes, &numBytesWritten);

        if (res != FR_OK)
        {
            retVal = false;
            TRACE_01(TRACE_LEVEL_ERROR, "Write failed: %i", res);
            break;
        }
        //res = f_sync(&_pdmpcmFile._FilePDM);
        //if (res != FR_OK)
        //{
        //    retVal = false;
        //    TRACE_01(TRACE_LEVEL_ERROR, "Sync failed: %i", res);
        //    break;
        //}
    } while (0);
    return retVal;
}

bool Service::WritePCMRecordData(const uint16_t* const pData, const uint16_t dataLengthBytes)
{
    bool retVal = true;
    UINT numBytesWritten;
    FRESULT res = f_write(&_pdmpcmFile._FilePCM, pData, dataLengthBytes, &numBytesWritten);
    if (res != FR_OK)
    {
        retVal = false;
    }
    //res = f_sync(&_pdmpcmFile._FilePCM);
    //if (res != FR_OK)
    //{
    //    retVal = false;
    //}
    return retVal;
}

void Service::SetFileName(const FileType fileType, const uint16_t counter)
{
    char* ptr = _pdmpcmFile._fileNamePDM;
    if (fileType == PCM)
    {
        ptr = _pdmpcmFile._fileNamePCM;
    }
    memcpy(ptr, NV_FILE_NAME_PREFIX, STRLEN_STATIC(NV_FILE_NAME_PREFIX));
    ptr += STRLEN_STATIC(NV_FILE_NAME_PREFIX) ;
    *(ptr++) = char(counter / 100 % 10) + '0';
    *(ptr++) = char(counter / 10 % 10) + '0';
    *(ptr++) = char(counter % 10) + '0';
    if (fileType == PDM)
    {
        memcpy(ptr, NV_PDM_FILE_NAME_SUFFIX, STRLEN_STATIC(NV_PDM_FILE_NAME_SUFFIX));
    }
    else if (fileType == PCM)
    {
         memcpy(ptr, NV_PCM_FILE_NAME_SUFFIX, STRLEN_STATIC(NV_PCM_FILE_NAME_SUFFIX));       
    }
}

} /* namespace NV */

