/**
 *******************************************************************************
 * @file    OpenPDMFilter.c
 * @author  CL
 * @version V1.0.0
 * @date    9-September-2015
 * @brief   Open PDM audio software decoding Library.   
 *          This Library is used to decode and reconstruct the audio signal
 *          produced by ST MEMS microphone (MP45Dxxx, MP34Dxxx). 
 *******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2018 STMicroelectronics</center></h2>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *******************************************************************************
 */


/* Includes ------------------------------------------------------------------*/

#include "OpenPDMFilter.h"


/* Variables -----------------------------------------------------------------*/

uint32_t div_const = 0;
int64_t sub_const = 0;
uint32_t sinc[DECIMATION_MAX * SINCN];
uint32_t sinc1[DECIMATION_MAX];
uint32_t sinc2[DECIMATION_MAX * 2];
uint32_t coef[SINCN][DECIMATION_MAX];
#ifdef USE_LUT
int32_t lut[256][DECIMATION_MAX / 8][SINCN];
#endif


static TPDMFilter_InitStruct f_initStruct = { 0 };



/* Functions -----------------------------------------------------------------*/

#ifdef USE_LUT
int32_t filter_table_mono_64(uint8_t *data, uint8_t sincn)
{
  return (int32_t)
    lut[data[0]][0][sincn] +
    lut[data[1]][1][sincn] +
    lut[data[2]][2][sincn] +
    lut[data[3]][3][sincn] +
    lut[data[4]][4][sincn] +
    lut[data[5]][5][sincn] +
    lut[data[6]][6][sincn] +
    lut[data[7]][7][sincn];
}
int32_t filter_table_stereo_64(uint8_t *data, uint8_t sincn)
{
  return (int32_t)
    lut[data[0]][0][sincn] +
    lut[data[2]][1][sincn] +
    lut[data[4]][2][sincn] +
    lut[data[6]][3][sincn] +
    lut[data[8]][4][sincn] +
    lut[data[10]][5][sincn] +
    lut[data[12]][6][sincn] +
    lut[data[14]][7][sincn];
}
int32_t filter_table_mono_128(uint8_t *data, uint8_t sincn)
{
  return (int32_t)
    lut[data[0]][0][sincn] +
    lut[data[1]][1][sincn] +
    lut[data[2]][2][sincn] +
    lut[data[3]][3][sincn] +
    lut[data[4]][4][sincn] +
    lut[data[5]][5][sincn] +
    lut[data[6]][6][sincn] +
    lut[data[7]][7][sincn] +
    lut[data[8]][8][sincn] +
    lut[data[9]][9][sincn] +
    lut[data[10]][10][sincn] +
    lut[data[11]][11][sincn] +
    lut[data[12]][12][sincn] +
    lut[data[13]][13][sincn] +
    lut[data[14]][14][sincn] +
    lut[data[15]][15][sincn];
}
int32_t filter_table_stereo_128(uint8_t *data, uint8_t sincn)
{
  return (int32_t)
    lut[data[0]][0][sincn] +
    lut[data[2]][1][sincn] +
    lut[data[4]][2][sincn] +
    lut[data[6]][3][sincn] +
    lut[data[8]][4][sincn] +
    lut[data[10]][5][sincn] +
    lut[data[12]][6][sincn] +
    lut[data[14]][7][sincn] +
    lut[data[16]][8][sincn] +
    lut[data[18]][9][sincn] +
    lut[data[20]][10][sincn] +
    lut[data[22]][11][sincn] +
    lut[data[24]][12][sincn] +
    lut[data[26]][13][sincn] +
    lut[data[28]][14][sincn] +
    lut[data[30]][15][sincn];
}
int32_t (* filter_tables_64[2]) (uint8_t *data, uint8_t sincn) = {filter_table_mono_64, filter_table_stereo_64};
int32_t (* filter_tables_128[2]) (uint8_t *data, uint8_t sincn) = {filter_table_mono_128, filter_table_stereo_128};
#else
int32_t filter_table(uint8_t *data, uint8_t sincn)
{
  uint8_t c, i;
  uint16_t data_index = 0;
  uint32_t *coef_p = &coef[sincn][0];
  int32_t F = 0;
  uint8_t decimation = f_initStruct.Decimation;
  uint8_t channels = f_initStruct.In_MicChannels;

  for (i = 0; i < decimation; i += 8) {
    c = data[data_index];
    F += ((c >> 7)       ) * coef_p[i    ] +
         ((c >> 6) & 0x01) * coef_p[i + 1] +
         ((c >> 5) & 0x01) * coef_p[i + 2] +
         ((c >> 4) & 0x01) * coef_p[i + 3] +
         ((c >> 3) & 0x01) * coef_p[i + 4] +
         ((c >> 2) & 0x01) * coef_p[i + 5] +
         ((c >> 1) & 0x01) * coef_p[i + 6] +
         ((c     ) & 0x01) * coef_p[i + 7];
    data_index += channels;
  }
  return F;
}
#endif

