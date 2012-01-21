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
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea3 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Series series3 = new System.Windows.Forms.DataVisualization.Charting.Series();
            this.btnRefreshTrees = new System.Windows.Forms.Button();
            this.chart1 = new System.Windows.Forms.DataVisualization.Charting.Chart();
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
            ((System.ComponentModel.ISupportInitialize)(this.chart1)).BeginInit();
            this.SuspendLayout();
            // 
            // btnRefreshTrees
            // 
            this.btnRefreshTrees.Location = new System.Drawing.Point(12, 11);
            this.btnRefreshTrees.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.btnRefreshTrees.Name = "btnRefreshTrees";
            this.btnRefreshTrees.Size = new System.Drawing.Size(116, 26);
            this.btnRefreshTrees.TabIndex = 0;
            this.btnRefreshTrees.Text = "Refresh Trees";
            this.btnRefreshTrees.UseVisualStyleBackColor = true;
            this.btnRefreshTrees.Click += new System.EventHandler(this.btnRefreshTrees_Click);
            // 
            // chart1
            // 
            chartArea3.AxisX.Enabled = System.Windows.Forms.DataVisualization.Charting.AxisEnabled.False;
            chartArea3.AxisY.Enabled = System.Windows.Forms.DataVisualization.Charting.AxisEnabled.False;
            chartArea3.Name = "ChartArea1";
            this.chart1.ChartAreas.Add(chartArea3);
            this.chart1.Location = new System.Drawing.Point(729, 460);
            this.chart1.Name = "chart1";
            series3.ChartArea = "ChartArea1";
            series3.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.FastLine;
            series3.Name = "Series1";
            this.chart1.Series.Add(series3);
            this.chart1.Size = new System.Drawing.Size(200, 100);
            this.chart1.TabIndex = 5;
            this.chart1.Text = "chart1";
            // 
            // btnSave
            // 
            this.btnSave.Location = new System.Drawing.Point(134, 11);
            this.btnSave.Name = "btnSave";
            this.btnSave.Size = new System.Drawing.Size(75, 26);
            this.btnSave.TabIndex = 6;
            this.btnSave.Text = "Save";
            this.btnSave.UseVisualStyleBackColor = true;
            this.btnSave.Click += new System.EventHandler(this.btnSave_Click);
            // 
            // btnLoad
            // 
            this.btnLoad.Location = new System.Drawing.Point(215, 11);
            this.btnLoad.Name = "btnLoad";
            this.btnLoad.Size = new System.Drawing.Size(75, 26);
            this.btnLoad.TabIndex = 7;
            this.btnLoad.Text = "Load";
            this.btnLoad.UseVisualStyleBackColor = true;
            this.btnLoad.Click += new System.EventHandler(this.btnLoad_Click);
            // 
            // btnMerge
            // 
            this.btnMerge.Location = new System.Drawing.Point(458, 11);
            this.btnMerge.Name = "btnMerge";
            this.btnMerge.Size = new System.Drawing.Size(156, 26);
            this.btnMerge.TabIndex = 12;
            this.btnMerge.Text = "Merge Settings To..";
            this.btnMerge.UseVisualStyleBackColor = true;
            this.btnMerge.Click += new System.EventHandler(this.btnMerge_Click);
            // 
            // rbScope
            // 
            this.rbScope.AutoSize = true;
            this.rbScope.Location = new System.Drawing.Point(788, 43);
            this.rbScope.Name = "rbScope";
            this.rbScope.Size = new System.Drawing.Size(69, 21);
            this.rbScope.TabIndex = 11;
            this.rbScope.Tag = "3";
            this.rbScope.Text = "Scope";
            this.rbScope.UseVisualStyleBackColor = true;
            // 
            // rbActions
            // 
            this.rbActions.AutoSize = true;
            this.rbActions.Location = new System.Drawing.Point(585, 43);
            this.rbActions.Name = "rbActions";
            this.rbActions.Size = new System.Drawing.Size(75, 21);
            this.rbActions.TabIndex = 10;
            this.rbActions.Tag = "2";
            this.rbActions.Text = "Actions";
            this.rbActions.UseVisualStyleBackColor = true;
            // 
            // rbShadow
            // 
            this.rbShadow.AutoSize = true;
            this.rbShadow.Location = new System.Drawing.Point(373, 43);
            this.rbShadow.Name = "rbShadow";
            this.rbShadow.Size = new System.Drawing.Size(79, 21);
            this.rbShadow.TabIndex = 9;
            this.rbShadow.Tag = "1";
            this.rbShadow.Text = "Shadow";
            this.rbShadow.UseVisualStyleBackColor = true;
            // 
            // rbSettings
            // 
            this.rbSettings.AutoSize = true;
            this.rbSettings.Checked = true;
            this.rbSettings.Location = new System.Drawing.Point(120, 43);
            this.rbSettings.Name = "rbSettings";
            this.rbSettings.Size = new System.Drawing.Size(80, 21);
            this.rbSettings.TabIndex = 8;
            this.rbSettings.TabStop = true;
            this.rbSettings.Tag = "0";
            this.rbSettings.Text = "Settings";
            this.rbSettings.UseVisualStyleBackColor = true;
            // 
            // kowhaiTreeScope
            // 
            this.kowhaiTreeScope.ContextMenuEnabled = true;
            this.kowhaiTreeScope.Location = new System.Drawing.Point(729, 69);
            this.kowhaiTreeScope.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.kowhaiTreeScope.Name = "kowhaiTreeScope";
            this.kowhaiTreeScope.Size = new System.Drawing.Size(200, 386);
            this.kowhaiTreeScope.TabIndex = 4;
            // 
            // kowhaiTreeActions
            // 
            this.kowhaiTreeActions.ContextMenuEnabled = true;
            this.kowhaiTreeActions.Location = new System.Drawing.Point(523, 69);
            this.kowhaiTreeActions.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.kowhaiTreeActions.Name = "kowhaiTreeActions";
            this.kowhaiTreeActions.Size = new System.Drawing.Size(200, 494);
            this.kowhaiTreeActions.TabIndex = 3;
            // 
            // kowhaiTreeShadow
            // 
            this.kowhaiTreeShadow.ContextMenuEnabled = true;
            this.kowhaiTreeShadow.Location = new System.Drawing.Point(317, 69);
            this.kowhaiTreeShadow.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.kowhaiTreeShadow.Name = "kowhaiTreeShadow";
            this.kowhaiTreeShadow.Size = new System.Drawing.Size(200, 494);
            this.kowhaiTreeShadow.TabIndex = 2;
            // 
            // kowhaiTreeSettings
            // 
            this.kowhaiTreeSettings.ContextMenuEnabled = true;
            this.kowhaiTreeSettings.Location = new System.Drawing.Point(12, 69);
            this.kowhaiTreeSettings.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.kowhaiTreeSettings.Name = "kowhaiTreeSettings";
            this.kowhaiTreeSettings.Size = new System.Drawing.Size(299, 494);
            this.kowhaiTreeSettings.TabIndex = 1;
            // 
            // btnDiff
            // 
            this.btnDiff.Location = new System.Drawing.Point(296, 11);
            this.btnDiff.Name = "btnDiff";
            this.btnDiff.Size = new System.Drawing.Size(156, 26);
            this.btnDiff.TabIndex = 13;
            this.btnDiff.Text = "Diff Settings To..";
            this.btnDiff.UseVisualStyleBackColor = true;
            this.btnDiff.Click += new System.EventHandler(this.btnDiff_Click);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(940, 572);
            this.Controls.Add(this.btnDiff);
            this.Controls.Add(this.btnLoad);
            this.Controls.Add(this.btnSave);
            this.Controls.Add(this.rbShadow);
            this.Controls.Add(this.btnMerge);
            this.Controls.Add(this.rbActions);
            this.Controls.Add(this.rbScope);
            this.Controls.Add(this.chart1);
            this.Controls.Add(this.kowhaiTreeScope);
            this.Controls.Add(this.kowhaiTreeActions);
            this.Controls.Add(this.rbSettings);
            this.Controls.Add(this.kowhaiTreeShadow);
            this.Controls.Add(this.kowhaiTreeSettings);
            this.Controls.Add(this.btnRefreshTrees);
            this.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.Name = "MainForm";
            this.Text = "MainForm";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.MainForm_FormClosed);
            this.Load += new System.EventHandler(this.MainForm_Load);
            ((System.ComponentModel.ISupportInitialize)(this.chart1)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnRefreshTrees;
        private kowhai_sharp.KowhaiTree kowhaiTreeSettings;
        private kowhai_sharp.KowhaiTree kowhaiTreeShadow;
        private kowhai_sharp.KowhaiTree kowhaiTreeActions;
        private kowhai_sharp.KowhaiTree kowhaiTreeScope;
        private System.Windows.Forms.DataVisualization.Charting.Chart chart1;
        private System.Windows.Forms.Button btnSave;
        private System.Windows.Forms.Button btnLoad;
        private System.Windows.Forms.RadioButton rbScope;
        private System.Windows.Forms.RadioButton rbActions;
        private System.Windows.Forms.RadioButton rbShadow;
        private System.Windows.Forms.RadioButton rbSettings;
        private System.Windows.Forms.Button btnMerge;
        private System.Windows.Forms.Button btnDiff;
    }
}
