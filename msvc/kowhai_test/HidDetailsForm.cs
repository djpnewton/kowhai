using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace kowhai_test
{
    public partial class HidDetailsForm : Form
    {
        public ushort VendorId
        {
            get { return Convert.ToUInt16(tbVendorId.Text, 16); }
            set { tbVendorId.Text = String.Format("{0:X}", value); }
        }
        public ushort ProductId
        {
            get { return Convert.ToUInt16(tbProductId.Text, 16); }
            set { tbProductId.Text = String.Format("{0:X}", value); }
        }
        public ushort UsagePage
        {
            get { return Convert.ToUInt16(tbUsagePage.Text, 16); }
            set { tbUsagePage.Text = String.Format("{0:X}", value); }
        }
        public ushort Usage
        {
            get { return Convert.ToUInt16(tbUsage.Text, 16); }
            set { tbUsage.Text = String.Format("{0:X}", value); }
        }
        public byte ReportId
        {
            get { return Convert.ToByte(tbReportId.Text, 16); }
            set { tbReportId.Text = String.Format("{0:X}", value); }
        }
        public byte[] ReportPrefix
        {
            get
            {
                byte[] result = new byte[tbReportPrefix.Text.Length / 2];
                for (int i = 0; i < tbReportPrefix.Text.Length / 2; i++)
                {
                    string text = tbReportPrefix.Text.Substring(i * 2, 2);
                    result[i] = Convert.ToByte(text, 16);
                }
                return result;
            }
            set
            {
                StringBuilder text = new StringBuilder(value.Length * 2);
                for (int i = 0; i < value.Length; i++)
                    text.Append(String.Format("{0:X}", value[i]));
                tbReportPrefix.Text = text.ToString();
            }
        }
        public int ReportSize
        {
            get { return Convert.ToInt32(tbReportSize.Text, 16); }
            set { tbReportSize.Text = String.Format("{0:X}", value); }
        }

        public HidDetailsForm()
        {
            InitializeComponent();
        }
    }
}
