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
// This define (WIMPY_MCU) precludes the use of hardware interfaces
// such as I2C & SPI
//

#include "pico/stdlib.h"
#include <stdlib.h>
#include "OneBitDisplay.h"
#include "obd_io.inl" // I/O (non-portable) code is in here
#include "obd.inl" // All of the display interface code is in here
#include "obd_gfx.inl" // drawing code
#ifdef __cplusplus
//
// C++ Class implementation
//
BBI2C * ONE_BIT_DISPLAY::getBB()
{
    return &_obd.bbi2c;
} /* getBB() */

void ONE_BIT_DISPLAY::setBB(BBI2C *pBB)
{
   memcpy(&_obd.bbi2c, pBB, sizeof(BBI2C));
} /* setBB() */

void ONE_BIT_DISPLAY::SPIbegin(int iType, int32_t iSpeed)
{
    obdSPIInit(&_obd, iType, _obd.iDCPin, _obd.iCSPin, _obd.iRSTPin, _obd.iMOSIPin, _obd.iCLKPin, _obd.iLEDPin, _obd.flip, _obd.invert, _obd.bBitBang, iSpeed);
} /* SPIbegin() */

void ONE_BIT_DISPLAY::setSPIPins(int iCS, int iMOSI, int iSCLK, int iDC, int iReset, int iLED)
{
    _obd.iCSPin = iCS;
    _obd.iMOSIPin = iMOSI;
    _obd.iCLKPin = iSCLK;
    _obd.iDCPin = iDC;
    _obd.iRSTPin = iReset;
    _obd.iLEDPin = iLED;
} /* setSPIPins() */

void ONE_BIT_DISPLAY::setI2CPins(int iSDA, int iSCL, int iReset)
{
    _obd.iSDAPin = iSDA;
    _obd.iSCLPin = iSCL;
    _obd.iRSTPin = iReset;
}
void ONE_BIT_DISPLAY::setBitBang(bool bBitBang)
{
    _obd.bBitBang = bBitBang;
}
void ONE_BIT_DISPLAY::setPower(bool bOn)
{
    obdPower(&_obd, bOn);
} /* setPower() */

void ONE_BIT_DISPLAY::setFlags(int iFlags)
{
    _obd.iFlags = iFlags;
    _obd.invert = iFlags & OBD_INVERTED;
    _obd.flip = iFlags & OBD_FLIP180;
}

void ONE_BIT_DISPLAY::setContrast(uint8_t ucContrast)
{
  obdSetContrast(&_obd, ucContrast);
}

int ONE_BIT_DISPLAY::I2Cbegin(int iType, int iAddr, int32_t iSpeed)
{
  return obdI2CInit(&_obd, iType, iAddr, _obd.flip, _obd.invert, !_obd.bBitBang, _obd.iSDAPin, _obd.iSCLPin, _obd.iRSTPin, iSpeed);
} /* I2Cbegin() */

void ONE_BIT_DISPLAY::setBuffer(uint8_t *pBuffer)
{
    _obd.ucScreen = pBuffer;
}

int ONE_BIT_DISPLAY::allocBuffer(void)
{
    int iSize = _obd.width * ((_obd.height+7)>>3);
    _obd.ucScreen = (uint8_t *)malloc(iSize);
    if (_obd.iFlags & OBD_3COLOR)
        iSize *= 2; // 2 bit planes
    if (_obd.ucScreen != NULL) {
        _obd.render = false; // draw into RAM only
        return OBD_SUCCESS;
    }
    return OBD_ERROR_NO_MEMORY; // failed
} /* allocBuffer() */

void * ONE_BIT_DISPLAY::getBuffer(void)
{
    return (void *)_obd.ucScreen;
} /* getBuffer() */

void ONE_BIT_DISPLAY::freeBuffer(void)
{
    if (_obd.ucScreen) {
        free(_obd.ucScreen);
        _obd.ucScreen = NULL;
    }
} /* freeBuffer() */

void ONE_BIT_DISPLAY::setScroll(bool bScroll)
{
    _obd.bScroll = bScroll;
}

void ONE_BIT_DISPLAY::setRotation(int iRotation)
{
    obdSetRotation(&_obd, iRotation);
} /* setRotation() */

