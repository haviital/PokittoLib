using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ConsoleApplication1
{
    class Program
    {
        static void Main(string[] args)
        {
            List<Color> set = new List<Color>();
            set.Add(Color.FromArgb(255,0,255));
            set.Add(Color.FromArgb(0,0,0));
            set.Add(Color.FromArgb(94,94,94));
            set.Add(Color.FromArgb(56,104,144));
            set.Add(Color.FromArgb(40,60,88));
            set.Add(Color.FromArgb(50,50,50));
            set.Add(Color.FromArgb(70,70,70));
            set.Add(Color.FromArgb(90,90,90));

            foreach (var file in args)
                using (var bitmap = new Bitmap(file))
                    for (int y = 0; y < bitmap.Height; ++y)
                        for (int x = 0; x < bitmap.Width; ++x)
                        {
                            var colour = bitmap.GetPixel(x, y);
                            if (!set.Contains(colour) && colour.A == byte.MaxValue)
                                set.Add(colour);
                        }

            if (set.Count > 0)
                using (var result = new Bitmap(set.Count, 1))
                {
                    foreach (var pair in set.Zip(Enumerable.Range(0, set.Count), Tuple.Create))
                        result.SetPixel(pair.Item2, 0, pair.Item1);

                    result.Save("palette.png");
                }            
        }
    }
}
