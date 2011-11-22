using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using kowhai_sharp;

namespace kowhai_test
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            const int SIZE = 64;
            Sock sock = new Sock();
            if (sock.Connect())
            {
                byte[] buffer = new byte[SIZE];
                buffer[0] = 0;
                buffer[1] = KowhaiProtocol.CMD_READ_DESCRIPTOR;
                int size = sock.Send(buffer, SIZE);
                size = sock.Receive(buffer, SIZE);
                if (size > 0)
                {
                    KowhaiProtocol.kowhai_protocol_t prot;
                    if (KowhaiProtocol.Parse(buffer, size, out prot) == Kowhai.STATUS_OK)
                    {
                        if (prot.header.command == KowhaiProtocol.CMD_READ_DESCRIPTOR_ACK)
                        {
                            Kowhai.kowhai_node_t[] descriptor = new Kowhai.kowhai_node_t[prot.payload.spec.descriptor.node_count];
                            KowhaiProtocol.CopyDescriptor(descriptor, prot.payload);
                            size = sock.Receive(buffer, SIZE);
                            if (size > 0)
                            {
                                if (KowhaiProtocol.Parse(buffer, size, out prot) == Kowhai.STATUS_OK)
                                {
                                    if (prot.header.command == KowhaiProtocol.CMD_READ_DESCRIPTOR_ACK_END)
                                    {
                                        KowhaiProtocol.CopyDescriptor(descriptor, prot.payload);
                                        kowhaiTree1.Update(descriptor);
                                    }
                                }
                            }
                        }
                    }
                }
                sock.Disconnect();
            }
        }
    }
}
