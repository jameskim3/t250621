﻿namespace aruco_test
{
    partial class Form3
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            button3 = new Button();
            pictureBox1 = new PictureBox();
            button4 = new Button();
            button5 = new Button();
            ((System.ComponentModel.ISupportInitialize)pictureBox1).BeginInit();
            SuspendLayout();
            // 
            // button3
            // 
            button3.Location = new Point(1403, 85);
            button3.Name = "button3";
            button3.Size = new Size(68, 77);
            button3.TabIndex = 0;
            button3.Text = "cal";
            button3.UseVisualStyleBackColor = true;
            button3.Click += button3_Click;
            // 
            // pictureBox1
            // 
            pictureBox1.Location = new Point(23, 12);
            pictureBox1.Name = "pictureBox1";
            pictureBox1.Size = new Size(1155, 1039);
            pictureBox1.TabIndex = 1;
            pictureBox1.TabStop = false;
            // 
            // button4
            // 
            button4.Location = new Point(1396, 215);
            button4.Name = "button4";
            button4.Size = new Size(66, 79);
            button4.TabIndex = 2;
            button4.Text = "overlay";
            button4.UseVisualStyleBackColor = true;
            button4.Click += button4_Click;
            // 
            // button5
            // 
            button5.Location = new Point(1399, 336);
            button5.Name = "button5";
            button5.Size = new Size(65, 66);
            button5.TabIndex = 3;
            button5.Text = "gen";
            button5.UseVisualStyleBackColor = true;
            button5.Click += button5_Click;
            // 
            // Form3
            // 
            AutoScaleDimensions = new SizeF(10F, 25F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(1548, 1150);
            Controls.Add(button5);
            Controls.Add(button4);
            Controls.Add(pictureBox1);
            Controls.Add(button3);
            Name = "Form3";
            Text = "chessbd";
            ((System.ComponentModel.ISupportInitialize)pictureBox1).EndInit();
            ResumeLayout(false);
        }

        #endregion

        private Button button1;
        private Button button2;
        private Button button3;
        private PictureBox pictureBox1;
        private Button button4;
        private Button button5;
    }
}