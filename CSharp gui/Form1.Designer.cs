namespace WindowsFormsApp1
{
    partial class Form1
    {
        private System.ComponentModel.IContainer components = null;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.Button buttonWebcamCapture;
        private System.Windows.Forms.Button buttonFileCapture;
        private System.Windows.Forms.Button buttonStop;

        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        private void InitializeComponent()
        {
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.buttonWebcamCapture = new System.Windows.Forms.Button();
            this.buttonFileCapture = new System.Windows.Forms.Button();
            this.buttonStop = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.SuspendLayout();
            // 
            // pictureBox1
            // 
            this.pictureBox1.Location = new System.Drawing.Point(12, 12);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(640, 480);
            this.pictureBox1.TabIndex = 0;
            this.pictureBox1.TabStop = false;
            // 
            // buttonWebcamCapture
            // 
            this.buttonWebcamCapture.Location = new System.Drawing.Point(658, 12);
            this.buttonWebcamCapture.Name = "buttonWebcamCapture";
            this.buttonWebcamCapture.Size = new System.Drawing.Size(100, 64);
            this.buttonWebcamCapture.TabIndex = 1;
            this.buttonWebcamCapture.Text = "Start Capture From WebCam";
            this.buttonWebcamCapture.UseVisualStyleBackColor = true;
            this.buttonWebcamCapture.Click += new System.EventHandler(this.buttonWebcamCapture_Click);
            // 
            // buttonFileCapture
            // 
            this.buttonFileCapture.Location = new System.Drawing.Point(658, 99);
            this.buttonFileCapture.Name = "buttonFileCapture";
            this.buttonFileCapture.Size = new System.Drawing.Size(100, 55);
            this.buttonFileCapture.TabIndex = 2;
            this.buttonFileCapture.Text = "Start Capture From File";
            this.buttonFileCapture.UseVisualStyleBackColor = true;
            this.buttonFileCapture.Click += new System.EventHandler(this.buttonFileCapture_Click);
            // 
            // buttonStop
            // 
            this.buttonStop.Location = new System.Drawing.Point(658, 181);
            this.buttonStop.Name = "buttonStop";
            this.buttonStop.Size = new System.Drawing.Size(100, 56);
            this.buttonStop.TabIndex = 3;
            this.buttonStop.Text = "Stop";
            this.buttonStop.UseVisualStyleBackColor = true;
            this.buttonStop.Click += new System.EventHandler(this.buttonStop_Click);
            // 
            // Form1
            // 
            this.ClientSize = new System.Drawing.Size(800, 600);
            this.Controls.Add(this.buttonStop);
            this.Controls.Add(this.buttonFileCapture);
            this.Controls.Add(this.buttonWebcamCapture);
            this.Controls.Add(this.pictureBox1);
            this.Name = "Form1";
            this.Text = "TV";
            this.Load += new System.EventHandler(this.Form1_Load);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.ResumeLayout(false);

        }
    }
}
