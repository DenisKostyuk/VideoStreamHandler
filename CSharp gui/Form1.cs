using System;
using System.Diagnostics;
using System.Drawing;
using System.Runtime.InteropServices;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace WindowsFormsApp1
{
    public partial class Form1 : Form
    {

        [DllImport("C:\\Users\\denis\\Desktop\\FinalProject\\Submition Request for 1\\x64\\Release\\Project1.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void playFile(IntPtr hwnd);

        [DllImport("C:\\Users\\denis\\Desktop\\FinalProject\\Submition Request for 1\\x64\\Release\\Project1.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void playWebcam(IntPtr hwnd);

        [DllImport("C:\\Users\\denis\\Desktop\\FinalProject\\Submition Request for 1\\x64\\Release\\Project1.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void stopCapture();

        private bool isCapturing = false;
        private Task captureTask;

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            CheckForMediaLooksSDK();
            
        }

        private void CheckForMediaLooksSDK()
        {
            DialogResult result = MessageBox.Show("Do you have the MediaLooks SDK installed on your computer?",
                                                  "MediaLooks SDK", MessageBoxButtons.YesNo, MessageBoxIcon.Question);
            if (result == DialogResult.No)
            {
                string sdkDownloadURL = "https://s3.eu-central-1.amazonaws.com/injecto.setup.files/MFormats_SDK_2.7.0.13139.H264TC.msi";
                Process.Start(sdkDownloadURL);
                MessageBox.Show("Please download and install the MediaLooks SDK from the provided link. After downloading and installing the SDK, please come back to the application.",
                                "Install SDK", MessageBoxButtons.OK, MessageBoxIcon.Information);
                Application.Exit();
            }
        }

       

        private void buttonWebcamCapture_Click(object sender, EventArgs e)
        {
            if (!isCapturing)
            {
                StartCapture(playWebcam);
            }
        }

        private void buttonFileCapture_Click(object sender, EventArgs e)
        {
            if (!isCapturing)
            {
                StartCapture(playFile);
            }
        }

        private void buttonStop_Click(object sender, EventArgs e)
        {
            if (isCapturing)
            {
                StopCapture();
            }
        }

        private void StartCapture(Action<IntPtr> captureMethod)
        {
            try
            {
                IntPtr hwnd = pictureBox1.Handle;
                isCapturing = true;
                captureTask = Task.Run(() => captureMethod(hwnd));
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error: {ex.Message}");
                isCapturing = false;
            }
        }

        private void StopCapture()
        {
            try
            {
                stopCapture();
                isCapturing = false;
                captureTask?.Wait(); // Wait for the capture task to finish
                ClearPictureBox();
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error stopping capture: {ex.Message}");
            }
        }

        private void ClearPictureBox()
        {
            if (pictureBox1.InvokeRequired)
            {
                pictureBox1.Invoke(new Action(ClearPictureBox));
            }
            else
            {
                pictureBox1.Image = null;
            }
        }

        public void UpdatePictureBox(Bitmap frame)
        {
            if (pictureBox1.InvokeRequired)
            {
                pictureBox1.Invoke(new Action<Bitmap>(UpdatePictureBox), frame);
            }
            else
            {
                pictureBox1.Image = frame;
            }
        }
    }
}
