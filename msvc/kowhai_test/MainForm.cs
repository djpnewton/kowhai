﻿using System;
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
        const int TREE_ID_SETTINGS = 0;
        const int TREE_ID_SHADOW = 1;
        const int TREE_ID_ACTIONS = 2;
        const int TREE_ID_SCOPE = 3;

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
                btnRefreshTrees.Enabled = true;
                sock.SockBufferReceived += new SockReceiveEventHandler(sock_SockBufferReceived);
                sock.StartAsyncReceives(new byte[PACKET_SIZE], PACKET_SIZE);
                kowhaiTreeSettings.DataChange += new KowhaiTree.DataChangeEventHandler(kowhaiTree_DataChange);
                kowhaiTreeShadow.DataChange += new KowhaiTree.DataChangeEventHandler(kowhaiTree_DataChange);
                kowhaiTreeActions.DataChange += new KowhaiTree.DataChangeEventHandler(kowhaiTree_DataChange);
                kowhaiTreeScope.DataChange += new KowhaiTree.DataChangeEventHandler(kowhaiTree_DataChange);
                kowhaiTreeSettings.NodeRead += new KowhaiTree.NodeReadEventHandler(kowhaiTree_NodeRead);
                kowhaiTreeShadow.NodeRead += new KowhaiTree.NodeReadEventHandler(kowhaiTree_NodeRead);
                kowhaiTreeActions.NodeRead += new KowhaiTree.NodeReadEventHandler(kowhaiTree_NodeRead);
                kowhaiTreeScope.NodeRead += new KowhaiTree.NodeReadEventHandler(kowhaiTree_NodeRead);
            }
            else
                btnRefreshTrees.Enabled = false;
        }

        private void MainForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            if (btnRefreshTrees.Enabled)
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

        private ArrayList ScopePoints = new ArrayList();
        UInt16 ScopeMinVal = UInt16.MaxValue, ScopeMaxVal = UInt16.MinValue;
        private void ProcessPacket(byte[] buffer)
        {
            KowhaiProtocol.kowhai_protocol_t prot;
            Kowhai.kowhai_symbol_t[] symbols;
            if (KowhaiProtocol.Parse(buffer, buffer.Length, out prot, out symbols) == Kowhai.STATUS_OK)
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
                        {
                            KowhaiTree tree = GetKowhaiTree(prot.header.tree_id);
                            tree.UpdateData(data, nodeOffset + prot.payload.spec.data.memory.offset);
                            if (tree == kowhaiTreeScope)
                            {
                                //Kowhai.GetNode(tree.GetDescriptor(), symbolsPath, out nodeOffset, out node);
                                // update the scope points the the new data
                                for (int i = 0; i < data.Length / 2; i++)
                                {
                                    UInt16 value = BitConverter.ToUInt16(data, i * 2);
                                    if (value > ScopeMaxVal)
                                        ScopeMaxVal = value;
                                    if (value < ScopeMinVal)
                                        ScopeMinVal = value;
                                    int arrayIndex = 0;
                                    if (symbols.Length == 2)
                                        arrayIndex = symbols[1].parts.array_index;
                                    int k = (arrayIndex * 2 + prot.payload.spec.data.memory.offset) / 2 + i;
                                    if (ScopePoints.Count > k)
                                        ScopePoints[k] = value;
                                    else
                                        ScopePoints.Add(value);
                                }
                                pnlScope.Invalidate();
                            }
                        }
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
                            GetKowhaiTree(prot.header.tree_id).UpdateDescriptor(descriptor, KowhaiSymbols.Symbols.Strings, null);

                            buffer = new byte[PACKET_SIZE];
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
            prot.header.tree_id = GetTreeId(sender);
            prot.header.command = KowhaiProtocol.CMD_WRITE_DATA;
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
            prot.header.tree_id = GetTreeId(sender);
            prot.header.command = KowhaiProtocol.CMD_READ_DATA;
            int bytesRequired;
            KowhaiProtocol.Create(buffer, PACKET_SIZE, ref prot, symbols, out bytesRequired);
            sock.Send(buffer, bytesRequired);
        }

        private void btnRefreshTrees_Click(object sender, EventArgs e)
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
            System.Threading.Thread.Sleep(100);
            Application.DoEvents();
            buffer[0] = TREE_ID_SCOPE;
            sock.Send(buffer, 2);
        }

        string getSymbolName(Object param, UInt16 value)
        {
            return KowhaiSymbols.Symbols.Strings[value];
        }

        private void btnSave_Click(object sender, EventArgs e)
        {
            KowhaiTree tree = GetTreeFromRadioButtonSelection();
            string text;
            if (KowhaiSerialize.Serialize(tree.GetDescriptor(), tree.GetData(), out text, 0x1000, null, getSymbolName) == Kowhai.STATUS_OK)
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
            OpenFileDialog d = new OpenFileDialog();
            d.Filter = "Kowhai Files | *.kowhai";
            d.DefaultExt = "kowhai";
            if (d.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                string text = System.IO.File.ReadAllText(d.FileName);
                Kowhai.kowhai_node_t[] descriptor;
                byte[] data;
                KowhaiTree tree = GetTreeFromRadioButtonSelection();
                if (KowhaiSerialize.Deserialize(text, out descriptor, out data) == Kowhai.STATUS_OK)
                {
                    tree.UpdateDescriptor(descriptor, KowhaiSymbols.Symbols.Strings, null);
                    tree.UpdateData(data, 0);
                }
            }
        }

        void onSettingsDiffLeft(Object param, Kowhai.Tree tree, Kowhai.kowhai_symbol_t[] symbolPath)
        {
            kowhaiTreeSettings.DiffAt(symbolPath);
        }

        private void btnDiff_Click(object sender, EventArgs e)
        {
            kowhaiTreeSettings.ResetNodesBackColor();
            KowhaiTree rightTree = GetTreeFromRadioButtonSelection();
            Kowhai.Tree left = new Kowhai.Tree(kowhaiTreeSettings.GetDescriptor(), kowhaiTreeSettings.GetData());
            Kowhai.Tree right = new Kowhai.Tree(rightTree.GetDescriptor(), rightTree.GetData());
            if (KowhaiUtils.Diff(left, right, null, onSettingsDiffLeft, null) != Kowhai.STATUS_OK)
                MessageBox.Show("Diff Error", "Doh!", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }

        private void btnMerge_Click(object sender, EventArgs e)
        {
            KowhaiTree destTree = GetTreeFromRadioButtonSelection();
            if (KowhaiUtils.Merge(new Kowhai.Tree(destTree.GetDescriptor(), destTree.GetData()),
                new Kowhai.Tree(kowhaiTreeSettings.GetDescriptor(), kowhaiTreeSettings.GetData())) == Kowhai.STATUS_OK)
                destTree.Update();
            else
                MessageBox.Show("Merge Error", "Doh!", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }

        private Kowhai.kowhai_symbol_t[] GetRootSymbolPath(byte treeId)
        {
            if (treeId == TREE_ID_SETTINGS)
                return new Kowhai.kowhai_symbol_t[] { new Kowhai.kowhai_symbol_t((uint)KowhaiSymbols.Symbols.Constants.Settings) };
            if (treeId == TREE_ID_SHADOW)
                return new Kowhai.kowhai_symbol_t[] { new Kowhai.kowhai_symbol_t((uint)KowhaiSymbols.Symbols.Constants.Shadow) };
            if (treeId == TREE_ID_ACTIONS)
                return new Kowhai.kowhai_symbol_t[] { new Kowhai.kowhai_symbol_t((uint)KowhaiSymbols.Symbols.Constants.Actions) };
            if (treeId == TREE_ID_SCOPE)
                return new Kowhai.kowhai_symbol_t[] { new Kowhai.kowhai_symbol_t((uint)KowhaiSymbols.Symbols.Constants.Scope) };
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
            if (treeId == TREE_ID_SCOPE)
                return kowhaiTreeScope;
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
            if (sender == kowhaiTreeScope)
                return TREE_ID_SCOPE;
            return 255;
        }

        KowhaiTree GetTreeFromRadioButtonSelection()
        {
            if (rbSettings.Checked)
                return kowhaiTreeSettings;
            if (rbShadow.Checked)
                return kowhaiTreeShadow;
            if (rbActions.Checked)
                return kowhaiTreeActions;
            if (rbScope.Checked)
                return kowhaiTreeScope;
            return null;
        }

        private Kowhai.kowhai_node_t[] GetDescriptor(object sender)
        {
            return descriptors[GetTreeId(sender)];
        }

        private void OnPaint(object sender, PaintEventArgs e)
        {
            // repaint the scope points
            Graphics g = pnlScope.CreateGraphics();
            int w = pnlScope.Width;
            int h = pnlScope.Height;
            float x = 0, dx = (float)w / (ScopePoints.Count - 1);
            PointF last = new PointF(x, 0), next;
            g.Clear(Color.Gray);
            for (int i = 0; i < ScopePoints.Count; i++, x += dx)
            {
                // build the next point
                UInt16 y = (UInt16)ScopePoints[i];
                float yf = h * (y - ScopeMinVal) / (ScopeMaxVal - ScopeMinVal);
                next = new PointF(x, yf);
                g.DrawLine(Pens.Blue, next, last);
                last = next;
            }
        }
    }
}
