using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Collections;
using kowhai_sharp;

namespace kowhai_test
{
    public partial class MainForm : Form
    {
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
                btnRefreshList.Enabled = true;
                sock.SockBufferReceived += new SockReceiveEventHandler(sock_SockBufferReceived);
                sock.StartAsyncReceives(new byte[PACKET_SIZE], PACKET_SIZE);
                kowhaiTreeMain.DataChange += new KowhaiTree.DataChangeEventHandler(kowhaiTree_DataChange);
                kowhaiTreeMain.NodeRead += new KowhaiTree.NodeReadEventHandler(kowhaiTree_NodeRead);
            }
            else
                btnRefreshList.Enabled = false;
        }

        private void MainForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            if (btnRefreshList.Enabled)
                sock.Disconnect();
        }

        void sock_SockBufferReceived(object sender, SockReceiveEventArgs e)
        {
            byte[] buffer = new byte[e.Size];
            Array.Copy(e.Buffer, buffer, e.Size);
            this.BeginInvoke((MethodInvoker)delegate
            {
                ProcessPacket(buffer);
            });
        }

        byte[] ScopePointData = null;
        List<UInt16> ScopePoints = new List<UInt16>();
        UInt16 ScopeMinVal = UInt16.MaxValue, ScopeMaxVal = UInt16.MinValue;
        private void ProcessPacket(byte[] buffer)
        {
            KowhaiProtocol.kowhai_protocol_t prot;
            Kowhai.kowhai_symbol_t[] symbols;
            int result = KowhaiProtocol.Parse(buffer, buffer.Length, out prot, out symbols);
            if (result == Kowhai.STATUS_OK)
            {
                while (prot.header.id > descriptors.Count - 1)
                    descriptors.Add(null);
                Kowhai.kowhai_node_t[] descriptor = descriptors[prot.header.id];

                switch (prot.header.command)
                {
                    case KowhaiProtocol.CMD_GET_TREE_COUNT_ACK:
                    {
                        InitTreeList(prot.payload.spec.tree_count);

                        buffer = new byte[PACKET_SIZE];
                        int bytesRequired;
                        prot.header.command = KowhaiProtocol.CMD_GET_FUNCTION_LIST;
                        if (KowhaiProtocol.Create(buffer, PACKET_SIZE, ref prot,
                            out bytesRequired) == Kowhai.STATUS_OK)
                            sock.Send(buffer, bytesRequired);
                        break;
                    }
                    case KowhaiProtocol.CMD_READ_DATA_ACK:
                    case KowhaiProtocol.CMD_WRITE_DATA_ACK:
                    case KowhaiProtocol.CMD_READ_DATA_ACK_END:
                        byte[] data = KowhaiProtocol.GetBuffer(prot);
                        int nodeOffset;
                        Kowhai.kowhai_node_t node;
                        if (Kowhai.GetNode(descriptor, symbols, out nodeOffset, out node) == Kowhai.STATUS_OK)
                        {
                            if (!TreeIdOk(prot.header.id))
                                return;
                            KowhaiTree tree = kowhaiTreeMain;
                            tree.UpdateData(data, nodeOffset + prot.payload.spec.data.memory.offset);
                            if (descriptor[0].symbol == (ushort)KowhaiSymbols.Symbols.Constants.Scope)
                            {
                                Kowhai.kowhai_symbol_t[] symbolPath = new Kowhai.kowhai_symbol_t[] {
                                    new Kowhai.kowhai_symbol_t((uint)KowhaiSymbols.Symbols.Constants.Scope),
                                    new Kowhai.kowhai_symbol_t((uint)KowhaiSymbols.Symbols.Constants.Pixels)
                                };
                                if (Kowhai.GetNode(tree.GetDescriptor(), symbolPath, out nodeOffset, out node) == Kowhai.STATUS_OK)
                                {
                                    // update the scope points the the new data
                                    if (ScopePointData == null || ScopePointData.Length != node.count * 2)
                                        Array.Resize(ref ScopePointData, node.count * 2);
                                    int arrayIndex = 0;
                                    if (symbols.Length == 2)
                                        arrayIndex = symbols[1].parts.array_index;
                                    Array.Copy(data, 0, ScopePointData, arrayIndex * 2 + prot.payload.spec.data.memory.offset, data.Length);
                                    for (int i = 0; i < ScopePointData.Length / 2; i++)
                                    {
                                        UInt16 value = BitConverter.ToUInt16(ScopePointData, i * 2);
                                        if (value > ScopeMaxVal)
                                            ScopeMaxVal = value;
                                        if (value < ScopeMinVal)
                                            ScopeMinVal = value;
                                        if (ScopePoints.Count > i)
                                            ScopePoints[i] = value;
                                        else
                                            ScopePoints.Add(value);
                                    }
                                    pnlScope.Invalidate();
                                }
                            }
                        }
                        break;
                    case KowhaiProtocol.CMD_READ_DESCRIPTOR_ACK:
                    case KowhaiProtocol.CMD_READ_DESCRIPTOR_ACK_END:
                        if (descriptor == null || descriptor.Length < prot.payload.spec.descriptor.node_count)
                        {
                            Array.Resize<Kowhai.kowhai_node_t>(ref descriptor, prot.payload.spec.descriptor.node_count);
                            descriptors[prot.header.id] = descriptor;
                        }
                        KowhaiProtocol.CopyDescriptor(descriptor, prot.payload);

                        if (prot.header.command == KowhaiProtocol.CMD_READ_DESCRIPTOR_ACK_END)
                        {
                            if (!TreeIdOk(prot.header.id))
                                return;
                            SetTreeListName(descriptor[0].symbol);
                            kowhaiTreeMain.UpdateDescriptor(descriptor, KowhaiSymbols.Symbols.Strings, null);

                            buffer = new byte[PACKET_SIZE];
                            int bytesRequired;
                            prot.header.command = KowhaiProtocol.CMD_READ_DATA;
                            uint sym = descriptor[0].symbol;
                            Kowhai.kowhai_symbol_t[] syms = new Kowhai.kowhai_symbol_t[] { new Kowhai.kowhai_symbol_t(sym) };
                            if (KowhaiProtocol.Create(buffer, PACKET_SIZE, ref prot,
                                syms,
                                out bytesRequired) == Kowhai.STATUS_OK)
                                sock.Send(buffer, bytesRequired);
                        }
                        break;
                    case KowhaiProtocol.CMD_GET_FUNCTION_LIST_ACK:
                    case KowhaiProtocol.CMD_GET_FUNCTION_LIST_ACK_END:
                        ushort[] funcs = new ushort[prot.payload.spec.function_list.list_count];
                        KowhaiProtocol.CopyFunctionList(funcs, prot.payload);
                        InitFunctionList(funcs);
                        break;
                    default:
                        MessageBox.Show(string.Format("ProcessPacket(): Unknown command ({0})", prot.header.command),
                            "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                        break;
                }
            }
            else
            {
                MessageBox.Show(string.Format("ProcessPacket(): KowhaiProtocol.Parse() = {0}", result),
                    "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }
        private void SetTreeListName(ushort symbol)
        {
            lbTreeList.SelectedIndexChanged -= lbTreeList_SelectedIndexChanged;
            lbTreeList.Items[lbTreeList.SelectedIndex] = KowhaiSymbols.Symbols.Strings[symbol];
            lbTreeList.SelectedIndexChanged += lbTreeList_SelectedIndexChanged;
        }

        private void InitTreeList(byte treeCount)
        {
            lbTreeList.Items.Clear();
            for (int i = 0; i < treeCount; i++)
                lbTreeList.Items.Add(i);
        }

        private void InitFunctionList(ushort[] funcs)
        {
            lbFunctionList.Items.Clear();
            foreach (ushort func in funcs)
                lbFunctionList.Items.Add(KowhaiSymbols.Symbols.Strings[func]);
        }

        private bool TreeIdOk(ushort id)
        {
            if (id != lbTreeList.SelectedIndex)
            {
                MessageBox.Show("prot.header.id != lbTreeList.SelectedIndex", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return false;
            }
            return true;
        }

        private List<ushort> CreateNodeInfoArrayIndexList(KowhaiTree.KowhaiNodeInfo info)
        {
            List<ushort> arrayIndexes = new List<ushort>();
            while (info != null)
            {
                arrayIndexes.Add(info.ArrayIndex);
                info = info.Parent;
            }
            arrayIndexes.Reverse();
            return arrayIndexes;
        }

        void kowhaiTree_DataChange(object sender, KowhaiTree.DataChangeEventArgs e)
        {
            Application.DoEvents();
            System.Threading.Thread.Sleep(250);

            byte[] buffer = new byte[PACKET_SIZE];
            List<ushort> arrayIndexes = CreateNodeInfoArrayIndexList(e.Info);
            Kowhai.kowhai_symbol_t[] symbols = Kowhai.GetSymbolPath(GetDescriptor(sender), e.Info.KowhaiNode, e.Info.NodeIndex, arrayIndexes.ToArray());
            KowhaiProtocol.kowhai_protocol_t prot = new KowhaiProtocol.kowhai_protocol_t();
            prot.header.command = KowhaiProtocol.CMD_WRITE_DATA;
            prot.header.id = GetTreeId();
            int bytesRequired;
            KowhaiProtocol.Create(buffer, PACKET_SIZE, ref prot, symbols, out bytesRequired);
            int overhead;
            KowhaiProtocol.kowhai_protocol_get_overhead(ref prot, out overhead);
            int offset = 0;
            int maxPayloadSize = PACKET_SIZE - overhead;
            while (e.Buffer.Length - offset > maxPayloadSize)
            {
                KowhaiProtocol.Create(buffer, PACKET_SIZE, ref prot, symbols, CopyArray(e.Buffer, offset, maxPayloadSize), (ushort)offset, out bytesRequired);
                sock.Send(buffer, bytesRequired);
                offset += maxPayloadSize;
            }
            KowhaiProtocol.Create(buffer, PACKET_SIZE, ref prot, symbols, CopyArray(e.Buffer, offset, e.Buffer.Length - offset), (ushort)offset, out bytesRequired);
            sock.Send(buffer, bytesRequired);
        }

        private byte[] CopyArray(byte[] source, int offset, int size)
        {
            byte[] data = new byte[size];
            Array.Copy(source, offset, data, 0, size);
            return data;
        }

        void kowhaiTree_NodeRead(object sender, KowhaiTree.NodeReadEventArgs e)
        {
            Application.DoEvents();
            System.Threading.Thread.Sleep(250);

            byte[] buffer = new byte[PACKET_SIZE];
            List<ushort> arrayIndexes = CreateNodeInfoArrayIndexList(e.Info);
            Kowhai.kowhai_symbol_t[] symbols = Kowhai.GetSymbolPath(GetDescriptor(sender), e.Info.KowhaiNode, e.Info.NodeIndex, arrayIndexes.ToArray());
            KowhaiProtocol.kowhai_protocol_t prot = new KowhaiProtocol.kowhai_protocol_t();
            prot.header.command = KowhaiProtocol.CMD_READ_DATA;
            prot.header.id = GetTreeId();
            int bytesRequired;
            KowhaiProtocol.Create(buffer, PACKET_SIZE, ref prot, symbols, out bytesRequired);
            sock.Send(buffer, bytesRequired);
        }

        private void btnRefreshList_Click(object sender, EventArgs e)
        {
            kowhaiTreeMain.Clear();
            byte[] buffer = new byte[3];
            buffer[0] = KowhaiProtocol.CMD_GET_TREE_COUNT;
            sock.Send(buffer, buffer.Length);
        }


        private void lbTreeList_SelectedIndexChanged(object sender, EventArgs e)
        {
            byte[] buffer = new byte[3];
            buffer[0] = KowhaiProtocol.CMD_READ_DESCRIPTOR;
            buffer[1] = GetTreeId();
            sock.Send(buffer, buffer.Length);
        }

        string getSymbolName(Object param, UInt16 value)
        {
            return KowhaiSymbols.Symbols.Strings[value];
        }

        private void btnSave_Click(object sender, EventArgs e)
        {
            string text;
            if (KowhaiSerialize.Serialize(kowhaiTreeMain.GetDescriptor(), kowhaiTreeMain.GetData(), out text, 0x1000, null, getSymbolName) == Kowhai.STATUS_OK)
            {
                SaveFileDialog d = new SaveFileDialog();
                d.Filter = "Kowhai Files | *.kowhai";
                d.DefaultExt = "kowhai";
                if (d.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                {
                    System.IO.StreamWriter sw = System.IO.File.CreateText(d.FileName);
                    sw.Write(text);
                    sw.Close();
                }
            }
        }

        private void btnLoad_Click(object sender, EventArgs e)
        {
            LoadTree(kowhaiTreeMain);
        }

        private void LoadTree(KowhaiTree tree)
        {
            OpenFileDialog d = new OpenFileDialog();
            d.Filter = "Kowhai Files | *.kowhai";
            d.DefaultExt = "kowhai";
            if (d.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                string text = System.IO.File.ReadAllText(d.FileName);
                Kowhai.kowhai_node_t[] descriptor;
                byte[] data;
                if (KowhaiSerialize.Deserialize(text, out descriptor, out data) == Kowhai.STATUS_OK)
                {
                    tree.UpdateDescriptor(descriptor, KowhaiSymbols.Symbols.Strings, null);
                    tree.UpdateData(data, 0);
                }
            }
        }

        private void btnLoadScratch_Click(object sender, EventArgs e)
        {
            LoadTree(kowhaiTreeScratch);
        }

        void onSettingsDiffRight(Object param, Kowhai.Tree tree, Kowhai.kowhai_symbol_t[] symbolPath)
        {
            kowhaiTreeScratch.DiffAt(symbolPath);
        }

        private void btnDiff_Click(object sender, EventArgs e)
        {
            KowhaiTree leftTree = kowhaiTreeMain;
            Kowhai.Tree left = new Kowhai.Tree(leftTree.GetDescriptor(), leftTree.GetData());
            kowhaiTreeScratch.ResetNodesBackColor();
            Kowhai.Tree right = new Kowhai.Tree(kowhaiTreeScratch.GetDescriptor(), kowhaiTreeScratch.GetData());
            if (KowhaiUtils.Diff(left, right, null, null, onSettingsDiffRight) != Kowhai.STATUS_OK)
                MessageBox.Show("Diff Error", "Doh!", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }

        private void btnMerge_Click(object sender, EventArgs e)
        {
            KowhaiTree srcTree = kowhaiTreeMain;
            Kowhai.Tree src = new Kowhai.Tree(srcTree.GetDescriptor(), srcTree.GetData());
            Kowhai.Tree dst = new Kowhai.Tree(kowhaiTreeScratch.GetDescriptor(), kowhaiTreeScratch.GetData());
            if (KowhaiUtils.Merge(dst, src) == Kowhai.STATUS_OK)
                kowhaiTreeScratch.Update();
            else
                MessageBox.Show("Merge Error", "Doh!", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }

        private byte GetTreeId()
        {
            return (byte)lbTreeList.SelectedIndex;
        }

        private Kowhai.kowhai_node_t[] GetDescriptor(object sender)
        {
            return descriptors[GetTreeId()];
        }

        private void OnPaint(object sender, PaintEventArgs e)
        {
            // repaint the scope points
            Graphics g = e.Graphics;
            g.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.AntiAlias;
            int w = pnlScope.Width;
            int h = pnlScope.Height;
            const int border = 10;
            float x = border, dx = (float)(w - border * 2) / (ScopePoints.Count - 1);
            PointF last = new PointF(x, h - border), next;
            PointF[] points = new PointF[ScopePoints.Count];
            g.Clear(Color.White);
            if (ScopePoints.Count > 1)
            {
                for (int i = 0; i < ScopePoints.Count; i++, x += dx)
                {
                    // build the next point
                    UInt16 y = ScopePoints[i];
                    float yf = (h - border * 2) * (y - ScopeMinVal) / (ScopeMaxVal - ScopeMinVal);
                    next = new PointF(x, h - border - yf);
                    points[i] = last;
                    last = next;
                }
                g.DrawLines(Pens.CornflowerBlue, points);
            }
        }
    }
}