void convolve(uint32_t Signal[/* SignalLen */], unsigned short SignalLen,
              uint32_t Kernel[/* KernelLen */], unsigned short KernelLen,
              uint32_t Result[/* SignalLen + KernelLen - 1 */])
{
  uint16_t n;

  for (n = 0; n < SignalLen + KernelLen - 1; n++)
  {
    unsigned short kmin, kmax, k;
    
    Result[n] = 0;
    
    kmin = (n >= KernelLen - 1) ? n - (KernelLen - 1) : 0;
    kmax = (n < SignalLen - 1) ? n : SignalLen - 1;
    
    for (k = kmin; k <= kmax; k++) {
      Result[n] += Signal[k] * Kernel[n - k];
    }
  }
}

void Open_PDM_Filter_Init(TPDMFilter_InitStruct *Param)
{
  uint16_t i, j;
  int64_t sum = 0;

  f_initStruct = *Param;



  for (i = 0; i < SINCN; i++) {
    f_initStruct.Coef[i] = 0;
    f_initStruct.bit[i] = 0;
  }
  for (i = 0; i < f_initStruct.Decimation; i++) {
    sinc1[i] = 1;
  }

  f_initStruct.OldOut = f_initStruct.OldIn = f_initStruct.OldZ = 0;
  f_initStruct.LP_ALFA = (f_initStruct.LP_HZ != 0 ? (uint16_t) (f_initStruct.LP_HZ * 256 / (f_initStruct.LP_HZ + f_initStruct.Fs / (2 * PI))) : 0);
  f_initStruct.HP_ALFA = (f_initStruct.HP_HZ != 0 ? (uint16_t) (f_initStruct.Fs * 256 / (2 * PI * f_initStruct.HP_HZ + f_initStruct.Fs)) : 0);

  f_initStruct.FilterLen = f_initStruct.Decimation * SINCN;       
  sinc[0] = 0;
  sinc[f_initStruct.Decimation * SINCN - 1] = 0;      
  convolve(sinc1, f_initStruct.Decimation, sinc1, f_initStruct.Decimation, sinc2);
  convolve(sinc2, f_initStruct.Decimation * 2 - 1, sinc1, f_initStruct.Decimation, &sinc[1]);     
  for(j = 0; j < SINCN; j++) {
    for (i = 0; i < f_initStruct.Decimation; i++) {
      coef[j][i] = sinc[j * f_initStruct.Decimation + i];
      sum += sinc[j * f_initStruct.Decimation + i];
    }
  }

  sub_const = sum >> 1;
  div_const = sub_const * f_initStruct.MaxVolume / 32768 / FILTER_GAIN;
  div_const = (div_const == 0 ? 1 : div_const);

#ifdef USE_LUT
  /* Look-Up Table. */
  uint16_t c, d, s;
  for (s = 0; s < SINCN; s++)
  {
    uint32_t *coef_p = &coef[s][0];
    for (c = 0; c < 256; c++)
      for (d = 0; d < f_initStruct.Decimation / 8; d++)
        lut[c][d][s] = ((c >> 7)       ) * coef_p[d * 8    ] +
                       ((c >> 6) & 0x01) * coef_p[d * 8 + 1] +
                       ((c >> 5) & 0x01) * coef_p[d * 8 + 2] +
                       ((c >> 4) & 0x01) * coef_p[d * 8 + 3] +
                       ((c >> 3) & 0x01) * coef_p[d * 8 + 4] +
                       ((c >> 2) & 0x01) * coef_p[d * 8 + 5] +
                       ((c >> 1) & 0x01) * coef_p[d * 8 + 6] +
                       ((c     ) & 0x01) * coef_p[d * 8 + 7];
  }
#endif
}

