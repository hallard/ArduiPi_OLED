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
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace ArduiPi_OLED_Wrapper.Test
{
    class Program
    {
        static void Main(string[] args)
        {

            var oledType = ChooseOLEDType();

            Console.WriteLine($"Running test with selected display [{oledType}]");
            Console.WriteLine("Hit CTRL+C to stop");

            var wr = new Wrapper(oledType);

            wr.SetTextColor(1);
            wr.SetTextSize(2);

            while (true)
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

        static OLED_Types ChooseOLEDType()
        {
            var types = Enum.GetValues(typeof(OLED_Types)).ToList<OLED_Types>();
            OLED_Types? type = null;

            var sbPrompt = new StringBuilder();
            sbPrompt.AppendLine("Available display devices drivers:");
            foreach (var x in types)
            {
                sbPrompt.AppendLine($"\t{(int)x}\t{x}");
            }
            sbPrompt.Append("Choose display type :");

            var charToChoice = types.ToDictionary(k => ((int)k).ToString()[0], v => v);

            while (!type.HasValue)
            {
                Console.Write(sbPrompt.ToString());

                var k = Console.ReadKey();

                if (charToChoice.ContainsKey(k.KeyChar))
                {
                    type = charToChoice[k.KeyChar];

                    break;
                }
                else
                    Console.WriteLine($"Invalid choice [{k.KeyChar}]");
            }

            return type.Value;
        }
    }

    public static class Extensions
    {

        public static List<T> ToList<T>(this Array ary)
        {
            var res = new List<T>();

            foreach (var x in ary) res.Add((T)x);

            return res;
        }

    }
}
