namespace DesktopClient
{
    partial class Form1
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
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
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            labelMorse = new Label();
            labelText = new Label();
            txtMorse = new TextBox();
            txtText = new TextBox();
            btnGetData = new Button();
            lblStatus = new Label();
            SuspendLayout();
            // 
            // labelMorse
            // 
            labelMorse.AutoSize = true;
            labelMorse.Location = new Point(12, 18);
            labelMorse.Name = "labelMorse";
            labelMorse.Size = new Size(58, 20);
            labelMorse.TabIndex = 0;
            labelMorse.Text = "Морзе:";
            // 
            // labelText
            // 
            labelText.AutoSize = true;
            labelText.Location = new Point(12, 136);
            labelText.Name = "labelText";
            labelText.Size = new Size(107, 20);
            labelText.TabIndex = 1;
            labelText.Text = "Расшифровка:";
            // 
            // txtMorse
            // 
            txtMorse.Location = new Point(12, 41);
            txtMorse.Multiline = true;
            txtMorse.Name = "txtMorse";
            txtMorse.ReadOnly = true;
            txtMorse.ScrollBars = ScrollBars.Vertical;
            txtMorse.Size = new Size(305, 92);
            txtMorse.TabIndex = 2;
            txtMorse.TextChanged += txtMorse_TextChanged;
            // 
            // txtText
            // 
            txtText.Location = new Point(12, 159);
            txtText.Multiline = true;
            txtText.Name = "txtText";
            txtText.ScrollBars = ScrollBars.Vertical;
            txtText.Size = new Size(305, 125);
            txtText.TabIndex = 3;
            // 
            // btnGetData
            // 
            btnGetData.Location = new Point(118, 362);
            btnGetData.Name = "btnGetData";
            btnGetData.Size = new Size(143, 29);
            btnGetData.TabIndex = 4;
            btnGetData.Text = "Получить данные";
            btnGetData.UseVisualStyleBackColor = true;
            btnGetData.Click += btnGetData_Click;
            // 
            // lblStatus
            // 
            lblStatus.AutoSize = true;
            lblStatus.Location = new Point(107, 339);
            lblStatus.Name = "lblStatus";
            lblStatus.Size = new Size(167, 20);
            lblStatus.TabIndex = 5;
            lblStatus.Text = "Статус: не подключено";
            // 
            // Form1
            // 
            AutoScaleDimensions = new SizeF(8F, 20F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(382, 403);
            Controls.Add(lblStatus);
            Controls.Add(btnGetData);
            Controls.Add(txtText);
            Controls.Add(txtMorse);
            Controls.Add(labelText);
            Controls.Add(labelMorse);
            FormBorderStyle = FormBorderStyle.FixedDialog;
            MaximizeBox = false;
            Name = "Form1";
            StartPosition = FormStartPosition.CenterScreen;
            Text = "Из света в буквы";
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private Label labelMorse;
        private Label labelText;
        private TextBox txtMorse;
        private TextBox txtText;
        private Button btnGetData;
        private Label lblStatus;
    }
}
