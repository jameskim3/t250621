using OpenCvSharp;
using OpenCvSharp.Aruco;
using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.Windows.Forms;

namespace ArucoMarkerApp
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            this.Text = "ArUco Marker ID 0";
            this.Load += Form1_Load;
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            int markerSize = 300;
            int markerId = 0;

            var dict = CvAruco.GetPredefinedDictionary(PredefinedDictionaryName.Dict4X4_50);
            using (var mat = new Mat())
            {
                dict.GenerateImageMarker(markerId, markerSize, mat, 1); // ✅ 대신 DrawMarker 안 씀
                Bitmap bmp = MatToBitmap(mat);

                var pictureBox = new PictureBox
                {
                    Size = new System.Drawing.Size(markerSize, markerSize),        // ✅ 명확하게 지정
                    Location = new System.Drawing.Point(10, 10),
                    SizeMode = PictureBoxSizeMode.Zoom,
                    BorderStyle = BorderStyle.FixedSingle
                };

                this.Controls.Add(pictureBox);
                this.ClientSize = new System.Drawing.Size(markerSize + 20, markerSize + 20);
                //this.ClientSize = new Size(markerSize + 20, markerSize + 20);
            }
        }

        private Bitmap MatToBitmap(Mat mat)
        {
            if (mat.Type() != MatType.CV_8UC1 && mat.Type() != MatType.CV_8UC3)
                throw new NotSupportedException("Only 8UC1 or 8UC3 supported");

            PixelFormat format = mat.Type() == MatType.CV_8UC1
                ? PixelFormat.Format8bppIndexed
                : PixelFormat.Format24bppRgb;

            Bitmap bitmap = new Bitmap(mat.Width, mat.Height, format);
            BitmapData data = bitmap.LockBits(new Rectangle(0, 0, mat.Width, mat.Height),
                ImageLockMode.WriteOnly, format);

            mat.GetArray(out byte[] bytes);
            System.Runtime.InteropServices.Marshal.Copy(bytes, 0, data.Scan0, bytes.Length);
            bitmap.UnlockBits(data);

            if (format == PixelFormat.Format8bppIndexed)
            {
                ColorPalette palette = bitmap.Palette;
                for (int i = 0; i < 256; i++)
                    palette.Entries[i] = Color.FromArgb(i, i, i);
                bitmap.Palette = palette;
            }

            return bitmap;
        }
    }
}