void ONE_BIT_DISPLAY::fillScreen(int iColor)
{
  obdFill(&_obd, iColor, (_obd.ucScreen == NULL));
} /* fillScreen() */

void ONE_BIT_DISPLAY::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    obdRectangle(&_obd, x, y, x+w-1, y+h-1, color, 0);
} /* drawRect() */

void ONE_BIT_DISPLAY::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    obdRectangle(&_obd, x, y, x+w-1, y+h-1, color, 1);
} /* fillRect() */

void ONE_BIT_DISPLAY::setTextWrap(bool bWrap)
{
  _obd.wrap = (int)bWrap;
}

void ONE_BIT_DISPLAY::setTextColor(int iFG, int iBG)
{
    if (iFG > OBD_RED) iFG = OBD_BLACK;
    if (iBG > OBD_RED) iBG = OBD_BLACK;
    if ((_obd.iFlags & OBD_3COLOR) == 0) {
        if (iFG == OBD_RED) iFG = OBD_BLACK; // can't set red color
        if (iBG == OBD_RED) iBG = OBD_BLACK;
    }
    _obd.iFG = iFG;
    _obd.iBG = (iBG == -1) ? iFG : iBG;
} /* setTextColor() */

void ONE_BIT_DISPLAY::setCursor(int x, int y)
{
    if (x >= 0)
       _obd.iCursorX = x;
    if (y >= 0)
    _obd.iCursorY = y;
} /* setCursor() */

int ONE_BIT_DISPLAY::loadBMP(uint8_t *pBMP, int x, int y, int iFG, int iBG)
{
    return obdLoadBMP(&_obd, pBMP, x, y, iFG, iBG);
} /* loadBMP() */

void ONE_BIT_DISPLAY::setFont(int iFont)
{
    _obd.iFont = iFont;
    _obd.pFreeFont = NULL;
} /* setFont() */

void ONE_BIT_DISPLAY::setTextSize(uint8_t s)
{
    _obd.u32FontScaleX = _obd.u32FontScaleY = 256 * s;
    _obd.iFont = -1;
    _obd.pFreeFont = NULL;
}
void ONE_BIT_DISPLAY::setTextSize(uint8_t sx, uint8_t sy)
{
    _obd.u32FontScaleX = 256 * sx;
    _obd.u32FontScaleY = 256 * sy;
    _obd.iFont = -1;
    _obd.pFreeFont = NULL;
}
int ONE_BIT_DISPLAY::scrollBuffer(int iStartCol, int iEndCol, int iStartRow, int iEndRow, int bUp)
{
    return obdScrollBuffer(&_obd, iStartCol, iEndCol, iStartRow, iEndRow, bUp);
} /* scrollBuffer() */

void ONE_BIT_DISPLAY::setFreeFont(const GFXfont *pFont)
{
    _obd.pFreeFont = (GFXfont *)pFont;
} /* setFreeFont() */

void ONE_BIT_DISPLAY::drawLine(int x1, int y1, int x2, int y2, int iColor)
{
    obdDrawLine(&_obd, x1, y1, x2, y2, iColor, 1);
} /* drawLine() */

inline GFXglyph *pgm_read_glyph_ptr(const GFXfont *gfxFont, uint8_t c) {
#ifdef __AVR__
  return &(((GFXglyph *)pgm_read_ptr(&gfxFont->glyph))[c]);
#else
  // expression in __AVR__ section may generate "dereferencing type-punned
  // pointer will break strict-aliasing rules" warning In fact, on other
  // platforms (such as STM32) there is no need to do this pointer magic as
  // program memory may be read in a usual way So expression may be simplified
  return gfxFont->glyph + c;
#endif //__AVR__
}

void obdScroll1Line(OBDISP *pOBD, int iAmount)
{
int y, iLines;

    if (pOBD == NULL || pOBD->ucScreen == NULL)
        return;
    iLines = (pOBD->height+7)>>3;
    for (y=0; y<iLines-iAmount; y+=iAmount) {
        memcpy(&pOBD->ucScreen[y*pOBD->width], &pOBD->ucScreen[(y+iAmount)*pOBD->width], pOBD->width*iAmount);
    }
    memset(&pOBD->ucScreen[(iLines-iAmount) * pOBD->width], (char)pOBD->iBG, pOBD->width*iAmount);
} /* obdScroll1Line() */

