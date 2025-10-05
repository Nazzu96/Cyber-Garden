using System;
using System.IO.Ports;
using System.Text;
using System.Windows.Forms;

namespace DesktopClient
{
    public partial class Form1 : Form
    {
        private SerialPort serialPort;
        private bool isConnected = false;

        public Form1()
        {
            InitializeComponent();
            serialPort = new SerialPort("COM9", 9600);
            serialPort.Encoding = Encoding.UTF8;
            serialPort.NewLine = "\n";
            serialPort.DataReceived += SerialPort_DataReceived;

        }

        private void SerialPort_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            try
            {
                string line = serialPort.ReadLine().Trim();

                if (line.StartsWith("Предложение:"))
                {
                    int sep = line.IndexOf("| Код Морзе:");
                    if (sep >= 0)
                    {
                        string sentence = line.Substring("Предложение:".Length, sep - "Предложение:".Length).Trim();
                        string morse = line.Substring(sep + "| Код Морзе:".Length).Trim();

                        this.BeginInvoke((Action)(() =>
                        {
                            txtMorse.AppendText(morse + Environment.NewLine);
                            txtText.AppendText(sentence + Environment.NewLine);
                        }));
                    }
                }
                else if (line.StartsWith("Цвет:"))
                {
                    string color = line.Substring("Цвет:".Length).Trim();
                    this.BeginInvoke((Action)(() =>
                    {
                        lblStatus.Text = "Последний цвет: " + color;
                    }));
                }
                else
                {
                    this.BeginInvoke((Action)(() =>
                    {
                        txtText.AppendText("[log] " + line + Environment.NewLine);
                    }));
                }
            }
            catch { }
        }

        private void txtMorse_TextChanged(object sender, EventArgs e)
        {

        }

        private void btnGetData_Click(object sender, EventArgs e)
        {
            try
            {
                if (!isConnected)
                {
                    if (!serialPort.IsOpen)
                        serialPort.Open();

                    isConnected = true;
                    btnGetData.Text = "Остановить";
                    lblStatus.Text = "Статус: подключено (COM9, 9600)";
                    txtMorse.Clear();
                    txtText.Clear();
                }
                else
                {
                    if (serialPort.IsOpen)
                        serialPort.Close();

                    isConnected = false;
                    btnGetData.Text = "получить данные";
                    lblStatus.Text = "Статус: не подключено";
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Ошибка COM-порта: " + ex.Message);
                lblStatus.Text = "Статус: ошибка подключения";
            }
        }

        protected override void OnFormClosing(FormClosingEventArgs e)
        {
            if (serialPort != null && serialPort.IsOpen)
                serialPort.Close();

            base.OnFormClosing(e);
        }
    }
}
