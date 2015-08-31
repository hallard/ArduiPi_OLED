/*********************************************************************
This is a library for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

These displays use SPI to communicate, 4 or 5 pins are required to
interface

Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.
BSD license, check license.txt for more information
All text above, and the splash screen below must be included in any redistribution

02/18/2013  Charles-Henri Hallard (http://hallard.me)
			Modified for compiling and use on Raspberry ArduiPi Board
			LCD size and connection are now passed as arguments on
			the command line (no more #define on compilation needed)
			ArduiPi project documentation http://hallard.me/arduipi
07/01/2013  Charles-Henri Hallard
			Reduced code size removed the Adafruit Logo (sorry guys)
			Buffer for OLED is now dynamic to LCD size
			Added support of Seeed OLED 64x64 Display

07/26/2013  Charles-Henri Hallard
			modified name for generic library using different OLED type

02/24/2015  Charles-Henri Hallard
			added support for 1.3" I2C OLED with SH1106 driver

08/26/2015	Lorenzo Delana <lorenzo.delana@gmail.com>
			Wrappers for functions export.

*********************************************************************/

#include "./ArduiPi_OLED_lib.h" 
#include "./Adafruit_GFX.h"
#include "./ArduiPi_OLED.h"

ArduiPi_OLED *extDisplay = NULL;

// Initialize the wrapper ( allocate the ArduiPi_OLED object and init display hardware type ).
extern "C" void Wrapper_Init(int oledType)
{
	if (extDisplay != NULL) return;

	if (oledType < 0 || oledType >= OLED_LAST_OLED) oledType = 0;

	extDisplay = new ArduiPi_OLED();

	// SPI
	if (extDisplay->oled_is_spi_proto(oledType))
	{
		// SPI change parameters to fit to your LCD
		if (!extDisplay->init(OLED_SPI_DC, OLED_SPI_RESET, OLED_SPI_CS, oledType))
			exit(EXIT_FAILURE);
	}
	else
	{
		// I2C change parameters to fit to your LCD
		if (!extDisplay->init(OLED_I2C_RESET, oledType))
			exit(EXIT_FAILURE);
	}

	extDisplay->begin();

	// init done
	extDisplay->clearDisplay();   // clears the screen  buffer
	extDisplay->display();   		// display it (clear display)	
}

extern "C" void Wrapper_Close()
{
	extDisplay->close();
}

extern "C" void Wrapper_Display()
{
	extDisplay->display();
}

extern "C" void Wrapper_ClearDisplay()
{
	extDisplay->clearDisplay();
}

extern "C" void Wrapper_DrawPixel(int16_t x, int16_t y, uint16_t color)
{
	extDisplay->drawPixel(x, y, color);
}

extern "C" void Wrapper_StartScrollLeft(uint8_t start, uint8_t stop)
{
	extDisplay->startscrollleft(start, stop);
}

extern "C" void Wrapper_StartScrollRight(uint8_t start, uint8_t stop)
{
	extDisplay->startscrollright(start, stop);
}

extern "C" void Wrapper_StartScrollDiagLeft(uint8_t start, uint8_t stop)
{
	extDisplay->startscrolldiagleft(start, stop);
}

extern "C" void Wrapper_StartScrollDiagRight(uint8_t start, uint8_t stop)
{
	extDisplay->startscrolldiagright(start, stop);
}

extern "C" void Wrapper_SetHorizontalScrollProperties(bool direction, uint8_t startRow, uint8_t endRow, uint8_t startColumn, uint8_t endColumn, uint8_t scrollSpeed)
{
	extDisplay->setHorizontalScrollProperties(direction, startRow, endRow, startColumn, endColumn, scrollSpeed);
}

extern "C" void Wrapper_StopScroll()
{
	extDisplay->stopscroll();
}

extern "C" void Wrapper_Print(const char * string)
{
	extDisplay->print(string);
}

extern "C" void Wrapper_DrawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
	extDisplay->drawCircle(x0, y0, r, color);
}

extern "C" void Wrapper_DrawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color)
{
	extDisplay->drawCircleHelper(x0, y0, r, cornername, color);
}

extern "C" void Wrapper_FillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
	extDisplay->fillCircle(x0, y0, r, color);
}

extern "C" void Wrapper_FillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color)
{
	extDisplay->fillCircleHelper(x0, y0, r, cornername, delta, color);
}

extern "C" void Wrapper_DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
	extDisplay->drawLine(x0, y0, x1, y1, color);
}

extern "C" void Wrapper_DrawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
	extDisplay->drawRect(x, y, w, h, color);
}

extern "C" void Wrapper_DrawVerticalBargraph(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color, uint16_t percent)
{
	extDisplay->drawVerticalBargraph(x, y, w, h, color, percent);
}

extern "C" void Wrapper_DrawHorizontalBargraph(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color, uint16_t percent)
{
	extDisplay->drawHorizontalBargraph(x, y, w, h, color, percent);
}

extern "C" void Wrapper_FillScreen(uint16_t color)
{
	extDisplay->fillScreen(color);
}

extern "C" void Wrapper_DrawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
	extDisplay->drawRoundRect(x, y, w, h, r, color);
}

extern "C" void Wrapper_FillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
	extDisplay->fillRoundRect(x, y, w, h, r, color);
}

extern "C" void Wrapper_DrawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
	extDisplay->drawTriangle(x0, y0, x1, y1, x2, y2, color);
}

extern "C" void Wrapper_FillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
	extDisplay->fillTriangle(x0, y0, x1, y1, x2, y2, color);
}

extern "C" void Wrapper_DrawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color)
{
	extDisplay->drawBitmap(x, y, bitmap, w, h, color);
}

extern "C" void Wrapper_Write(byte c)
{
	extDisplay->write(c);
}

extern "C" void Wrapper_DrawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size)
{
	extDisplay->drawChar(x, y, c, color, bg, size);
}

extern "C" void Wrapper_SetCursor(int16_t x, int16_t y)
{
	extDisplay->setCursor(x, y);
}

extern "C" void Wrapper_SetTextSize(uint8_t s)
{
	extDisplay->setTextSize(s);
}

extern "C" void Wrapper_SetTextColor(uint16_t c)
{
	extDisplay->setTextColor(c);
}

extern "C" void Wrapper_SetTextWrap(boolean w)
{
	extDisplay->setTextWrap(w);
}

extern "C" int16_t Wrapper_DisplayWidth()
{
	extDisplay->width();
}

extern "C" int16_t Wrapper_DisplayHeight()
{
	extDisplay->height();
}
