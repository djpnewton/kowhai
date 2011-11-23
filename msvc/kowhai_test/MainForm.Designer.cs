namespace kowhai_test
{
    partial class MainForm
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
            this.button1 = new System.Windows.Forms.Button();
            this.kowhaiTreeShadow = new kowhai_sharp.KowhaiTree();
            this.kowhaiTreeSettings = new kowhai_sharp.KowhaiTree();
            this.kowhaiTreeActions = new kowhai_sharp.KowhaiTree();
            this.SuspendLayout();
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(9, 10);
            this.button1.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(56, 28);
            this.button1.TabIndex = 0;
            this.button1.Text = "button1";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // kowhaiTreeShadow
            // 
            this.kowhaiTreeShadow.Location = new System.Drawing.Point(270, 42);
            this.kowhaiTreeShadow.Margin = new System.Windows.Forms.Padding(2);
            this.kowhaiTreeShadow.Name = "kowhaiTreeShadow";
            this.kowhaiTreeShadow.Size = new System.Drawing.Size(274, 415);
            this.kowhaiTreeShadow.TabIndex = 2;
            // 
            // kowhaiTreeSettings
            // 
            this.kowhaiTreeSettings.Location = new System.Drawing.Point(9, 42);
            this.kowhaiTreeSettings.Margin = new System.Windows.Forms.Padding(2);
            this.kowhaiTreeSettings.Name = "kowhaiTreeSettings";
            this.kowhaiTreeSettings.Size = new System.Drawing.Size(257, 415);
            this.kowhaiTreeSettings.TabIndex = 1;
            // 
            // kowhaiTreeActions
            // 
            this.kowhaiTreeActions.Location = new System.Drawing.Point(548, 42);
            this.kowhaiTreeActions.Margin = new System.Windows.Forms.Padding(2);
            this.kowhaiTreeActions.Name = "kowhaiTreeActions";
            this.kowhaiTreeActions.Size = new System.Drawing.Size(274, 415);
            this.kowhaiTreeActions.TabIndex = 3;
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(830, 465);
            this.Controls.Add(this.kowhaiTreeActions);
            this.Controls.Add(this.kowhaiTreeShadow);
            this.Controls.Add(this.kowhaiTreeSettings);
            this.Controls.Add(this.button1);
            this.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.Name = "MainForm";
            this.Text = "MainForm";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.MainForm_FormClosed);
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button button1;
        private kowhai_sharp.KowhaiTree kowhaiTreeSettings;
        private kowhai_sharp.KowhaiTree kowhaiTreeShadow;
        private kowhai_sharp.KowhaiTree kowhaiTreeActions;
    }
}