void Open_PDM_Filter_64(uint8_t* data, int16_t* dataOut, uint16_t volume, const unsigned int nSamples)
{
  uint8_t i, data_out_index;
  uint8_t channels = f_initStruct.In_MicChannels;
  uint8_t data_inc = ((DECIMATION_MAX >> 4) * channels);
  int64_t Z, Z0, Z1, Z2;
  int64_t OldOut, OldIn, OldZ;

  OldOut = f_initStruct.OldOut;
  OldIn = f_initStruct.OldIn;
  OldZ = f_initStruct.OldZ;

#ifdef USE_LUT
  uint8_t j = channels - 1;
#endif

  for (i = 0, data_out_index = 0; i < nSamples; i++, data_out_index += channels) {
#ifdef USE_LUT
    Z0 = filter_tables_64[j](data, 0);
    Z1 = filter_tables_64[j](data, 1);
    Z2 = filter_tables_64[j](data, 2);
#else
    Z0 = filter_table(data, 0);
    Z1 = filter_table(data, 1);
    Z2 = filter_table(data, 2);
#endif

    Z = f_initStruct.Coef[1] + Z2 - sub_const;
    f_initStruct.Coef[1] = f_initStruct.Coef[0] + Z1;
    f_initStruct.Coef[0] = Z0;

    OldOut = (f_initStruct.HP_ALFA * (OldOut + Z - OldIn)) >> 8;
    OldIn = Z;
    OldZ = ((256 - f_initStruct.LP_ALFA) * OldZ + f_initStruct.LP_ALFA * OldOut) >> 8;

    Z = OldZ * volume;
    Z = RoundDiv(Z, div_const);
    Z = SaturaLH(Z, -32700, 32700);

    dataOut[data_out_index] = Z;
    data += data_inc;
  }

  f_initStruct.OldOut = OldOut;
  f_initStruct.OldIn = OldIn;
  f_initStruct.OldZ = OldZ;
}
static uint8_t _countSetBits(uint8_t n)
{
    uint8_t count = 0;
    while (n) {
        count += n & 1;
        n >>= 1;
    }
    return count;
}

void Open_PDM_Filter_128(uint8_t* data, int16_t* dataOut, uint16_t volume, const unsigned int nSamples)
{
  uint8_t i, data_out_index;
  uint8_t channels = f_initStruct.In_MicChannels;
  uint8_t data_inc = ((DECIMATION_MAX >> 3) * channels);
  int64_t Z, Z0, Z1, Z2;
  int64_t OldOut, OldIn, OldZ;
#include "stdlib.h"
  static uint8_t* pSizeBitware = 0;
  if (pSizeBitware == 0)
  {
    pSizeBitware = (uint8_t*)malloc(nSamples);
  }

  OldOut = f_initStruct.OldOut;
  OldIn = f_initStruct.OldIn;
  OldZ = f_initStruct.OldZ;

#ifdef USE_LUT
  uint8_t j = channels - 1;
#endif

  for (i = 0, data_out_index = 0; i < nSamples; i++, data_out_index += channels) {
#ifdef USE_LUT
    Z0 = filter_tables_128[j](data, 0);
    Z1 = filter_tables_128[j](data, 1);
    Z2 = filter_tables_128[j](data, 2);
#else
    Z0 = filter_table(data, 0);
    Z1 = filter_table(data, 1);
    Z2 = filter_table(data, 2);
#endif

    Z = f_initStruct.Coef[1] + Z2 - sub_const;
    f_initStruct.Coef[1] = f_initStruct.Coef[0] + Z1;
    f_initStruct.Coef[0] = Z0;

    OldOut = (f_initStruct.HP_ALFA * (OldOut + Z - OldIn)) >> 8;
    OldIn = Z;
    OldZ = ((256 - f_initStruct.LP_ALFA) * OldZ + f_initStruct.LP_ALFA * OldOut) >> 8;

    Z = OldZ * volume;
    Z = RoundDiv(Z, div_const);
    Z = SaturaLH(Z, -32700, 32700);

    dataOut[data_out_index] = Z;
    pSizeBitware[i] = 0;
    for (int j = 0; j < data_inc; ++j)
    {
        pSizeBitware[i] += _countSetBits(data[j]);
    }
    data += data_inc;
  }

  f_initStruct.OldOut = OldOut;
  f_initStruct.OldIn = OldIn;
  f_initStruct.OldZ = OldZ;
}
