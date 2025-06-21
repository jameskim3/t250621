using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using OpenCvSharp;
using System;
using System.IO;
using Size = System.Drawing.Size;
using CvSize = OpenCvSharp.Size;
using WinSize = System.Drawing.Size;
using CvPoint = OpenCvSharp.Point;
using WinPoint = System.Drawing.Point;

namespace aruco_test
{
    public class DistortedChessboardGenerator
    {
        private int cols = 9, rows = 6;       // 내부 코너 수
        private int squareSize = 50;          // 한 칸 크기 (픽셀)
        private int boardWidth, boardHeight;
        private string folder = "distorted_chessboards";

        public DistortedChessboardGenerator()
        {
            boardWidth = (cols + 1) * squareSize;
            boardHeight = (rows + 1) * squareSize;
            Directory.CreateDirectory(folder);
        }

        public void GenerateAll(int count = 10)
        {
            // 원본 정면 체스보드
            Mat baseBoard = GenerateBaseBoard();

            for (int i = 0; i < count; i++)
            {
                Mat warped = ApplyRandomPerspective(baseBoard, i);
                string path = Path.Combine(folder, $"warp_{i + 1}.png");
                Cv2.ImWrite(path, warped);
                Console.WriteLine($"Saved: {path}");
            }
        }

        private Mat GenerateBaseBoard()
        {
            var mat = new Mat(new CvSize(300, 300), MatType.CV_8UC1, Scalar.White);
            for (int i = 0; i <= rows; i++)
            {
                for (int j = 0; j <= cols; j++)
                {
                    if ((i + j) % 2 == 0)
                    {
                        Cv2.Rectangle(mat,
                            new CvPoint(j * squareSize, i * squareSize),
                            new CvPoint((j + 1) * squareSize, (i + 1) * squareSize),
                            Scalar.Black, -1);
                    }
                }
            }
            return mat;
        }

        private Mat ApplyRandomPerspective(Mat input, int seed)
        {
            int margin = 50;
            Random rand = new Random(seed * 1234);

            // 원래 네 점
            Point2f[] src = new Point2f[]
            {
            new Point2f(0, 0),
            new Point2f(input.Cols - 1, 0),
            new Point2f(input.Cols - 1, input.Rows - 1),
            new Point2f(0, input.Rows - 1)
            };

            // 랜덤 왜곡 네 점
            Point2f[] dst = new Point2f[4];
            for (int i = 0; i < 4; i++)
            {
                float dx = (float)(rand.NextDouble() * margin - margin / 2);
                float dy = (float)(rand.NextDouble() * margin - margin / 2);
                dst[i] = new Point2f(src[i].X + dx, src[i].Y + dy);
            }

            Mat H = Cv2.GetPerspectiveTransform(src, dst);
            Mat output = new Mat();
            Cv2.WarpPerspective(input, output, H, input.Size(), InterpolationFlags.Linear, BorderTypes.Constant, Scalar.White);
            return output;
        }
    }
}