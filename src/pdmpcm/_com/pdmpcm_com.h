#ifndef PDMPCM_COM_H
#define PDMPCM_COM_H

#include "recorder_inc.h"

typedef struct
{
    /* The canonical WAVE format starts with the RIFF header: */
    char        _chunkId[4];    /*<< "RIFF", big-endian. */
    uint32_t    _chunkSize;     /*<< 36 + SubChunk2Size, or more precisely: 4 + (8 + SubChunk1Size) + (8 + SubChunk2Size)
                                This is the size of the rest of the chunk following this number.  This is the size of the entire file in bytes minus 8 bytes for the
                               two fields not included in this count: ChunkID and ChunkSize.*/
    char        _format[4];     /*<< "WAVE", big-endian.*/
    /*  The "WAVE" format consists of two subchunks: "fmt " and "data":
        The "fmt " subchunk describes the sound data's format: */
    char        _subChunkId[4]; /*<< "fmt ", big-endian*/
    uint32_t    _subChunkSize;  /*<< 16 for PCM.  This is the size of the rest of the Subchunk which follows this number. */
    uint16_t    _audioFormat;   /*<< PCM = 1*/
    uint16_t    _numChannels;   /*<< Mono = 1, Stereo = 2, ..*/
    uint32_t    _sampleRate;    /*<< e.g. 8000, 44100,..*/
    uint32_t    _byteRate;      /*<< = SampleRate * NumChannels * BitsPerSample/8. */
    uint16_t    _blockAlign;    /*<< = NumChannels * BitsPerSample/8 */
    uint16_t    _bitsPerSample; /*<< e.g. 8 bits = 8*/
    /* The "data" subchunk contains the size of the data and the actual sound: */
    char        _subchunk2ID[4];/*<< Contains the letters "data" (0x64617461 big-endian form).*/
    uint32_t    _subchunk2Size; /*<< == NumSamples * NumChannels * BitsPerSample/8. This is the number of bytes in the data.
                               You can also think of this as the size of the read of the subchunk following this number. */
} WaveFormatHeader;


#ifdef __cplusplus
extern "C" {
#endif


bool pdmpcm_init(void);
void pdmpcm_process(const uint8_t* const pDataPDMIn, int16_t* pDataPCMOut);


/** Init gpio and interrupt. */
#ifndef PDMPCM_INIT
void PDMPCM_INIT(void);
#endif

#ifndef PDMPCM_START 
void PDMPCM_START(void);
#endif

/** Size of pdm buffer for 1 ms */
uint16_t pdmpcm_get_pdm_size_in_bytes(void);
uint16_t pdmpcm_get_pcm_size_in_bytes(void);

uint8_t* pdmpcm_pop_pdm_buffer(void);
int16_t* pdmpcm_pop_pcm_buffer(void);

void pdmpcm_push_pdm_buffer(uint8_t* const pBufferPDM);
void pdmpcm_push_pcm_buffer(int16_t* const pBufferPCM);


void fillWaveHeader(const uint32_t numDataBytes, WaveFormatHeader* const pDataBuffer);



#ifdef __cplusplus
}
#endif



#endif /* PDMPCM_COM_H */