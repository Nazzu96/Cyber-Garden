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

                if (line.StartsWith("�����������:"))
                {
                    int sep = line.IndexOf("| ��� �����:");
                    if (sep >= 0)
                    {
                        string sentence = line.Substring("�����������:".Length, sep - "�����������:".Length).Trim();
                        string morse = line.Substring(sep + "| ��� �����:".Length).Trim();

                        this.BeginInvoke((Action)(() =>
                        {
                            txtMorse.AppendText(morse + Environment.NewLine);
                            txtText.AppendText(sentence + Environment.NewLine);
                        }));
                    }
                }
                else if (line.StartsWith("����:"))
                {
                    string color = line.Substring("����:".Length).Trim();
                    this.BeginInvoke((Action)(() =>
                    {
                        lblStatus.Text = "��������� ����: " + color;
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
                    btnGetData.Text = "����������";
                    lblStatus.Text = "������: ���������� (COM9, 9600)";
                    txtMorse.Clear();
                    txtText.Clear();
                }
                else
                {
                    if (serialPort.IsOpen)
                        serialPort.Close();

                    isConnected = false;
                    btnGetData.Text = "�������� ������";
                    lblStatus.Text = "������: �� ����������";
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("������ COM-�����: " + ex.Message);
                lblStatus.Text = "������: ������ �����������";
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
