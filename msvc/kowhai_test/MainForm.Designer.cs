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
            this.tableLayoutPanel1.SuspendLayout();
            this.panel6.SuspendLayout();
            this.SuspendLayout();
            // 
            // btnSave
            // 
            this.btnSave.Location = new System.Drawing.Point(194, 9);
            this.btnSave.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.btnSave.Name = "btnSave";
            this.btnSave.Size = new System.Drawing.Size(56, 21);
            this.btnSave.TabIndex = 2;
            this.btnSave.Text = "Save";
            this.btnSave.UseVisualStyleBackColor = true;
            this.btnSave.Click += new System.EventHandler(this.btnSave_Click);
            // 
            // btnLoad
            // 
            this.btnLoad.Location = new System.Drawing.Point(254, 9);
            this.btnLoad.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.btnLoad.Name = "btnLoad";
            this.btnLoad.Size = new System.Drawing.Size(56, 21);
            this.btnLoad.TabIndex = 3;
            this.btnLoad.Text = "Load";
            this.btnLoad.UseVisualStyleBackColor = true;
            this.btnLoad.Click += new System.EventHandler(this.btnLoad_Click);
            // 
            // btnMerge
            // 
            this.btnMerge.Location = new System.Drawing.Point(574, 9);
            this.btnMerge.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.btnMerge.Name = "btnMerge";
            this.btnMerge.Size = new System.Drawing.Size(117, 21);
            this.btnMerge.TabIndex = 6;
            this.btnMerge.Text = "Merge To Scratch";
            this.btnMerge.UseVisualStyleBackColor = true;
            this.btnMerge.Click += new System.EventHandler(this.btnMerge_Click);
            // 
            // btnDiff
            // 
            this.btnDiff.Location = new System.Drawing.Point(453, 9);
            this.btnDiff.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.btnDiff.Name = "btnDiff";
            this.btnDiff.Size = new System.Drawing.Size(117, 21);
            this.btnDiff.TabIndex = 5;
            this.btnDiff.Text = "Diff To Scratch";
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
            this.tableLayoutPanel1.Location = new System.Drawing.Point(12, 35);
            this.tableLayoutPanel1.Name = "tableLayoutPanel1";
            this.tableLayoutPanel1.RowCount = 2;
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 15F));
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel1.Size = new System.Drawing.Size(681, 418);
            this.tableLayoutPanel1.TabIndex = 14;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(228, 0);
            this.label3.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(41, 13);
            this.label3.TabIndex = 22;
            this.label3.Text = "Current";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(2, 0);
            this.label2.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(29, 13);
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
            this.panel6.Location = new System.Drawing.Point(0, 15);
            this.panel6.Margin = new System.Windows.Forms.Padding(0);
            this.panel6.Name = "panel6";
            this.panel6.Size = new System.Drawing.Size(226, 403);
            this.panel6.TabIndex = 20;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(2, 138);
            this.label4.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(48, 13);
            this.label4.TabIndex = 22;
            this.label4.Text = "Function";
            // 
            // lbFunctionList
            // 
            this.lbFunctionList.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.lbFunctionList.FormattingEnabled = true;
            this.lbFunctionList.Location = new System.Drawing.Point(2, 154);
            this.lbFunctionList.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.lbFunctionList.Name = "lbFunctionList";
            this.lbFunctionList.Size = new System.Drawing.Size(223, 147);
            this.lbFunctionList.TabIndex = 8;
            this.lbFunctionList.DoubleClick += new System.EventHandler(this.lbFunctionList_DoubleClick);
            // 
            // lbTreeList
            // 
            this.lbTreeList.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.lbTreeList.FormattingEnabled = true;
            this.lbTreeList.Location = new System.Drawing.Point(2, 2);
            this.lbTreeList.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.lbTreeList.Name = "lbTreeList";
            this.lbTreeList.Size = new System.Drawing.Size(223, 134);
            this.lbTreeList.TabIndex = 7;
            this.lbTreeList.SelectedIndexChanged += new System.EventHandler(this.lbTreeList_SelectedIndexChanged);
            // 
            // pnlScope
            // 
            this.pnlScope.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.pnlScope.Location = new System.Drawing.Point(3, 311);
            this.pnlScope.Name = "pnlScope";
            this.pnlScope.Size = new System.Drawing.Size(220, 89);
            this.pnlScope.TabIndex = 5;
            this.pnlScope.Paint += new System.Windows.Forms.PaintEventHandler(this.OnPaint);
            // 
            // kowhaiTreeScratch
            // 
            this.kowhaiTreeScratch.ContextMenuEnabled = true;
            this.kowhaiTreeScratch.Dock = System.Windows.Forms.DockStyle.Fill;
            this.kowhaiTreeScratch.EditDataInPlace = false;
            this.kowhaiTreeScratch.Location = new System.Drawing.Point(455, 17);
            this.kowhaiTreeScratch.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.kowhaiTreeScratch.Name = "kowhaiTreeScratch";
            this.kowhaiTreeScratch.Size = new System.Drawing.Size(224, 399);
            this.kowhaiTreeScratch.TabIndex = 9;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(455, 0);
            this.label1.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(44, 13);
            this.label1.TabIndex = 14;
            this.label1.Text = "Scratch";
            // 
            // kowhaiTreeMain
            // 
            this.kowhaiTreeMain.ContextMenuEnabled = true;
            this.kowhaiTreeMain.Dock = System.Windows.Forms.DockStyle.Fill;
            this.kowhaiTreeMain.EditDataInPlace = false;
            this.kowhaiTreeMain.Location = new System.Drawing.Point(228, 17);
            this.kowhaiTreeMain.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.kowhaiTreeMain.Name = "kowhaiTreeMain";
            this.kowhaiTreeMain.Size = new System.Drawing.Size(223, 399);
            this.kowhaiTreeMain.TabIndex = 8;
            // 
            // btnLoadScratch
            // 
            this.btnLoadScratch.Location = new System.Drawing.Point(332, 9);
            this.btnLoadScratch.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.btnLoadScratch.Name = "btnLoadScratch";
            this.btnLoadScratch.Size = new System.Drawing.Size(117, 21);
            this.btnLoadScratch.TabIndex = 4;
            this.btnLoadScratch.Text = "Load To Scratch";
            this.btnLoadScratch.UseVisualStyleBackColor = true;
            this.btnLoadScratch.Click += new System.EventHandler(this.btnLoadScratch_Click);
            // 
            // btnRefreshList
            // 
            this.btnRefreshList.Location = new System.Drawing.Point(12, 9);
            this.btnRefreshList.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.btnRefreshList.Name = "btnRefreshList";
            this.btnRefreshList.Size = new System.Drawing.Size(75, 21);
            this.btnRefreshList.TabIndex = 0;
            this.btnRefreshList.Text = "Refresh List";
            this.btnRefreshList.UseVisualStyleBackColor = true;
            this.btnRefreshList.Click += new System.EventHandler(this.btnRefreshList_Click);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(705, 465);
            this.Controls.Add(this.btnRefreshList);
            this.Controls.Add(this.btnLoadScratch);
            this.Controls.Add(this.tableLayoutPanel1);
            this.Controls.Add(this.btnDiff);
            this.Controls.Add(this.btnLoad);
            this.Controls.Add(this.btnSave);
            this.Controls.Add(this.btnMerge);
            this.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
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
    }
}
