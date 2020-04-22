#ifndef NV_SERVICE_
#define NV_SERVICE_

#include "recorder_inc.h"
#include "hal_com.h"
#include "_com/nv_com.h"

namespace NV
{
class Service : public ServiceIf
{
public:
    Service(): _pdmStarted(false) {}
    virtual bool Init(void);
    virtual void Deinit(void);

    virtual bool StartPDM(void);
    virtual bool StopPDM(void);
    /**     Open PCM file from info*/
    virtual bool StartPCM(void);
    virtual bool StopPCM(void);

    virtual bool GetPDMData(const uint16_t size, uint8_t* pBuffer,  uint16_t& numBytesRead);

    virtual bool WritePDMRecordData(const uint16_t* const pData, const uint16_t dataLengthBytes);
    virtual bool WritePCMRecordData(const uint16_t* const pData, const uint16_t dataLength);
    virtual bool WriteTraceData(const uint8_t* const pData, const uint16_t dataLength);

    bool _pdmStarted;

private:
    typedef enum
    {
        PDM,
        PCM
    } FileType;

    typedef struct
    {
        uint16_t _filePDMCounter;
        uint16_t _filePCMCounter;
    } ConfigInfo;

    typedef enum
    {
        UNINITIALIZED,
        INITIALIZED,
        PDM_STARTED,
        PCM_STARTED,
    } State;


    #define NV_FILE_NAME_PREFIX "DMP"
    #define NV_PDM_FILE_NAME_SUFFIX ".pdm"
    #define NV_PCM_FILE_NAME_SUFFIX ".wav"
    #define NV_FILE_INFO "NV_INFO.bin"
    #define NV_FILE_TRACE "NV_TRACE.txt"

    static const uint16_t NV_FILE_NAME_SIZE = STRLEN_STATIC(NV_FILE_NAME_PREFIX) + 3 + 4 + 1 + 10; /* <NV_FILE_NAME_PREFIX>_XXX.txt*/
    static const uint16_t MAX_NUM_OF_FILES = 1000U;

    typedef struct 
    {
        char _fileNamePDM[NV_FILE_NAME_SIZE];
        char _fileNamePCM[NV_FILE_NAME_SIZE];
        FIL _FilePDM; 
        FIL _FilePCM;
    } PDMPCMFile;

    bool InitInfoFile(void);
    bool InitTraceFile(void);

    void SetFileName(const FileType type, const uint16_t counter);

    FATFS _FatFs;				                /** File system object for each logical drive */
    PDMPCMFile _pdmpcmFile;

    FIL _infoFile;                          /** Configuration file descriptor. */
    FIL _traceFile;                          /** Configuration file descriptor. */

    ConfigInfo  _configInfo;                 /** Configuration in @see _infoFile. */
    State       _state;
};
} /* namespace NV */
#endif // !NV_SERVICE_

