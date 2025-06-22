using OpenCvSharp;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace aruco_test
{

    using cvsize = OpenCvSharp.Size;
    using dsize = System.Drawing.Size;
    using dpoint = System.Drawing.Point;

    public partial class Form3 : Form
    {
        private CameraCalibrator calibrator;
        DistortedChessboardGenerator generator;


        public Form3()
        {
            InitializeComponent();
            calibrator = new CameraCalibrator();
            generator = new DistortedChessboardGenerator();

        }
        private void button3_Click(object sender, EventArgs e)
        {
            // img 폴더에 체스보드 이미지 있어야 함
            bool success = calibrator.CalibrateFromFolder("img", new cvsize(9, 6), 25f);

            if (success)
                MessageBox.Show("캘리브레이션 성공!", "Info", MessageBoxButtons.OK, MessageBoxIcon.Information);
            else
                MessageBox.Show("캘리브레이션 실패!", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
        }

        private void button4_Click(object sender, EventArgs e)
        {
            string folderPath = "img";
            var patternSize = new cvsize(9, 6);
            float squareSize = 25f;

            var files = Directory.GetFiles(folderPath, "*.png");
            if (files.Length == 0)
            {
                MessageBox.Show("체스보드 이미지 없음");
                return;
            }

            foreach (var file in files)
            {
                using var image = Cv2.ImRead(file);
                using var gray = new Mat();
                Cv2.CvtColor(image, gray, ColorConversionCodes.BGR2GRAY);

                bool found = Cv2.FindChessboardCorners(gray, patternSize, out Point2f[] corners);

                if (found)
                {
                    // 코너 정밀화
                    Cv2.CornerSubPix(gray, corners, new cvsize(11, 11), new cvsize(-1, -1),
                        new TermCriteria(CriteriaTypes.Eps | CriteriaTypes.Count, 30, 0.1));

                    // 이미지에 코너 표시 (오버레이)
                    Cv2.DrawChessboardCorners(image, patternSize, corners, found);
                }

                // WinForms PictureBox에 표시
                pictureBox1.Image?.Dispose();  // 이전 이미지 해제
                pictureBox1.Image = OpenCvSharp.Extensions.BitmapConverter.ToBitmap(image.Clone());

                // 사용자에게 이미지 하나씩 확인할 시간 주기
                Application.DoEvents();  // UI 강제 업데이트
                Thread.Sleep(500);       // 0.5초 대기
            }

            MessageBox.Show("오버레이 표시 완료");
        }

        private void button5_Click(object sender, EventArgs e)
        {
            generator.GenerateAndSaveOne();
        }
    }
}

