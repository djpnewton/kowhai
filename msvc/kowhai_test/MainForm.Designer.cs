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
            this.btnSave = new System.Windows.Forms.Button();
            this.btnLoad = new System.Windows.Forms.Button();
            this.btnMerge = new System.Windows.Forms.Button();
            this.btnDiff = new System.Windows.Forms.Button();
            this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.panel6 = new System.Windows.Forms.Panel();
            this.label4 = new System.Windows.Forms.Label();
            this.lbFunctionList = new System.Windows.Forms.ListBox();
            this.lbTreeList = new System.Windows.Forms.ListBox();
            this.pnlScope = new System.Windows.Forms.Panel();
            this.kowhaiTreeScratch = new kowhai_sharp.KowhaiTree();
            this.label1 = new System.Windows.Forms.Label();
            this.kowhaiTreeMain = new kowhai_sharp.KowhaiTree();
            this.btnLoadScratch = new System.Windows.Forms.Button();
            this.btnRefreshList = new System.Windows.Forms.Button();
            this.btnSocket = new System.Windows.Forms.Button();
            this.btnHID = new System.Windows.Forms.Button();
            this.btnDisconnect = new System.Windows.Forms.Button();
            this.tableLayoutPanel1.SuspendLayout();
            this.panel6.SuspendLayout();
            this.SuspendLayout();
            // 
            // btnSave
            // 
            this.btnSave.Location = new System.Drawing.Point(321, 11);
            this.btnSave.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.btnSave.Name = "btnSave";
            this.btnSave.Size = new System.Drawing.Size(56, 26);
            this.btnSave.TabIndex = 3;
            this.btnSave.Text = "Save";
            this.btnSave.UseVisualStyleBackColor = true;
            this.btnSave.Click += new System.EventHandler(this.btnSave_Click);
            // 
            // btnLoad
            // 
            this.btnLoad.Location = new System.Drawing.Point(383, 11);
            this.btnLoad.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.btnLoad.Name = "btnLoad";
            this.btnLoad.Size = new System.Drawing.Size(56, 26);
            this.btnLoad.TabIndex = 4;
            this.btnLoad.Text = "Load";
            this.btnLoad.UseVisualStyleBackColor = true;
            this.btnLoad.Click += new System.EventHandler(this.btnLoad_Click);
            // 
            // btnMerge
            // 
            this.btnMerge.Location = new System.Drawing.Point(747, 11);
            this.btnMerge.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.btnMerge.Name = "btnMerge";
            this.btnMerge.Size = new System.Drawing.Size(56, 26);
            this.btnMerge.TabIndex = 7;
            this.btnMerge.Text = "Merge";
            this.btnMerge.UseVisualStyleBackColor = true;
            this.btnMerge.Click += new System.EventHandler(this.btnMerge_Click);
            // 
            // btnDiff
            // 
            this.btnDiff.Location = new System.Drawing.Point(685, 11);
            this.btnDiff.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.btnDiff.Name = "btnDiff";
            this.btnDiff.Size = new System.Drawing.Size(56, 26);
            this.btnDiff.TabIndex = 6;
            this.btnDiff.Text = "Diff";
            this.btnDiff.UseVisualStyleBackColor = true;
            this.btnDiff.Click += new System.EventHandler(this.btnDiff_Click);
            // 
            // tableLayoutPanel1
            // 
            this.tableLayoutPanel1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tableLayoutPanel1.ColumnCount = 3;
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 33.33333F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 33.33334F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 33.33334F));
            this.tableLayoutPanel1.Controls.Add(this.label3, 1, 0);
            this.tableLayoutPanel1.Controls.Add(this.label2, 0, 0);
            this.tableLayoutPanel1.Controls.Add(this.panel6, 0, 1);
            this.tableLayoutPanel1.Controls.Add(this.kowhaiTreeScratch, 2, 1);
            this.tableLayoutPanel1.Controls.Add(this.label1, 2, 0);
            this.tableLayoutPanel1.Controls.Add(this.kowhaiTreeMain, 1, 1);
            this.tableLayoutPanel1.Location = new System.Drawing.Point(16, 43);
            this.tableLayoutPanel1.Margin = new System.Windows.Forms.Padding(4);
            this.tableLayoutPanel1.Name = "tableLayoutPanel1";
            this.tableLayoutPanel1.RowCount = 2;
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 18F));
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel1.Size = new System.Drawing.Size(908, 514);
            this.tableLayoutPanel1.TabIndex = 14;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(305, 0);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(55, 17);
            this.label3.TabIndex = 22;
            this.label3.Text = "Current";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(3, 0);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(38, 17);
            this.label2.TabIndex = 21;
            this.label2.Text = "Tree";
            // 
            // panel6
            // 
            this.panel6.Controls.Add(this.label4);
            this.panel6.Controls.Add(this.lbFunctionList);
            this.panel6.Controls.Add(this.lbTreeList);
            this.panel6.Controls.Add(this.pnlScope);
            this.panel6.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel6.Location = new System.Drawing.Point(0, 18);
            this.panel6.Margin = new System.Windows.Forms.Padding(0);
            this.panel6.Name = "panel6";
            this.panel6.Size = new System.Drawing.Size(302, 496);
            this.panel6.TabIndex = 20;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(3, 170);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(62, 17);
            this.label4.TabIndex = 22;
            this.label4.Text = "Function";
            // 
            // lbFunctionList
            // 
            this.lbFunctionList.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.lbFunctionList.FormattingEnabled = true;
            this.lbFunctionList.ItemHeight = 16;
            this.lbFunctionList.Location = new System.Drawing.Point(3, 190);
            this.lbFunctionList.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.lbFunctionList.Name = "lbFunctionList";
            this.lbFunctionList.Size = new System.Drawing.Size(297, 180);
            this.lbFunctionList.TabIndex = 9;
            this.lbFunctionList.DoubleClick += new System.EventHandler(this.lbFunctionList_DoubleClick);
            // 
            // lbTreeList
            // 
            this.lbTreeList.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.lbTreeList.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.lbTreeList.FormattingEnabled = true;
            this.lbTreeList.ItemHeight = 16;
            this.lbTreeList.Location = new System.Drawing.Point(3, 2);
            this.lbTreeList.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.lbTreeList.Name = "lbTreeList";
            this.lbTreeList.Size = new System.Drawing.Size(297, 164);
            this.lbTreeList.TabIndex = 8;
            this.lbTreeList.DrawItem += new System.Windows.Forms.DrawItemEventHandler(this.lbTreeList_DrawItem);
            this.lbTreeList.SelectedIndexChanged += new System.EventHandler(this.lbTreeList_SelectedIndexChanged);
            // 
            // pnlScope
            // 
            this.pnlScope.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.pnlScope.Location = new System.Drawing.Point(4, 383);
            this.pnlScope.Margin = new System.Windows.Forms.Padding(4);
            this.pnlScope.Name = "pnlScope";
            this.pnlScope.Size = new System.Drawing.Size(294, 110);
            this.pnlScope.TabIndex = 10;
            this.pnlScope.Paint += new System.Windows.Forms.PaintEventHandler(this.OnPaint);
            // 
            // kowhaiTreeScratch
            // 
            this.kowhaiTreeScratch.ContextMenuEnabled = true;
            this.kowhaiTreeScratch.Dock = System.Windows.Forms.DockStyle.Fill;
            this.kowhaiTreeScratch.EditDataInPlace = false;
            this.kowhaiTreeScratch.Location = new System.Drawing.Point(607, 20);
            this.kowhaiTreeScratch.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.kowhaiTreeScratch.Name = "kowhaiTreeScratch";
            this.kowhaiTreeScratch.Size = new System.Drawing.Size(298, 492);
            this.kowhaiTreeScratch.TabIndex = 12;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(607, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(56, 17);
            this.label1.TabIndex = 14;
            this.label1.Text = "Scratch";
            // 
            // kowhaiTreeMain
            // 
            this.kowhaiTreeMain.ContextMenuEnabled = true;
            this.kowhaiTreeMain.Dock = System.Windows.Forms.DockStyle.Fill;
            this.kowhaiTreeMain.EditDataInPlace = false;
            this.kowhaiTreeMain.Location = new System.Drawing.Point(305, 20);
            this.kowhaiTreeMain.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.kowhaiTreeMain.Name = "kowhaiTreeMain";
            this.kowhaiTreeMain.Size = new System.Drawing.Size(296, 492);
            this.kowhaiTreeMain.TabIndex = 11;
            // 
            // btnLoadScratch
            // 
            this.btnLoadScratch.Location = new System.Drawing.Point(623, 11);
            this.btnLoadScratch.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.btnLoadScratch.Name = "btnLoadScratch";
            this.btnLoadScratch.Size = new System.Drawing.Size(56, 26);
            this.btnLoadScratch.TabIndex = 5;
            this.btnLoadScratch.Text = "Load";
            this.btnLoadScratch.UseVisualStyleBackColor = true;
            this.btnLoadScratch.Click += new System.EventHandler(this.btnLoadScratch_Click);
            // 
            // btnRefreshList
            // 
            this.btnRefreshList.Location = new System.Drawing.Point(153, 10);
            this.btnRefreshList.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.btnRefreshList.Name = "btnRefreshList";
            this.btnRefreshList.Size = new System.Drawing.Size(100, 26);
            this.btnRefreshList.TabIndex = 2;
            this.btnRefreshList.Text = "Refresh List";
            this.btnRefreshList.UseVisualStyleBackColor = true;
            this.btnRefreshList.Click += new System.EventHandler(this.btnRefreshList_Click);
            // 
            // btnSocket
            // 
            this.btnSocket.Location = new System.Drawing.Point(19, 10);
            this.btnSocket.Name = "btnSocket";
            this.btnSocket.Size = new System.Drawing.Size(61, 26);
            this.btnSocket.TabIndex = 0;
            this.btnSocket.Text = "Socket";
            this.btnSocket.UseVisualStyleBackColor = true;
            this.btnSocket.Click += new System.EventHandler(this.btnSocket_Click);
            // 
            // btnHID
            // 
            this.btnHID.Location = new System.Drawing.Point(86, 10);
            this.btnHID.Name = "btnHID";
            this.btnHID.Size = new System.Drawing.Size(61, 26);
            this.btnHID.TabIndex = 1;
            this.btnHID.Text = "HID";
            this.btnHID.UseVisualStyleBackColor = true;
            this.btnHID.Click += new System.EventHandler(this.btnHID_Click);
            // 
            // btnDisconnect
            // 
            this.btnDisconnect.Location = new System.Drawing.Point(153, 36);
            this.btnDisconnect.Name = "btnDisconnect";
            this.btnDisconnect.Size = new System.Drawing.Size(100, 26);
            this.btnDisconnect.TabIndex = 15;
            this.btnDisconnect.Text = "Disconnect";
            this.btnDisconnect.UseVisualStyleBackColor = true;
            this.btnDisconnect.Visible = false;
            this.btnDisconnect.Click += new System.EventHandler(this.btnDisconnect_Click);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(940, 572);
            this.Controls.Add(this.btnDisconnect);
            this.Controls.Add(this.btnHID);
            this.Controls.Add(this.btnSocket);
            this.Controls.Add(this.btnRefreshList);
            this.Controls.Add(this.btnLoadScratch);
            this.Controls.Add(this.tableLayoutPanel1);
            this.Controls.Add(this.btnDiff);
            this.Controls.Add(this.btnLoad);
            this.Controls.Add(this.btnSave);
            this.Controls.Add(this.btnMerge);
            this.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.Name = "MainForm";
            this.Text = "MainForm";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.MainForm_FormClosed);
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.tableLayoutPanel1.ResumeLayout(false);
            this.tableLayoutPanel1.PerformLayout();
            this.panel6.ResumeLayout(false);
            this.panel6.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button btnSave;
        private System.Windows.Forms.Button btnLoad;
        private System.Windows.Forms.Button btnMerge;
        private System.Windows.Forms.Button btnDiff;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
        private kowhai_sharp.KowhaiTree kowhaiTreeScratch;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button btnLoadScratch;
        private kowhai_sharp.KowhaiTree kowhaiTreeMain;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Panel panel6;
        private System.Windows.Forms.ListBox lbTreeList;
        private System.Windows.Forms.Panel pnlScope;
        private System.Windows.Forms.Button btnRefreshList;
        private System.Windows.Forms.ListBox lbFunctionList;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Button btnSocket;
        private System.Windows.Forms.Button btnHID;
        private System.Windows.Forms.Button btnDisconnect;
    }
}
