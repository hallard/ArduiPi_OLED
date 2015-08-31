#region ArduiPi_OLED_Wrapper, Copyright(C) 2015 Lorenzo Delana, License under MIT
/*
 * The MIT License(MIT)
 *
 * Copyright(c) 2015 Lorenzo Delana <lorenzo.delana@gmail.com>
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/
#endregion

using System;
using System.Runtime.InteropServices;

namespace ArduiPi_OLED
{

    static class NativeWrapper
    {

        [DllImport("libArduiPi_OLED.so", EntryPoint = "Wrapper_Init", SetLastError = true)]
        public static extern int Wrapper_Init(int oledType);

        [DllImport("libArduiPi_OLED.so", EntryPoint = "Wrapper_Close", SetLastError = true)]
        public static extern int Wrapper_Close();

        [DllImport("libArduiPi_OLED.so", EntryPoint = "Wrapper_DrawPixel", SetLastError = true)]
        public static extern int Wrapper_DrawPixel(short x, short y, ushort color);

        [DllImport("libArduiPi_OLED.so", EntryPoint = "Wrapper_Display", SetLastError = true)]
        public static extern void Wrapper_Display();

        [DllImport("libArduiPi_OLED.so", EntryPoint = "Wrapper_ClearDisplay", SetLastError = true)]
        public static extern void Wrapper_ClearDisplay();

        [DllImport("libArduiPi_OLED.so", EntryPoint = "Wrapper_StartScrollLeft", SetLastError = true)]
        public static extern void Wrapper_StartScrollLeft(byte start, byte stop);

        [DllImport("libArduiPi_OLED.so", EntryPoint = "Wrapper_StartScrollRight", SetLastError = true)]
        public static extern void Wrapper_StartScrollRight(byte start, byte stop);

        [DllImport("libArduiPi_OLED.so", EntryPoint = "Wrapper_StartScrollDiagLeft", SetLastError = true)]
        public static extern void Wrapper_StartScrollDiagLeft(byte start, byte stop);

        [DllImport("libArduiPi_OLED.so", EntryPoint = "Wrapper_StartScrollDiagRight", SetLastError = true)]
        public static extern void Wrapper_StartScrollDiagRight(byte start, byte stop);

        [DllImport("libArduiPi_OLED.so", EntryPoint = "Wrapper_SetHorizontalScrollProperties", SetLastError = true)]
        public static extern void Wrapper_SetHorizontalScrollProperties(bool direction, byte startRow, byte endRow, byte startColumn, byte endColumn, byte scrollSpeed);

        [DllImport("libArduiPi_OLED.so", EntryPoint = "Wrapper_StopScroll", SetLastError = true)]
        public static extern void Wrapper_StopScroll();

        [DllImport("libArduiPi_OLED.so", EntryPoint = "Wrapper_Print", SetLastError = true)]
        public static extern void Wrapper_Print(string msg);

        [DllImport("libArduiPi_OLED.so", EntryPoint = "Wrapper_DrawCircle", SetLastError = true)]
        public static extern void Wrapper_DrawCircle(short x0, short y0, short r, ushort color);

        [DllImport("libArduiPi_OLED.so", EntryPoint = "Wrapper_DrawCircleHelper", SetLastError = true)]
        public static extern void Wrapper_DrawCircleHelper(short x0, short y0, short r, byte cornername, ushort color);

        [DllImport("libArduiPi_OLED.so", EntryPoint = "Wrapper_FillCircle", SetLastError = true)]
        public static extern void Wrapper_FillCircle(short x0, short y0, short r, ushort color);

        [DllImport("libArduiPi_OLED.so", EntryPoint = "Wrapper_FillCircleHelper", SetLastError = true)]
        public static extern void Wrapper_FillCircleHelper(short x0, short y0, short r, byte cornername, ushort color);

        [DllImport("libArduiPi_OLED.so", EntryPoint = "Wrapper_DrawLine", SetLastError = true)]
        public static extern void Wrapper_DrawLine(short x0, short y0, short x1, short y1, ushort color);

        [DllImport("libArduiPi_OLED.so", EntryPoint = "Wrapper_DrawRect", SetLastError = true)]
        public static extern void Wrapper_DrawRect(short x, short y, short w, short h, ushort color);

        [DllImport("libArduiPi_OLED.so", EntryPoint = "Wrapper_DrawVerticalBargraph", SetLastError = true)]
        public static extern void Wrapper_DrawVerticalBargraph(short x, short y, short w, short h, ushort color, ushort percent);

        [DllImport("libArduiPi_OLED.so", EntryPoint = "Wrapper_DrawHorizontalBargraph", SetLastError = true)]
        public static extern void Wrapper_DrawHorizontalBargraph(short x, short y, short w, short h, ushort color, ushort percent);

        [DllImport("libArduiPi_OLED.so", EntryPoint = "Wrapper_FillScreen", SetLastError = true)]
        public static extern void Wrapper_FillScreen(ushort color);

        [DllImport("libArduiPi_OLED.so", EntryPoint = "Wrapper_DrawRoundRect", SetLastError = true)]
        public static extern void Wrapper_DrawRoundRect(short x, short y, short w, short h, short r, ushort color);

        [DllImport("libArduiPi_OLED.so", EntryPoint = "Wrapper_FillRoundRect", SetLastError = true)]
        public static extern void Wrapper_FillRoundRect(short x, short y, short w, short h, short r, ushort color);

        [DllImport("libArduiPi_OLED.so", EntryPoint = "Wrapper_DrawTriangle", SetLastError = true)]
        public static extern void Wrapper_DrawTriangle(short x0, short y0, short x1, short y1, short x2, short y2, ushort color);

        [DllImport("libArduiPi_OLED.so", EntryPoint = "Wrapper_FillTriangle", SetLastError = true)]
        public static extern void Wrapper_FillTriangle(short x0, short y0, short x1, short y1, short x2, short y2, ushort color);

        [DllImport("libArduiPi_OLED.so", EntryPoint = "Wrapper_DrawBitmap", SetLastError = true)]
        public static extern void Wrapper_DrawBitmap(short x, short y, byte[] bitmap, short w, short h, ushort color);

        [DllImport("libArduiPi_OLED.so", EntryPoint = "Wrapper_Write", SetLastError = true)]
        public static extern void Wrapper_Write(byte c);

        [DllImport("libArduiPi_OLED.so", EntryPoint = "Wrapper_DrawChar", SetLastError = true)]
        public static extern void Wrapper_Drawchar(short x, short y, byte c, ushort color, ushort bg, byte size);

        [DllImport("libArduiPi_OLED.so", EntryPoint = "Wrapper_SetCursor", SetLastError = true)]
        public static extern void Wrapper_SetCursor(short x, short y);

        [DllImport("libArduiPi_OLED.so", EntryPoint = "Wrapper_SetTextSize", SetLastError = true)]
        public static extern void Wrapper_SetTextSize(byte s);

        [DllImport("libArduiPi_OLED.so", EntryPoint = "Wrapper_SetTextColor", SetLastError = true)]
        public static extern void Wrapper_SetTextColor(ushort s);

        [DllImport("libArduiPi_OLED.so", EntryPoint = "Wrapper_SetTextWrap", SetLastError = true)]
        public static extern void Wrapper_SetTextWrap(bool w);

        [DllImport("libArduiPi_OLED.so", EntryPoint = "Wrapper_DisplayWidth", SetLastError = true)]
        public static extern short Wrapper_DisplayWidth();

        [DllImport("libArduiPi_OLED.so", EntryPoint = "Wrapper_DisplayHeight", SetLastError = true)]
        public static extern short Wrapper_DisplayHeight();

    }

    public enum OLED_Types
    {
        Adafruit_SPI_128x32 = 0,
        Adafruit_SPI_128x64 = 1,
        Adafruit_I2C_128x32 = 2,
        Adafruit_I2C_128x64 = 3,
        Seeed_I2C_128x64 = 4,
        Seeed_I2C_96x96 = 5,
        SH1106_I2C_128x64 = 6
    };

    public class Wrapper : IDisposable
    {
        bool closed = false;

        public Wrapper(OLED_Types oledType)
        {
            NativeWrapper.Wrapper_Init((int)oledType);
        }

        public void Close()
        {
            if (!closed)
            {
                NativeWrapper.Wrapper_Close();
            }
        }

        public void Display()
        {
            NativeWrapper.Wrapper_Display();
        }

        public void ClearDisplay()
        {
            NativeWrapper.Wrapper_ClearDisplay();
        }

        public void DrawPixel(short x, short y, ushort color)
        {
            NativeWrapper.Wrapper_DrawPixel(x, y, color);
        }

        public void StartScrollLeft(byte start, byte stop)
        {
            NativeWrapper.Wrapper_StartScrollLeft(start, stop);
        }

        public void StartScrollRight(byte start, byte stop)
        {
            NativeWrapper.Wrapper_StartScrollRight(start, stop);
        }

        public void StartScrollDiagLeft(byte start, byte stop)
        {
            NativeWrapper.Wrapper_StartScrollDiagLeft(start, stop);
        }

        public void StartScrollDiagRight(byte start, byte stop)
        {
            NativeWrapper.Wrapper_StartScrollDiagRight(start, stop);
        }

        public void SetHorizontalScrollProperties(bool direction, byte startRow, byte endRow, byte startColumn, byte endColumn, byte scrollSpeed)
        {
            NativeWrapper.Wrapper_SetHorizontalScrollProperties(direction, startRow, endRow, startColumn, endColumn, scrollSpeed);
        }

        public void StopScroll()
        {
            NativeWrapper.Wrapper_StopScroll();
        }

        public void Print(string msg)
        {
            NativeWrapper.Wrapper_Print(msg);
        }

        public void DrawCircle(short x0, short y0, short r, ushort color)
        {
            NativeWrapper.Wrapper_DrawCircle(x0, y0, r, color);
        }

        public void DrawCircleHelper(short x0, short y0, short r, byte cornername, ushort color)
        {
            NativeWrapper.Wrapper_DrawCircleHelper(x0, y0, r, cornername, color);
        }

        public void FillCircle(short x0, short y0, short r, ushort color)
        {
            NativeWrapper.Wrapper_FillCircle(x0, y0, r, color);
        }

        public void FillCircleHelper(short x0, short y0, short r, byte cornername, ushort color)
        {
            NativeWrapper.Wrapper_FillCircleHelper(x0, y0, r, cornername, color);
        }

        public void DrawLine(short x0, short y0, short x1, short y1, ushort color)
        {
            NativeWrapper.Wrapper_DrawLine(x0, y0, x1, y1, color);
        }

        public void DrawRect(short x, short y, short w, short h, ushort color)
        {
            NativeWrapper.Wrapper_DrawRect(x, y, w, h, color);
        }

        public void DrawVerticalBargraph(short x, short y, short w, short h, ushort color, ushort percent)
        {
            NativeWrapper.Wrapper_DrawVerticalBargraph(x, y, w, h, color, percent);
        }

        public void DrawHorizontalBargraph(short x, short y, short w, short h, ushort color, ushort percent)
        {
            NativeWrapper.Wrapper_DrawHorizontalBargraph(x, y, w, h, color, percent);
        }

        public void FillScreen(ushort color)
        {
            NativeWrapper.Wrapper_FillScreen(color);
        }

        public void DrawRoundRect(short x, short y, short w, short h, short r, ushort color)
        {
            NativeWrapper.Wrapper_DrawRoundRect(x, y, w, h, r, color);
        }

        public void FillRoundRect(short x, short y, short w, short h, short r, ushort color)
        {
            NativeWrapper.Wrapper_FillRoundRect(x, y, w, h, r, color);
        }

        public void DrawTriangle(short x0, short y0, short x1, short y1, short x2, short y2, ushort color)
        {
            NativeWrapper.Wrapper_DrawTriangle(x0, y0, x1, y1, x2, y2, color);
        }

        public void FillTriangle(short x0, short y0, short x1, short y1, short x2, short y2, ushort color)
        {
            NativeWrapper.Wrapper_FillTriangle(x0, y0, x1, y1, x2, y2, color);
        }

        public void DrawBitmap(short x, short y, byte[] bitmap, short w, short h, ushort color)
        {
            NativeWrapper.Wrapper_DrawBitmap(x, y, bitmap, w, h, color);
        }

        public void Write(byte c)
        {
            NativeWrapper.Wrapper_Write(c);
        }

        public void DrawChar(short x, short y, byte c, ushort color, ushort bg, byte size)
        {
            NativeWrapper.Wrapper_Drawchar(x, y, c, color, bg, size);
        }

        public void SetCursor(short x, short y)
        {
            NativeWrapper.Wrapper_SetCursor(x, y);
        }

        public void SetTextSize(byte s)
        {
            NativeWrapper.Wrapper_SetTextSize(s);
        }

        public void SetTextColor(byte c)
        {
            NativeWrapper.Wrapper_SetTextColor(c);
        }

        public void SetTextWrap(bool w)
        {
            NativeWrapper.Wrapper_SetTextWrap(w);
        }

        public short DisplayWidth()
        {
            return NativeWrapper.Wrapper_DisplayWidth();
        }

        public short DisplayHeight()
        {
            return NativeWrapper.Wrapper_DisplayHeight();
        }

        public void Dispose()
        {
            Close();
        }
    };

}
