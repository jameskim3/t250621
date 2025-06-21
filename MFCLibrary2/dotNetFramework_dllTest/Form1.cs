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
using static dotNetFramework_dllTest.Form1;

namespace dotNetFramework_dllTest
{
    public partial class Form1 : Form
    {
        [DllImport("createDll.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr CreateMyMfcClass();

        [DllImport("createDll.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void DeleteMyMfcClass(IntPtr pInstance);

        [DllImport("createDll.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void SetMyMfcClassData(IntPtr pInstance, double value);

        [DllImport("createDll.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern double GetMyMfcClassData(IntPtr pInstance);

        [DllImport("createDll.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern double SquareMyMfcClassData(IntPtr pInstance);
        
        [DllImport("kernel32.dll")]
        public static extern bool AllocConsole();

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        public delegate void UiLogDelegate([MarshalAs(UnmanagedType.LPStr)] string message);

        [DllImport("createDll.dll")]
        public static extern void RegisterUiLogger(UiLogDelegate cb);

        [DllImport("createDll.dll")]
        public static extern void ReportNanovisionTime();
        UiLogDelegate _logDelegate;
        public Form1()
        {
            InitializeComponent();
            init();
        }
        private bool highlight = false;
        private Timer blinkTimer;
        ShadowLabel shadowLabel;
        public void init()
        {
            UiLogger.InitUILogger(); // 한 번만 호출
            AllocConsole();

            // CMyMfcClass 인스턴스 생성
            IntPtr myClass = CreateMyMfcClass();

            // 데이터 설정
            SetMyMfcClassData(myClass, 155.5);

            // 데이터 가져오기
            double value = GetMyMfcClassData(myClass);
            Console.WriteLine($"Value: {value}");

            label1.Text =$"Value: {value}";

            // 데이터의 제곱 계산
            double square = SquareMyMfcClassData(myClass);
            Console.WriteLine($"Square: {square}");
            label2.Text = $"Square: {square}";
            // 인스턴스 삭제
            DeleteMyMfcClass(myClass);
            int formW = this.ClientSize.Width;
            int formH = this.ClientSize.Height;

            // 위치: 좌상단 기준 5% 오프셋
            int x = (int)(formW * 0.85);
            int y = (int)(formH * 0.05);

            // 크기: 너비 30%, 높이 10%
            int w = (int)(formW * 0.3);
            int h = (int)(formH * 0.1);
            shadowLabel = new ShadowLabel();
            shadowLabel.Text = "Nanovision - [05/10]";
            shadowLabel.Font = new Font("Segoe UI", 12, FontStyle.Bold);
            shadowLabel.ForeColor = Color.White;
            shadowLabel.ShadowColor = Color.Black;
            shadowLabel.BackColor = Color.FromArgb(30, 30, 80);
            shadowLabel.Size = new Size(250, 30);
            shadowLabel.Location = new Point(x, y);
            shadowLabel.TextAlign = ContentAlignment.MiddleCenter;
            this.Controls.Add(shadowLabel);
            SetupBlinking();


            _logDelegate = new UiLogDelegate(msg =>
            {
                if (InvokeRequired)
                    BeginInvoke((MethodInvoker)(() => shadowLabel.Text = msg));
                else
                    shadowLabel.Text = msg;
            });

            RegisterUiLogger(_logDelegate);

            // 1초마다 시간 갱신
            timer1.Interval = 1000;
            timer1.Tick += (s, ev) => ReportNanovisionTime();
            timer1.Start();
        }
        private void SetupBlinking()
        {
            blinkTimer = new Timer();
            blinkTimer.Interval = 500; // 0.5초 간격
            blinkTimer.Tick += (s, e) =>
            {
                if (highlight)
                    shadowLabel.ForeColor = Color.White;
                else
                    shadowLabel.ForeColor = Color.Gold;

                highlight = !highlight;
                shadowLabel.Invalidate(); // 다시 그리기
            };
            blinkTimer.Start();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            UiLogger.LogManager_WriteLog("MainScreen", "StartButton");
        }
    }
}
