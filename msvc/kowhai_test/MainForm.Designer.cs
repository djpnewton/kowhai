﻿namespace kowhai_test
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
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea1 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Series series1 = new System.Windows.Forms.DataVisualization.Charting.Series();
            this.btnRefreshTrees = new System.Windows.Forms.Button();
            this.kowhaiTreeScope = new kowhai_sharp.KowhaiTree();
            this.kowhaiTreeActions = new kowhai_sharp.KowhaiTree();
            this.kowhaiTreeShadow = new kowhai_sharp.KowhaiTree();
            this.kowhaiTreeSettings = new kowhai_sharp.KowhaiTree();
            this.chart1 = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.btnSave = new System.Windows.Forms.Button();
            this.btnLoad = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.chart1)).BeginInit();
            this.SuspendLayout();
            // 
            // btnRefreshTrees
            // 
            this.btnRefreshTrees.Location = new System.Drawing.Point(12, 12);
            this.btnRefreshTrees.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.btnRefreshTrees.Name = "btnRefreshTrees";
            this.btnRefreshTrees.Size = new System.Drawing.Size(116, 34);
            this.btnRefreshTrees.TabIndex = 0;
            this.btnRefreshTrees.Text = "Refresh Trees";
            this.btnRefreshTrees.UseVisualStyleBackColor = true;
            this.btnRefreshTrees.Click += new System.EventHandler(this.btnRefreshTrees_Click);
            // 
            // kowhaiTreeScope
            // 
            this.kowhaiTreeScope.ContextMenuEnabled = true;
            this.kowhaiTreeScope.Location = new System.Drawing.Point(726, 52);
            this.kowhaiTreeScope.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.kowhaiTreeScope.Name = "kowhaiTreeScope";
            this.kowhaiTreeScope.Size = new System.Drawing.Size(233, 403);
            this.kowhaiTreeScope.TabIndex = 4;
            // 
            // kowhaiTreeActions
            // 
            this.kowhaiTreeActions.ContextMenuEnabled = true;
            this.kowhaiTreeActions.Location = new System.Drawing.Point(487, 52);
            this.kowhaiTreeActions.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.kowhaiTreeActions.Name = "kowhaiTreeActions";
            this.kowhaiTreeActions.Size = new System.Drawing.Size(233, 511);
            this.kowhaiTreeActions.TabIndex = 3;
            // 
            // kowhaiTreeShadow
            // 
            this.kowhaiTreeShadow.ContextMenuEnabled = true;
            this.kowhaiTreeShadow.Location = new System.Drawing.Point(250, 52);
            this.kowhaiTreeShadow.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.kowhaiTreeShadow.Name = "kowhaiTreeShadow";
            this.kowhaiTreeShadow.Size = new System.Drawing.Size(231, 511);
            this.kowhaiTreeShadow.TabIndex = 2;
            // 
            // kowhaiTreeSettings
            // 
            this.kowhaiTreeSettings.ContextMenuEnabled = true;
            this.kowhaiTreeSettings.Location = new System.Drawing.Point(12, 52);
            this.kowhaiTreeSettings.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.kowhaiTreeSettings.Name = "kowhaiTreeSettings";
            this.kowhaiTreeSettings.Size = new System.Drawing.Size(232, 511);
            this.kowhaiTreeSettings.TabIndex = 1;
            // 
            // chart1
            // 
            chartArea1.AxisX.Enabled = System.Windows.Forms.DataVisualization.Charting.AxisEnabled.False;
            chartArea1.AxisY.Enabled = System.Windows.Forms.DataVisualization.Charting.AxisEnabled.False;
            chartArea1.Name = "ChartArea1";
            this.chart1.ChartAreas.Add(chartArea1);
            this.chart1.Location = new System.Drawing.Point(726, 460);
            this.chart1.Name = "chart1";
            series1.ChartArea = "ChartArea1";
            series1.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.FastLine;
            series1.Name = "Series1";
            this.chart1.Series.Add(series1);
            this.chart1.Size = new System.Drawing.Size(233, 100);
            this.chart1.TabIndex = 5;
            this.chart1.Text = "chart1";
            // 
            // btnSave
            // 
            this.btnSave.Location = new System.Drawing.Point(487, 12);
            this.btnSave.Name = "btnSave";
            this.btnSave.Size = new System.Drawing.Size(75, 34);
            this.btnSave.TabIndex = 6;
            this.btnSave.Text = "Save";
            this.btnSave.UseVisualStyleBackColor = true;
            this.btnSave.Click += new System.EventHandler(this.btnSave_Click);
            // 
            // btnLoad
            // 
            this.btnLoad.Location = new System.Drawing.Point(568, 12);
            this.btnLoad.Name = "btnLoad";
            this.btnLoad.Size = new System.Drawing.Size(75, 34);
            this.btnLoad.TabIndex = 7;
            this.btnLoad.Text = "Load";
            this.btnLoad.UseVisualStyleBackColor = true;
            this.btnLoad.Click += new System.EventHandler(this.btnLoad_Click);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(966, 572);
            this.Controls.Add(this.btnLoad);
            this.Controls.Add(this.btnSave);
            this.Controls.Add(this.chart1);
            this.Controls.Add(this.kowhaiTreeScope);
            this.Controls.Add(this.kowhaiTreeActions);
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
    }
}