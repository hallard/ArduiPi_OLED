#region BananaPiOLEDMonitor, Copyright(C) 2015 Lorenzo Delana, License under MIT
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
using System.Linq;
using System.Threading;
using System.IO;
using Lib0.Graph;
using ArduiPi_OLED_Wrapper.Test;

namespace BananaPiOLEDMonitor
{
    class Program
    {
        static void Main(string[] args)
        {
            var oledType = Util.ChooseOLEDType();
            var netDev = Util.ChooseNetDev();

            Console.WriteLine($"Running test with selected display [{oledType}]");
            Console.WriteLine("Hit CTRL+C to stop");

            // init oled display
            var wrapper = new ArduiPi_OLED.Wrapper(oledType);
            wrapper.SetTextColor(1);
            wrapper.SetTextSize(2);

            var w = wrapper.DisplayWidth();
            var h = wrapper.DisplayHeight();

            //--- here can change to 24h for example
            var longTxt = "15m";
            var longTimespan = TimeSpan.FromMinutes(15);
            //---

            Console.WriteLine($"display w:{w} x h:{h}");

            // -20 to skip 16 of first char + 4 margin
            var graphW = (short)(w - 20);
            var graphH = (short)12; // let 4 pixel margin

            IMonitorPlot plotNet = new MonitorPlot(graphW);
            var dsDefaultNet = plotNet.DataSets.First(k => k.Name == "default");
            var dsLongNet = plotNet.AddDataSet(longTxt, longTimespan);

            IMonitorPlot plotCpu = new MonitorPlot(graphW);
            var dsDefaultCpu = plotCpu.DataSets.First(k => k.Name == "default");
            var dsLongCpu = plotCpu.AddDataSet(longTxt, longTimespan);

            {
                var cpuPrev = new CpuNfo(); cpuPrev.ReadFromProc();
                var netPrev = new NetNfo(netDev); netPrev.ReadFromProc();

                while (true)
                {
                    wrapper.ClearDisplay();

                    wrapper.SetTextSize(2);
                    wrapper.SetCursor(0, 0);
                    wrapper.Print("C");

                    wrapper.SetTextSize(1);
                    wrapper.SetCursor(0, 16);
                    wrapper.Print(longTxt);

                    wrapper.SetTextSize(2);
                    wrapper.SetCursor(0, 32);
                    wrapper.Print("N");

                    wrapper.SetTextSize(1);
                    wrapper.SetCursor(0, 48);
                    wrapper.Print(longTxt);

                    // CPU
                    {
                        var nfoCur = new CpuNfo();
                        nfoCur.ReadFromProc(); // read updated info
                        var diff = nfoCur - cpuPrev;
                        var loadF = (double)diff.Usage / diff.Total;
                        var load = (int)(loadF * 100);

                        Console.Write($"cpu % = {load}");

                        plotCpu.Add(new MonitorData(DateTime.Now, loadF));

                        cpuPrev = nfoCur;

                        // DisplayMonitorPlot (wrapper, ds, graphX0, graphW, graphY0, graphH, normalize)
                        DisplayMonitorPlot(wrapper, dsDefaultCpu, 20, graphW, 15, graphH, false);
                        DisplayMonitorPlot(wrapper, dsLongCpu, 20, graphW, 31, graphH, false);
                    }

                    // NET
                    {
                        var nfoCur = new NetNfo(netDev);
                        nfoCur.ReadFromProc(); // read updated info
                        var diff = nfoCur - netPrev;

                        var rate = Math.Max(diff.Received, diff.Transmitted);
                        Console.WriteLine($" ; net rate = {rate}");

                        plotNet.Add(new MonitorData(DateTime.Now, rate));

                        netPrev = nfoCur;

                        DisplayMonitorPlot(wrapper, dsDefaultNet, 20, graphW, 47, graphH, true);
                        DisplayMonitorPlot(wrapper, dsLongNet, 20, graphW, 63, graphH, true);
                    }

                    wrapper.Display();

                    Thread.Sleep(1000);
                }

            }

        }

        static void DisplayMonitorPlot(ArduiPi_OLED.Wrapper wrapper, IMonitorDataSet ds,
            short graphX0, short graphW, short graphY0, short graphH, bool normalize)
        {
            var pts = ds.Points.ToList();

            if (ds.SizeMax != graphW) throw new Exception($"graph width must match dataset points max {graphW} != {ds.SizeMax}");

            // baseline track
            
            for (int i = graphX0; i < graphX0 + graphW; i += 1)
            {
                wrapper.DrawPixel((short)i, (short)(graphY0), 1);
            }

            var max = 1.0;
            if (normalize)
            {
                foreach (var k in pts) max = Math.Max(max, k.Value);
            }

            for (int i = 0; i < pts.Count; ++i)
            {
                var pt = pts[i];
                var f = pt.Value;
                if (normalize) f = f / max;
                var x1 = (short)(graphX0 + i);
                var y1 = (short)(graphY0 - (f * graphH));

                wrapper.DrawPixel(x1, y1, 1);

                if (i > 0) // glue (x1,y1) with previous point (x0,y0)
                {
                    var ptPrev = pts[i - 1];
                    var fPrev = ptPrev.Value;
                    if (normalize) fPrev = fPrev / max;
                    var x0 = (short)(graphX0 + i - 1);
                    var y0 = (short)(graphY0 - (fPrev * graphH));

                    wrapper.DrawLine(x0, y0, x1, y1, 1);
                }
            }
        }

    }

    public class NetNfo
    {
        public string Dev { get; private set; }

        /// <summary>
        /// Received (bytes)
        /// </summary>
        public ulong Received { get; private set; }

        /// <summary>
        /// Transmitted (bytes)
        /// </summary>
        public ulong Transmitted { get; private set; }

        public static NetNfo operator -(NetNfo a, NetNfo b)
        {
            return new NetNfo(a.Dev)
            {
                Received = a.Received - b.Received,
                Transmitted = a.Transmitted - b.Transmitted
            };
        }

        public NetNfo(string dev)
        {
            Dev = dev;
        }

        public bool ReadFromProc()
        {
            var q = File.ReadLines("/proc/net/dev");

            foreach (var x in q)
            {
                if (x.Trim().StartsWith(Dev + ":"))
                {
                    var xq = x.Split(' ').Where(r => r.Trim().Length > 0).ToList();

                    Received = ulong.Parse(xq[1]);
                    Transmitted = ulong.Parse(xq[9]);

                    return true;
                }
            }

            return false;
        }
    }

    public class CpuNfo
    {
        public int User { get; private set; }
        public int Nice { get; private set; }
        public int System { get; private set; }
        public int Idle { get; private set; }

        public int Usage { get { return User + Nice + System; } }
        public int Total { get { return User + Nice + System + Idle; } }

        public static CpuNfo operator -(CpuNfo a, CpuNfo b)
        {
            return new CpuNfo()
            {
                User = a.User - b.User,
                Nice = a.Nice - b.Nice,
                System = a.System - b.System,
                Idle = a.Idle - b.Idle
            };
        }

        public bool ReadFromProc()
        {
            var q = File.ReadLines("/proc/stat");

            foreach (var x in q)
            {
                if (x.StartsWith("cpu"))
                {
                    var xq = x.Split(' ').Where(r => r.Trim().Length > 0).ToList();

                    // gather only summary
                    if (xq[0] == "cpu")
                    {
                        User = int.Parse(xq[1]);
                        Nice = int.Parse(xq[2]);
                        System = int.Parse(xq[3]);
                        Idle = int.Parse(xq[4]);
                    }

                    return true;
                }
            }

            return false;
        }
    }
}
