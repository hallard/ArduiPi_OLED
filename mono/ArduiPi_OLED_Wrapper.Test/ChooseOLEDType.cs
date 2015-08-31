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
using System.IO;
using System.Linq;
using System.Text;

namespace ArduiPi_OLED_Wrapper.Test
{

    public static class Util
    {

        public static string ChooseNetDev()
        {
            var q = File.ReadLines("/proc/net/dev").Skip(2);

            var devs = q.Select(w => w.Trim().Split(' ')[0].TrimEnd(':')).ToList();

            string dev = "";
            while (dev.Length == 0)
            {
                Console.WriteLine("Choose net device to monitor:");
                for (int i = 0; i < devs.Count; ++i)
                {
                    Console.WriteLine($"{i}\t{devs[i]}");
                }

                var k = Console.ReadKey();

                if (char.IsDigit(k.KeyChar))
                {
                    var choose = int.Parse($"{k.KeyChar}");                    

                    if (choose >= 0 && choose < devs.Count) { dev = devs[choose]; }
                }
            }

            return dev;
        }

        public static OLED_Types ChooseOLEDType(bool onlyShowTypes = false)
        {
            var types = Enum.GetValues(typeof(OLED_Types)).ToList<OLED_Types>();
            OLED_Types? type = null;

            var sbPrompt = new StringBuilder();
            sbPrompt.AppendLine("Available display devices drivers:");
            foreach (var x in types)
            {
                sbPrompt.AppendLine($"\t{(int)x}\t{x}");
            }
            if (onlyShowTypes)
            {
                Console.WriteLine(sbPrompt.ToString());
                return OLED_Types.Adafruit_I2C_128x32;
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
