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
            this.btnRefreshTrees = new System.Windows.Forms.Button();
            this.btnSave = new System.Windows.Forms.Button();
            this.btnLoad = new System.Windows.Forms.Button();
            this.btnMerge = new System.Windows.Forms.Button();
            this.rbScope = new System.Windows.Forms.RadioButton();
            this.rbActions = new System.Windows.Forms.RadioButton();
            this.rbShadow = new System.Windows.Forms.RadioButton();
            this.rbSettings = new System.Windows.Forms.RadioButton();
            this.kowhaiTreeScope = new kowhai_sharp.KowhaiTree();
            this.kowhaiTreeActions = new kowhai_sharp.KowhaiTree();
            this.kowhaiTreeShadow = new kowhai_sharp.KowhaiTree();
            this.kowhaiTreeSettings = new kowhai_sharp.KowhaiTree();
            this.btnDiff = new System.Windows.Forms.Button();
            this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
            this.panel1 = new System.Windows.Forms.Panel();
            this.pnlScope = new System.Windows.Forms.Panel();
            this.tableLayoutPanel1.SuspendLayout();
            this.panel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // btnRefreshTrees
            // 
            this.btnRefreshTrees.Location = new System.Drawing.Point(9, 9);
            this.btnRefreshTrees.Margin = new System.Windows.Forms.Padding(2);
            this.btnRefreshTrees.Name = "btnRefreshTrees";
            this.btnRefreshTrees.Size = new System.Drawing.Size(87, 21);
            this.btnRefreshTrees.TabIndex = 0;
            this.btnRefreshTrees.Text = "Refresh Trees";
            this.btnRefreshTrees.UseVisualStyleBackColor = true;
            this.btnRefreshTrees.Click += new System.EventHandler(this.btnRefreshTrees_Click);
            // 
            // btnSave
            // 
            this.btnSave.Location = new System.Drawing.Point(100, 9);
            this.btnSave.Margin = new System.Windows.Forms.Padding(2);
            this.btnSave.Name = "btnSave";
            this.btnSave.Size = new System.Drawing.Size(56, 21);
            this.btnSave.TabIndex = 6;
            this.btnSave.Text = "Save";
            this.btnSave.UseVisualStyleBackColor = true;
            this.btnSave.Click += new System.EventHandler(this.btnSave_Click);
            // 
            // btnLoad
            // 
            this.btnLoad.Location = new System.Drawing.Point(161, 9);
            this.btnLoad.Margin = new System.Windows.Forms.Padding(2);
            this.btnLoad.Name = "btnLoad";
            this.btnLoad.Size = new System.Drawing.Size(56, 21);
            this.btnLoad.TabIndex = 7;
            this.btnLoad.Text = "Load";
            this.btnLoad.UseVisualStyleBackColor = true;
            this.btnLoad.Click += new System.EventHandler(this.btnLoad_Click);
            // 
            // btnMerge
            // 
            this.btnMerge.Location = new System.Drawing.Point(344, 9);
            this.btnMerge.Margin = new System.Windows.Forms.Padding(2);
            this.btnMerge.Name = "btnMerge";
            this.btnMerge.Size = new System.Drawing.Size(117, 21);
            this.btnMerge.TabIndex = 12;
            this.btnMerge.Text = "Merge Settings To..";
            this.btnMerge.UseVisualStyleBackColor = true;
            this.btnMerge.Click += new System.EventHandler(this.btnMerge_Click);
            // 
            // rbScope
            // 
            this.rbScope.AutoSize = true;
            this.rbScope.Location = new System.Drawing.Point(512, 2);
            this.rbScope.Margin = new System.Windows.Forms.Padding(2);
            this.rbScope.Name = "rbScope";
            this.rbScope.Size = new System.Drawing.Size(56, 16);
            this.rbScope.TabIndex = 11;
            this.rbScope.Tag = "3";
            this.rbScope.Text = "Scope";
            this.rbScope.UseVisualStyleBackColor = true;
            // 
            // rbActions
            // 
            this.rbActions.AutoSize = true;
            this.rbActions.Location = new System.Drawing.Point(342, 2);
            this.rbActions.Margin = new System.Windows.Forms.Padding(2);
            this.rbActions.Name = "rbActions";
            this.rbActions.Size = new System.Drawing.Size(60, 16);
            this.rbActions.TabIndex = 10;
            this.rbActions.Tag = "2";
            this.rbActions.Text = "Actions";
            this.rbActions.UseVisualStyleBackColor = true;
            // 
            // rbShadow
            // 
            this.rbShadow.AutoSize = true;
            this.rbShadow.Location = new System.Drawing.Point(172, 2);
            this.rbShadow.Margin = new System.Windows.Forms.Padding(2);
            this.rbShadow.Name = "rbShadow";
            this.rbShadow.Size = new System.Drawing.Size(64, 16);
            this.rbShadow.TabIndex = 9;
            this.rbShadow.Tag = "1";
            this.rbShadow.Text = "Shadow";
            this.rbShadow.UseVisualStyleBackColor = true;
            // 
            // rbSettings
            // 
            this.rbSettings.AutoSize = true;
            this.rbSettings.Checked = true;
            this.rbSettings.Location = new System.Drawing.Point(2, 2);
            this.rbSettings.Margin = new System.Windows.Forms.Padding(2);
            this.rbSettings.Name = "rbSettings";
            this.rbSettings.Size = new System.Drawing.Size(63, 16);
            this.rbSettings.TabIndex = 8;
            this.rbSettings.TabStop = true;
            this.rbSettings.Tag = "0";
            this.rbSettings.Text = "Settings";
            this.rbSettings.UseVisualStyleBackColor = true;
            // 
            // kowhaiTreeScope
            // 
            this.kowhaiTreeScope.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.kowhaiTreeScope.ContextMenuEnabled = true;
            this.kowhaiTreeScope.Location = new System.Drawing.Point(0, 2);
            this.kowhaiTreeScope.Margin = new System.Windows.Forms.Padding(2);
            this.kowhaiTreeScope.Name = "kowhaiTreeScope";
            this.kowhaiTreeScope.Size = new System.Drawing.Size(169, 309);
            this.kowhaiTreeScope.TabIndex = 4;
            // 
            // kowhaiTreeActions
            // 
            this.kowhaiTreeActions.ContextMenuEnabled = true;
            this.kowhaiTreeActions.Dock = System.Windows.Forms.DockStyle.Fill;
            this.kowhaiTreeActions.Location = new System.Drawing.Point(342, 22);
            this.kowhaiTreeActions.Margin = new System.Windows.Forms.Padding(2);
            this.kowhaiTreeActions.Name = "kowhaiTreeActions";
            this.kowhaiTreeActions.Size = new System.Drawing.Size(166, 394);
            this.kowhaiTreeActions.TabIndex = 3;
            // 
            // kowhaiTreeShadow
            // 
            this.kowhaiTreeShadow.ContextMenuEnabled = true;
            this.kowhaiTreeShadow.Dock = System.Windows.Forms.DockStyle.Fill;
            this.kowhaiTreeShadow.Location = new System.Drawing.Point(172, 22);
            this.kowhaiTreeShadow.Margin = new System.Windows.Forms.Padding(2);
            this.kowhaiTreeShadow.Name = "kowhaiTreeShadow";
            this.kowhaiTreeShadow.Size = new System.Drawing.Size(166, 394);
            this.kowhaiTreeShadow.TabIndex = 2;
            // 
            // kowhaiTreeSettings
            // 
            this.kowhaiTreeSettings.ContextMenuEnabled = true;
            this.kowhaiTreeSettings.Dock = System.Windows.Forms.DockStyle.Fill;
            this.kowhaiTreeSettings.Location = new System.Drawing.Point(2, 22);
            this.kowhaiTreeSettings.Margin = new System.Windows.Forms.Padding(2);
            this.kowhaiTreeSettings.Name = "kowhaiTreeSettings";
            this.kowhaiTreeSettings.Size = new System.Drawing.Size(166, 394);
            this.kowhaiTreeSettings.TabIndex = 1;
            // 
            // btnDiff
            // 
            this.btnDiff.Location = new System.Drawing.Point(222, 9);
            this.btnDiff.Margin = new System.Windows.Forms.Padding(2);
            this.btnDiff.Name = "btnDiff";
            this.btnDiff.Size = new System.Drawing.Size(117, 21);
            this.btnDiff.TabIndex = 13;
            this.btnDiff.Text = "Diff Settings To..";
            this.btnDiff.UseVisualStyleBackColor = true;
            this.btnDiff.Click += new System.EventHandler(this.btnDiff_Click);
            // 
            // tableLayoutPanel1
            // 
            this.tableLayoutPanel1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.tableLayoutPanel1.ColumnCount = 4;
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.tableLayoutPanel1.Controls.Add(this.rbSettings, 0, 0);
            this.tableLayoutPanel1.Controls.Add(this.rbShadow, 1, 0);
            this.tableLayoutPanel1.Controls.Add(this.rbActions, 2, 0);
            this.tableLayoutPanel1.Controls.Add(this.rbScope, 3, 0);
            this.tableLayoutPanel1.Controls.Add(this.kowhaiTreeSettings, 0, 1);
            this.tableLayoutPanel1.Controls.Add(this.kowhaiTreeShadow, 1, 1);
            this.tableLayoutPanel1.Controls.Add(this.kowhaiTreeActions, 2, 1);
            this.tableLayoutPanel1.Controls.Add(this.panel1, 3, 1);
            this.tableLayoutPanel1.Location = new System.Drawing.Point(12, 35);
            this.tableLayoutPanel1.Name = "tableLayoutPanel1";
            this.tableLayoutPanel1.RowCount = 2;
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel1.Size = new System.Drawing.Size(681, 418);
            this.tableLayoutPanel1.TabIndex = 14;
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.pnlScope);
            this.panel1.Controls.Add(this.kowhaiTreeScope);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel1.Location = new System.Drawing.Point(510, 20);
            this.panel1.Margin = new System.Windows.Forms.Padding(0);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(171, 398);
            this.panel1.TabIndex = 12;
            // 
            // pnlScope
            // 
            this.pnlScope.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.pnlScope.Location = new System.Drawing.Point(3, 316);
            this.pnlScope.Name = "pnlScope";
            this.pnlScope.Size = new System.Drawing.Size(165, 79);
            this.pnlScope.TabIndex = 5;
            this.pnlScope.Paint += new System.Windows.Forms.PaintEventHandler(this.OnPaint);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(705, 465);
            this.Controls.Add(this.tableLayoutPanel1);
            this.Controls.Add(this.btnDiff);
            this.Controls.Add(this.btnLoad);
            this.Controls.Add(this.btnSave);
            this.Controls.Add(this.btnMerge);
            this.Controls.Add(this.btnRefreshTrees);
            this.Margin = new System.Windows.Forms.Padding(2);
            this.Name = "MainForm";
            this.Text = "MainForm";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.MainForm_FormClosed);
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.tableLayoutPanel1.ResumeLayout(false);
            this.tableLayoutPanel1.PerformLayout();
            this.panel1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button btnRefreshTrees;
        private kowhai_sharp.KowhaiTree kowhaiTreeSettings;
        private kowhai_sharp.KowhaiTree kowhaiTreeShadow;
        private kowhai_sharp.KowhaiTree kowhaiTreeActions;
        private kowhai_sharp.KowhaiTree kowhaiTreeScope;
        private System.Windows.Forms.Button btnSave;
        private System.Windows.Forms.Button btnLoad;
        private System.Windows.Forms.RadioButton rbScope;
        private System.Windows.Forms.RadioButton rbActions;
        private System.Windows.Forms.RadioButton rbShadow;
        private System.Windows.Forms.RadioButton rbSettings;
        private System.Windows.Forms.Button btnMerge;
        private System.Windows.Forms.Button btnDiff;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Panel pnlScope;
    }
}
