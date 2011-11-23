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
        Sock sock;
        const int PACKET_SIZE = 64;
        Kowhai.kowhai_node_t[] descriptor;

        public MainForm()
        {
            InitializeComponent();
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
            sock = new Sock();
            if (sock.Connect())
            {
                button1.Enabled = true;
                sock.SockBufferReceived += new SockReceiveEventHandler(sock_SockBufferReceived);
                sock.StartAsyncReceives(new byte[PACKET_SIZE], PACKET_SIZE);
                kowhaiTree1.DataChange += new KowhaiTree.DataChangeEventHandler(kowhaiTree1_DataChange);
            }
            else
                button1.Enabled = false;
        }

        private void MainForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            if (button1.Enabled)
                sock.Disconnect();
        }

        void sock_SockBufferReceived(object sender, SockReceiveEventArgs e)
        {
            KowhaiProtocol.kowhai_protocol_t prot;
            Kowhai.kowhai_symbol_t[] symbols;
            if (KowhaiProtocol.Parse(e.Buffer, e.Size, out prot, out symbols) == Kowhai.STATUS_OK)
            {
                switch (prot.header.command)
                {
                    case KowhaiProtocol.CMD_READ_DATA_ACK:
                    case KowhaiProtocol.CMD_WRITE_DATA_ACK:
                    case KowhaiProtocol.CMD_READ_DATA_ACK_END:
                        byte[] data = KowhaiProtocol.GetBuffer(prot);
                        int nodeOffset;
                        Kowhai.kowhai_node_t node;
                        if (Kowhai.GetNode(descriptor, symbols, out nodeOffset, out node) == Kowhai.STATUS_OK)
                            this.Invoke((MethodInvoker)delegate { kowhaiTree1.UpdateData(data, nodeOffset + prot.payload.spec.data.memory.offset); });
                        break;
                    case KowhaiProtocol.CMD_READ_DESCRIPTOR_ACK:
                        descriptor = new Kowhai.kowhai_node_t[prot.payload.spec.descriptor.node_count];
                        KowhaiProtocol.CopyDescriptor(descriptor, prot.payload);
                        break;
                    case KowhaiProtocol.CMD_READ_DESCRIPTOR_ACK_END:
                        KowhaiProtocol.CopyDescriptor(descriptor, prot.payload);
                        this.Invoke((MethodInvoker)delegate{ kowhaiTree1.UpdateDescriptor(descriptor, KowhaiSymbols.Symbols.Strings); });

                        byte[] buffer = new byte[PACKET_SIZE];
                        int bytesRequired;
                        prot.header.command = KowhaiProtocol.CMD_READ_DATA;
                        if (KowhaiProtocol.Create(buffer, PACKET_SIZE, ref prot,
                            new Kowhai.kowhai_symbol_t[] {new Kowhai.kowhai_symbol_t((uint)KowhaiSymbols.Symbols.Constants.General)},
                            out bytesRequired) == Kowhai.STATUS_OK)
                            sock.Send(buffer, bytesRequired);
                        break;
                }
            }
        }

        void kowhaiTree1_DataChange(object sender, KowhaiTree.DataChangeEventArgs e)
        {
            byte[] buffer = new byte[PACKET_SIZE];
            List<ushort> arrayIndexes = new List<ushort>();
            KowhaiTree.KowhaiNodeInfo info = e.Info;
            while (info != null)
            {
                arrayIndexes.Add(info.ArrayIndex);
                info = info.Parent;
            }
            arrayIndexes.Reverse();
            Kowhai.kowhai_symbol_t[] symbols = Kowhai.GetSymbolPath(descriptor, e.Info.KowhaiNode, e.Info.NodeIndex, arrayIndexes.ToArray());
            KowhaiProtocol.kowhai_protocol_t prot = new KowhaiProtocol.kowhai_protocol_t();
            prot.header.tree_id = 0;
            prot.header.command = KowhaiProtocol.CMD_WRITE_DATA;
            int bytesRequired;
            KowhaiProtocol.Create(buffer, PACKET_SIZE, ref prot, symbols, e.Buffer, 0, out bytesRequired);
            sock.Send(buffer, bytesRequired);
        }

        private void button1_Click(object sender, EventArgs e)
        {
            byte[] buffer = new byte[2];
            buffer[0] = 0;
            buffer[1] = KowhaiProtocol.CMD_READ_DESCRIPTOR;
            sock.Send(buffer, 2);
        }
    }
}
