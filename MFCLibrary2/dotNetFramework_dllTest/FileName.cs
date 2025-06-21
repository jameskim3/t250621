using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace dotNetFramework_dllTest
{
    public class ShadowLabel : Label
    {
        public Color ShadowColor { get; set; } = Color.Gray;
        public Point ShadowOffset { get; set; } = new Point(2, 2);

        protected override void OnPaint(PaintEventArgs e)
        {
            using (SolidBrush shadowBrush = new SolidBrush(ShadowColor))
            {
                e.Graphics.DrawString(this.Text, this.Font, shadowBrush,
                    this.ShadowOffset.X, this.ShadowOffset.Y);
            }

            using (SolidBrush mainBrush = new SolidBrush(this.ForeColor))
            {
                e.Graphics.DrawString(this.Text, this.Font, mainBrush,
                    0, 0);
            }
        }
    }
    public static class UiLogger
    {
        [DllImport("createDll.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void InitUILogger();

        [DllImport("createDll.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void LogManager_WriteLog(
            [MarshalAs(UnmanagedType.LPStr)] string screen,
            [MarshalAs(UnmanagedType.LPStr)] string button);
    }
}
