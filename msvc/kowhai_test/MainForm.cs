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
        const int TREE_ID_SETTINGS = 0;
        const int TREE_ID_SHADOW = 1;
        const int TREE_ID_ACTIONS = 2;

        Sock sock;
        const int PACKET_SIZE = 64;
        List<Kowhai.kowhai_node_t[]> descriptors = new List<Kowhai.kowhai_node_t[]>();

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
                kowhaiTreeSettings.DataChange += new KowhaiTree.DataChangeEventHandler(kowhaiTree_DataChange);
                kowhaiTreeShadow.DataChange += new KowhaiTree.DataChangeEventHandler(kowhaiTree_DataChange);
                kowhaiTreeActions.DataChange += new KowhaiTree.DataChangeEventHandler(kowhaiTree_DataChange);
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
                while (prot.header.tree_id > descriptors.Count - 1)
                    descriptors.Add(null);
                Kowhai.kowhai_node_t[] descriptor = descriptors[prot.header.tree_id];

                switch (prot.header.command)
                {
                    case KowhaiProtocol.CMD_READ_DATA_ACK:
                    case KowhaiProtocol.CMD_WRITE_DATA_ACK:
                    case KowhaiProtocol.CMD_READ_DATA_ACK_END:
                        byte[] data = KowhaiProtocol.GetBuffer(prot);
                        int nodeOffset;
                        Kowhai.kowhai_node_t node;
                        if (Kowhai.GetNode(descriptor, symbols, out nodeOffset, out node) == Kowhai.STATUS_OK)
                            this.Invoke((MethodInvoker)delegate { GetKowhaiTree(prot.header.tree_id).UpdateData(data, nodeOffset + prot.payload.spec.data.memory.offset); });
                        break;
                    case KowhaiProtocol.CMD_READ_DESCRIPTOR_ACK:
                    case KowhaiProtocol.CMD_READ_DESCRIPTOR_ACK_END:
                        if (descriptor == null || descriptor.Length < prot.payload.spec.descriptor.node_count)
                        {
                            Array.Resize<Kowhai.kowhai_node_t>(ref descriptor, prot.payload.spec.descriptor.node_count);
                            descriptors[prot.header.tree_id] = descriptor;
                        }
                        KowhaiProtocol.CopyDescriptor(descriptor, prot.payload);

                        if (prot.header.command == KowhaiProtocol.CMD_READ_DESCRIPTOR_ACK_END)
                        {
                            this.Invoke((MethodInvoker)delegate { GetKowhaiTree(prot.header.tree_id).UpdateDescriptor(descriptor, KowhaiSymbols.Symbols.Strings); });

                            byte[] buffer = new byte[PACKET_SIZE];
                            int bytesRequired;
                            prot.header.command = KowhaiProtocol.CMD_READ_DATA;
                            if (KowhaiProtocol.Create(buffer, PACKET_SIZE, ref prot,
                                GetRootSymbolPath(prot.header.tree_id),
                                out bytesRequired) == Kowhai.STATUS_OK)
                                sock.Send(buffer, bytesRequired);
                        }
                        break;
                }
            }
        }

        void kowhaiTree_DataChange(object sender, KowhaiTree.DataChangeEventArgs e)
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
            Kowhai.kowhai_symbol_t[] symbols = Kowhai.GetSymbolPath(GetDescriptor(sender), e.Info.KowhaiNode, e.Info.NodeIndex, arrayIndexes.ToArray());
            KowhaiProtocol.kowhai_protocol_t prot = new KowhaiProtocol.kowhai_protocol_t();
            prot.header.tree_id = GetTreeId(sender);
            prot.header.command = KowhaiProtocol.CMD_WRITE_DATA;
            int bytesRequired;
            KowhaiProtocol.Create(buffer, PACKET_SIZE, ref prot, symbols, e.Buffer, 0, out bytesRequired);
            sock.Send(buffer, bytesRequired);
        }

        private void button1_Click(object sender, EventArgs e)
        {
            byte[] buffer = new byte[2];
            buffer[0] = TREE_ID_SETTINGS;
            buffer[1] = KowhaiProtocol.CMD_READ_DESCRIPTOR;
            sock.Send(buffer, 2);
            System.Threading.Thread.Sleep(100);
            Application.DoEvents();
            buffer[0] = TREE_ID_SHADOW;
            sock.Send(buffer, 2);
            System.Threading.Thread.Sleep(100);
            Application.DoEvents();
            buffer[0] = TREE_ID_ACTIONS;
            sock.Send(buffer, 2);
        }

        private Kowhai.kowhai_symbol_t[] GetRootSymbolPath(byte treeId)
        {
            if (treeId == TREE_ID_SETTINGS)
                return new Kowhai.kowhai_symbol_t[] { new Kowhai.kowhai_symbol_t((uint)KowhaiSymbols.Symbols.Constants.Settings) };
            if (treeId == TREE_ID_SHADOW)
                return new Kowhai.kowhai_symbol_t[] { new Kowhai.kowhai_symbol_t((uint)KowhaiSymbols.Symbols.Constants.Shadow) };
            if (treeId == TREE_ID_ACTIONS)
                return new Kowhai.kowhai_symbol_t[] { new Kowhai.kowhai_symbol_t((uint)KowhaiSymbols.Symbols.Constants.Actions) };
            return null;
        }

        private KowhaiTree GetKowhaiTree(byte treeId)
        {
            if (treeId == TREE_ID_SETTINGS)
                return kowhaiTreeSettings;
            if (treeId == TREE_ID_SHADOW)
                return kowhaiTreeShadow;
            if (treeId == TREE_ID_ACTIONS)
                return kowhaiTreeActions;
            return null;
        }

        private byte GetTreeId(object sender)
        {
            if (sender == kowhaiTreeSettings)
                return TREE_ID_SETTINGS;
            if (sender == kowhaiTreeShadow)
                return TREE_ID_SHADOW;
            if (sender == kowhaiTreeActions)
                return TREE_ID_ACTIONS;
            return 255;
        }

        private Kowhai.kowhai_node_t[] GetDescriptor(object sender)
        {
            return descriptors[GetTreeId(sender)];
        }
    }
}
