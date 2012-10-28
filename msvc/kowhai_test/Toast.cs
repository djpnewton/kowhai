using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
using System.Windows.Forms;

namespace kowhai_test
{

    public class Toast : Panel
    {
        Timer timer;
        Form parent;
        string message;
        static int refCount = 0;
        static Point lastPosition = new Point(-1, -1);

        public event EventHandler Closed;
        public static int MaxToasts = 10;

        public Toast(Form parent, string message, int timeout)
        {
            // Create timer for close timeout
            timer = new Timer();
            timer.Interval = timeout;
            timer.Tick += timer_Tick;
            if (MaxToasts != 0 && refCount < MaxToasts)
            {
                this.parent = parent;
                this.message = message;
                // customize look and position
                BackColor = Color.LightYellow;
                BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
                DoToastLayout();
            }
            else
            {
                Visible = false;
                timer.Interval = 1;
            }
            timer.Start();
            refCount++;
        }

        protected void DoToastLayout()
        {
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
                if (Bottom > parent.ClientSize.Height)
                    Top = parent.ClientSize.Height - Height;
            }
            else
            {
                Left = parent.ClientSize.Width / 2 - Width / 2;
                Top = parent.ClientSize.Height / 2 - Height / 2;
            }
            // update last position
            lastPosition = new Point(Left, Bottom);
            // add refcount indicator
            lb = new Label();
            lb.AutoSize = true;
            lb.Text = refCount.ToString();
            lb.Font = new Font(FontFamily.GenericMonospace, 6);
            lb.ForeColor = Color.LightGray;
            lb.Left = 2;
            lb.Top = Height - 15;
            Controls.Add(lb);
            lb.BringToFront();
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

        protected void Close()
        {
            Hide();
            timer.Stop();
            refCount--;
            if (refCount == 0)
                lastPosition = new Point(-1, -1);
            if (Closed != null)
                Closed(this, new EventArgs());
        }
    }
}
