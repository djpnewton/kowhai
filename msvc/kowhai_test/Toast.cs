using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
using System.Windows.Forms;

namespace kowhai_test
{

    public class Toast : Form
    {
        Timer timer;
        Form parent;
        string message;
        static int refCount = 0;
        static Point lastPosition = new Point(-1, -1);

        public Toast(Form parent, string message, int timeout)
        {
            this.parent = parent;
            this.message = message;
            // customize form
            BackColor = Color.LightYellow;
            FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            // Create timer for close timeout
            timer = new Timer();
            timer.Interval = timeout;
            timer.Tick += timer_Tick;
        }

        protected override bool ShowWithoutActivation
        {
            get { return true; }
        }

        protected override CreateParams CreateParams
        {
            get
            {
                CreateParams p = base.CreateParams;

                p.Style |= 0x40000000; // WS_CHILD
                p.ExStyle |= 0x8000000; // WS_EX_NOACTIVATE - requires Win 2000 or higher :)
                p.ExStyle |= 0x0000080; // WS_EX_TOOLWINDOW

                return p;
            }
        }

        protected override void OnLoad(EventArgs e)
        {
            base.OnLoad(e);
            // Set message size and position
            int border = 25;
            Label lb = new Label();
            lb.AutoSize = false;
            lb.TextAlign = ContentAlignment.MiddleCenter;
            using (Graphics g = CreateGraphics())
            {
                SizeF size = g.MeasureString(message, lb.Font);
                lb.Width = (int)Math.Ceiling(size.Width) + border * 2;
                lb.Height = (int)Math.Ceiling(size.Height) + border * 2;
            }
            lb.Text = message;
            lb.Click += new EventHandler(l_Click);
            Width = lb.Width;
            Height = lb.Height;
            Controls.Add(lb);
            if (lastPosition.X > -1)
            {
                Left = lastPosition.X;
                Top = lastPosition.Y;
            }
            else
            {
                Left = parent.Left + parent.Width / 2 - Width / 2;
                Top = parent.Top + parent.Height / 2 - Height / 2;
            }
            // update last position
            refCount++;
            lastPosition = new Point(Left, Bottom);
        }

        protected override void OnShown(EventArgs e)
        {
            base.OnShown(e);
            timer.Start();
        }

        protected override void OnClick(EventArgs e)
        {
            Close();
        }

        void timer_Tick(object sender, EventArgs e)
        {
            Close();
        }

        void l_Click(object sender, EventArgs e)
        {
            Close();
        }

        protected override void OnClosed(EventArgs e)
        {
            base.OnClosed(e);
            refCount--;
            if (refCount == 0)
                lastPosition = new Point(-1, -1);
        }
    }
}
