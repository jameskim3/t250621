using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace vppEncrypt
{
    public partial class Form1 : Form
    {
        [DllImport("createDll.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr CreateMyMfcClass();
        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            HiddenSignatureInserter tt = new HiddenSignatureInserter();
            tt.AddHiddenSignatureInput(@"C:\tb\1.vpp");


        }

        private void button2_Click(object sender, EventArgs e)
        {
            // CMyMfcClass 인스턴스 생성
            IntPtr myClass = CreateMyMfcClass();

            HiddenSignatureInserter tt = new HiddenSignatureInserter();
            var ret = tt.verify(@"C:\tb\1.vpp");
            MessageBox.Show($"{ret}");

        }
    }
}