void ONE_BIT_DISPLAY::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    obdSetPixel(&_obd, x, y, color, _obd.render);
}
int16_t ONE_BIT_DISPLAY::getCursorX(void)
{
  return _obd.iCursorX;
}
int16_t ONE_BIT_DISPLAY::getCursorY(void)
{
  return _obd.iCursorY;
}
uint8_t ONE_BIT_DISPLAY::getRotation(void)
{
  return _obd.iOrientation;
}

void ONE_BIT_DISPLAY::getTextBounds(const char *string, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h)
{
    if (_obd.pFreeFont) {
        int width, top, bottom;
        obdGetStringBox(_obd.pFreeFont, (char *)string, &width, &top, &bottom);
        *x1 = x;
        *w = width;
        *y1 = y + top;
        *h = (bottom - top + 1);
    }
}
int16_t ONE_BIT_DISPLAY::width(void)
{
   return _obd.width;
}
int16_t ONE_BIT_DISPLAY::height(void)
{
   return _obd.height;
}
void ONE_BIT_DISPLAY::drawSprite(uint8_t *pSprite, int cx, int cy, int iPitch, int x, int y, uint8_t iPriority)
{
    obdDrawSprite(&_obd, pSprite, cx, cy, iPitch, x, y, iPriority);
}
int ONE_BIT_DISPLAY::drawGFX(uint8_t *pSrc, int iSrcCol, int iSrcRow, int iDestCol, int iDestRow, int iWidth, int iHeight, int iSrcPitch)
{
    return obdDrawGFX(&_obd, pSrc, iSrcCol, iSrcRow, iDestCol, iDestRow, iWidth, iHeight, iSrcPitch);
} /* drawGFX() */

void ONE_BIT_DISPLAY::drawTile(const uint8_t *pTile, int x, int y, int iRotation, int bInvert, int bRender)
{
    obdDrawTile(&_obd, pTile, x, y, iRotation, bInvert, bRender);
}
void ONE_BIT_DISPLAY::drawCircle(int32_t x, int32_t y, int32_t r, uint32_t color)
{
  obdEllipse(&_obd, x, y, r, r, color, 0);
}
void ONE_BIT_DISPLAY::fillCircle(int32_t x, int32_t y, int32_t r, uint32_t color)
{
    obdEllipse(&_obd, x, y, r, r, color, 1);
}
void ONE_BIT_DISPLAY::drawEllipse(int16_t x, int16_t y, int32_t rx, int32_t ry, uint16_t color)
{
    obdEllipse(&_obd, x, y, rx, ry, color, 0);
}
void ONE_BIT_DISPLAY::fillEllipse(int16_t x, int16_t y, int32_t rx, int32_t ry, uint16_t color)
{
    obdEllipse(&_obd, x, y, rx, ry, color, 1);
}
void ONE_BIT_DISPLAY::setPosition(int x, int y, int w, int h)
{
    obdSetPosition(&_obd, x, y, 1);
} /* setPosition() */
void ONE_BIT_DISPLAY::pushPixels(uint8_t *pPixels, int iCount)
{
    RawWriteData(&_obd, pPixels, iCount);
} /* pushPixels() */

void ONE_BIT_DISPLAY::pushImage(int x, int y, int w, int h, uint16_t *pixels)
{
    // DEBUG
    (void)x; (void)y; (void)w; (void)h; (void)pixels;
}

void ONE_BIT_DISPLAY::drawString(const char *pText, int x, int y)
{
    setCursor(x,y);
    if (_obd.pFreeFont != NULL)
        obdWriteStringCustom(&_obd, _obd.pFreeFont, x, y, (char *)pText, _obd.iFG);
    else
        obdWriteString(&_obd, 0, x, y, (char *)pText, _obd.iFont, _obd.iFG, 1);
} /* drawString() */

int ONE_BIT_DISPLAY::display(bool bRefresh)
{
    return obdDumpBuffer_2(&_obd, NULL, bRefresh);
}
void ONE_BIT_DISPLAY::setRender(bool bRAMOnly)
{
    _obd.render = !bRAMOnly;
}
#endif // __cplusplus
