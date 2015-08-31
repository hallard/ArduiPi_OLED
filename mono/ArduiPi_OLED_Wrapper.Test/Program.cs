#region ArduiPi_OLED_Wrapper.Test, Copyright(C) 2015 Lorenzo Delana, License under MIT
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

using ArduiPi_OLED;
using Mono.Unix;
using System;
using System.Linq;
using System.Threading;

namespace ArduiPi_OLED_Wrapper.Test
{
    class Program
    {
        static void Main(string[] args)
        {
            var oledType = Util.ChooseOLEDType();

            Console.WriteLine($"Running test with selected display [{oledType}]");
            Console.WriteLine("Hit CTRL+C to stop");

            var wr = new Wrapper(oledType);

            wr.SetTextColor(1);
            wr.SetTextSize(2);

            UnixSignal sigint = new UnixSignal(Mono.Unix.Native.Signum.SIGINT);

            while (!sigint.IsSet)
            {
                wr.ClearDisplay();
                wr.SetCursor(0, 0);
                var str = $"{DateTime.Now.ToString("HH:mm.ss")}";
                Console.WriteLine(str);
                wr.Print(str);
                wr.Display();

                Thread.Sleep(1000);
            }
        }
        
    }

    
}
