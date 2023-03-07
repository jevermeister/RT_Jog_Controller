//
// OneBitDisplay (OLED/LCD/E-Paper library)
// Copyright (c) 2020 BitBank Software, Inc.
// Written by Larry Bank (bitbank@pobox.com)
// Project started 3/23/2020
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//    http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// I/O functions for the OneBitDisplay library
//
// This define (WIMPY_MCU) precludes the use of hardware interfaces
// such as I2C & SPI
//

#define memcpy_P memcpy

static uint8_t pgm_read_byte(const uint8_t *ptr)
{
  return *ptr;
}
static int16_t pgm_read_word(const uint8_t *ptr)
{
  return ptr[0] + (ptr[1]<<8);
}

#define MAX_CACHE 192
static uint8_t u8Cache[MAX_CACHE]; // for faster character drawing
static volatile uint8_t u8End = 0;
static uint8_t u8Temp[40]; // for stretched character drawing

static void RawWrite(OBDISP *pOBD, unsigned char *pData, int iLen)
{
  {
    if (pOBD->bbi2c.bWire && iLen > 32) // Hardware I2C has write length limits
    {
       iLen--; // don't count the 0x40 byte the first time through
       while (iLen >= 31) // max 31 data byes + data introducer
       {
          I2CWrite(&pOBD->bbi2c, pOBD->oled_addr, pData, 32);
          iLen -= 31;
          pData += 31;
          pData[0] = 0x40;
       }
       if (iLen) iLen++; // If remaining data, add the last 0x40 byte
    }
    if (iLen) // if any data remaining
    {
      I2CWrite(&pOBD->bbi2c, pOBD->oled_addr, pData, iLen);
    }
  } // I2C
} /* RawWrite() */
static void RawWriteData(OBDISP *pOBD, unsigned char *pData, int iLen)
{
  {
    u8Temp[0] = 0x40; // data prefix byte
    while (iLen >= 31) { // max 31 data byes + data introducer
        memcpy(&u8Temp[1], pData, 31);
        I2CWrite(&pOBD->bbi2c, pOBD->oled_addr, u8Temp, 32);
        iLen -= 31;
        pData += 31;
    } // while >= 31 bytes to send
    if (iLen) // if any data remaining
    {
        memcpy(&u8Temp[1], pData, iLen);
        I2CWrite(&pOBD->bbi2c, pOBD->oled_addr, u8Temp, iLen+1);
    }
  } // I2C
} /* RawWriteData() */