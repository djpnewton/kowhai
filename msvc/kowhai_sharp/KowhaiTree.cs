using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace kowhai_sharp
{
    public partial class KowhaiTree : UserControl
    {
        public class KowhaiNodeInfo
        {
            public Kowhai.kowhai_node_t KowhaiNode;
            public int NodeIndex;
            public bool IsArrayItem;
            public ushort ArrayIndex;
            public ushort Offset;
            public KowhaiNodeInfo Parent;
            public KowhaiNodeInfo(Kowhai.kowhai_node_t kowhaiNode, int nodeIndex, bool isArrayItem, ushort arrayIndex, ushort offset, KowhaiNodeInfo parent)
            {
                KowhaiNode = kowhaiNode;
                NodeIndex = nodeIndex;
                IsArrayItem = isArrayItem;
                ArrayIndex = arrayIndex;
                Offset = offset;
                Parent = parent;
            }
        }

        public class DataChangeEventArgs : EventArgs
        {
            public KowhaiNodeInfo Info;
            public byte[] Buffer;
            public DataChangeEventArgs(KowhaiNodeInfo info, byte[] buffer)
            {
                Info = info;
                Buffer = buffer;
            }
        }

        public delegate void DataChangeEventHandler(object sender, DataChangeEventArgs e);

        public class NodeReadEventArgs : EventArgs
        {
            public KowhaiNodeInfo Info;
            public NodeReadEventArgs(KowhaiNodeInfo info)
            {
                Info = info;
            }
        }

        public delegate void NodeReadEventHandler(object sender, NodeReadEventArgs e);

        string[] symbols;
        byte[] data;

        public event DataChangeEventHandler DataChange;
        public event NodeReadEventHandler NodeRead;

        public KowhaiTree()
        {
            InitializeComponent();
        }

        string GetDataTypeString(int dataType)
        {
            dataType = Kowhai.RawDataType(dataType);
            switch (dataType)
            {
                case Kowhai.DATA_TYPE_CHAR:
                    return "char";
                case Kowhai.DATA_TYPE_UCHAR:
                    return "uchar";
                case Kowhai.DATA_TYPE_INT16:
                    return "int16";
                case Kowhai.DATA_TYPE_UINT16:
                    return "uint16";
                case Kowhai.DATA_TYPE_INT32:
                    return "int32";
                case Kowhai.DATA_TYPE_UINT32:
                    return "uint32";
                case Kowhai.DATA_TYPE_FLOAT:
                    return "float";
            }
            return "error";
        }

        private object GetDataValue(KowhaiNodeInfo info)
        {
            if (info == null)
                return null;
            int size = Kowhai.kowhai_get_data_size(info.KowhaiNode.data_type);
            if (info.Offset + size <= data.Length)
            {
                switch (info.KowhaiNode.data_type)
                {
                    case Kowhai.DATA_TYPE_CHAR:
                        return (sbyte)data[info.Offset];
                    case Kowhai.DATA_TYPE_UCHAR:
                        return data[info.Offset];
                    case Kowhai.DATA_TYPE_INT16:
                        return BitConverter.ToInt16(data, info.Offset);
                    case Kowhai.DATA_TYPE_UINT16:
                        return BitConverter.ToUInt16(data, info.Offset);
                    case Kowhai.DATA_TYPE_INT32:
                        return BitConverter.ToInt32(data, info.Offset);
                    case Kowhai.DATA_TYPE_UINT32:
                        return BitConverter.ToUInt32(data, info.Offset);
                    case Kowhai.DATA_TYPE_FLOAT:
                        return BitConverter.ToSingle(data, info.Offset);
                }
            }
            return null;
        }

        private byte[] TextToData(string text, ushort dataType)
        {
            switch (dataType)
            {
                case Kowhai.DATA_TYPE_CHAR:
                    return new byte[] { (byte)Convert.ToSByte(text) };
                case Kowhai.DATA_TYPE_UCHAR:
                    return new byte[] { Convert.ToByte(text) };
                case Kowhai.DATA_TYPE_INT16:
                    return BitConverter.GetBytes(Convert.ToInt16(text));
                case Kowhai.DATA_TYPE_UINT16:
                    return BitConverter.GetBytes(Convert.ToUInt16(text));
                case Kowhai.DATA_TYPE_INT32:
                    return BitConverter.GetBytes(Convert.ToInt32(text));
                case Kowhai.DATA_TYPE_UINT32:
                    return BitConverter.GetBytes(Convert.ToUInt32(text));
                case Kowhai.DATA_TYPE_FLOAT:
                    return BitConverter.GetBytes(Convert.ToSingle(text));
            }
            return null;
        }

        string GetNodeTagString(Kowhai.kowhai_node_t node)
        {
            if (node.tag > 0)
                return string.Format("({0})", node.tag);
            return "";
        }

        string GetNodeArrayString(Kowhai.kowhai_node_t node)
        {
            if (node.count > 1)
                return string.Format("[{0}]", node.count);
            return "";
        }

        string GetNodeName(Kowhai.kowhai_node_t node, KowhaiNodeInfo info)
        {
            if (node.type == Kowhai.NODE_TYPE_BRANCH)
                return string.Format("{0}{1}{2}", symbols[node.symbol], GetNodeArrayString(node), GetNodeTagString(node));
            if (info != null && info.IsArrayItem)
                return string.Format("#{0}{1} = {2}", info.ArrayIndex, GetNodeTagString(node), GetDataValue(info));
            else if (node.count > 1)
                return string.Format("{0}{1}{2}: {3}", symbols[node.symbol], GetNodeArrayString(node), GetNodeTagString(node), GetDataTypeString(node.data_type));
            else
                return string.Format("{0}{1}: {2} = {3}", symbols[node.symbol], GetNodeTagString(node), GetDataTypeString(node.data_type), GetDataValue(info));
        }

        void _UpdateDescriptor(Kowhai.kowhai_node_t[] descriptor, ref int index, ref ushort offset, TreeNode node)
        {
            while (index < descriptor.Length)
            {
                Kowhai.kowhai_node_t descNode = descriptor[index];
                switch (descNode.type)
                {
                    case Kowhai.NODE_TYPE_BRANCH:
                        if (node == null)
                            node = treeView1.Nodes.Add(GetNodeName(descNode, null));
                        else
                            node = node.Nodes.Add(GetNodeName(descNode, null));
                        KowhaiNodeInfo parentInfo = null;
                        if (node.Parent != null)
                            parentInfo = (KowhaiNodeInfo)node.Parent.Tag;
                        if (descNode.count > 1)
                        {
                            int prevIndex = index;
                            for (ushort i = 0; i < descNode.count; i++)
                            {
                                index = prevIndex;
                                TreeNode arrayNode = node.Nodes.Add("#" + i.ToString());
                                arrayNode.Tag = new KowhaiNodeInfo(descNode, index, true, i, offset, parentInfo);
                                index++;
                                _UpdateDescriptor(descriptor, ref index, ref offset, arrayNode);
                            }
                        }
                        else
                        {
                            node.Tag = new KowhaiNodeInfo(descNode, index, false, 0, offset, parentInfo);
                            index++;
                            _UpdateDescriptor(descriptor, ref index, ref offset, node);
                        }
                        node = node.Parent;
                        break;
                    case Kowhai.NODE_TYPE_LEAF:
                        TreeNode leaf = node.Nodes.Add(GetNodeName(descNode, null));
                        if (descNode.count > 1)
                        {
                            for (ushort i = 0; i < descNode.count; i++)
                            {
                                TreeNode child = leaf.Nodes.Add("#" + i.ToString());
                                child.Tag = new KowhaiNodeInfo(descNode, index, true, i, offset, (KowhaiNodeInfo)leaf.Parent.Tag);
                                offset += (ushort)Kowhai.kowhai_get_data_size(descNode.data_type);
                            }
                        }
                        else
                        {
                            leaf.Tag = new KowhaiNodeInfo(descNode, index, false, 0, offset, (KowhaiNodeInfo)leaf.Parent.Tag);
                            offset += (ushort)Kowhai.kowhai_get_data_size(descNode.data_type);
                        }
                        break;
                    case Kowhai.NODE_TYPE_END:
                        return;
                }
                index++;
            }
        }

        public void UpdateDescriptor(Kowhai.kowhai_node_t[] descriptor, string[] symbols)
        {
            this.symbols = symbols;
            treeView1.Nodes.Clear();
            int index = 0;
            ushort offset = 0;
            _UpdateDescriptor(descriptor, ref index, ref offset, null);
            treeView1.ExpandAll();
            return;
        }

        void UpdateTreeNodeData(TreeNodeCollection nodes)
        {
            foreach (TreeNode node in nodes)
            {
                if (node.Nodes.Count > 0)
                    UpdateTreeNodeData(node.Nodes);
                else if (node.Tag != null)
                {
                    KowhaiNodeInfo info = (KowhaiNodeInfo)node.Tag;
                    string newName = GetNodeName(info.KowhaiNode, info);
                    if (newName != node.Text)
                        node.Text = newName;
                }
            }
        }

        public void UpdateData(byte[] newData, int offset)
        {
            int maxSize = offset + newData.Length;
            if (data == null)
                data = new byte[maxSize];
            if (data.Length < maxSize)
                Array.Resize<byte>(ref data, maxSize);
            Array.Copy(newData, 0, data, offset, newData.Length);

            treeView1.BeginUpdate();
            UpdateTreeNodeData(treeView1.Nodes);
            treeView1.EndUpdate();
        }

        TreeNode selectedNode;

        private void treeView1_MouseDown(object sender, MouseEventArgs e)
        {
            selectedNode = treeView1.GetNodeAt(e.X, e.Y);
        }


        private void treeView1_MouseUp(object sender, MouseEventArgs e)
        {
            if (e.Button == System.Windows.Forms.MouseButtons.Right && selectedNode != null)
            {
                treeView1.SelectedNode = selectedNode;
                contextMenuStrip1.Show(this, new Point(e.X, e.Y));
            }
        }

        private void treeView1_DoubleClick(object sender, EventArgs e)
        {
            if (selectedNode != null)
            {
                treeView1.SelectedNode = selectedNode;
                BeginEdit(selectedNode);
            }
        }

        private void BeginEdit(TreeNode node)
        {
            if (data != null && node != null && !node.IsEditing)
            {
                if (node.Tag != null)
                {
                    KowhaiNodeInfo info = (KowhaiNodeInfo)node.Tag;
                    if (info.KowhaiNode.type == Kowhai.NODE_TYPE_LEAF)
                    {
                        object dataValue = GetDataValue(info);
                        if (dataValue != null)
                        {
                            treeView1.LabelEdit = true;
                            node.Text = dataValue.ToString();
                            node.BeginEdit();
                        }
                    }
                }
            }
        }

        private void treeView1_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Return || e.KeyCode == Keys.Enter)
                BeginEdit(treeView1.SelectedNode);
        }

        private void treeView1_AfterLabelEdit(object sender, NodeLabelEditEventArgs e)
        {
            e.CancelEdit = true;
            KowhaiNodeInfo info = (KowhaiNodeInfo)e.Node.Tag;
            if (e.Label != null)
            {
                byte[] data;
                try
                {
                    data = TextToData(e.Label, info.KowhaiNode.data_type);
                    e.Node.Text = "updating...";
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }
                DataChange(this, new DataChangeEventArgs(info, data));
            }
            else
                e.Node.Text = GetNodeName(info.KowhaiNode, info);
            treeView1.LabelEdit = false;
        }

        private void refreshNodeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (selectedNode != null && selectedNode.Tag != null)
            {
                KowhaiNodeInfo info = (KowhaiNodeInfo)selectedNode.Tag;
                NodeRead(this, new NodeReadEventArgs(info));
            }
        }

        private void writeNodeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (selectedNode != null && selectedNode.Tag != null)
            {
                KowhaiNodeInfo info = (KowhaiNodeInfo)selectedNode.Tag;
                DataChange(this, new DataChangeEventArgs(info, new byte[0]));
            }
        }
    }
}
