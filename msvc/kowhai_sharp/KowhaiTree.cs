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
        struct KowhaiNodeInfo
        {
            public Kowhai.kowhai_node_t KowhaiNode;
            public bool IsArrayItem;
            public int ArrayIndex;
            public int Offset;
            public KowhaiNodeInfo(Kowhai.kowhai_node_t kowhaiNode, bool isArrayItem, int arrayIndex, int offset)
            {
                KowhaiNode = kowhaiNode;
                IsArrayItem = isArrayItem;
                ArrayIndex = arrayIndex;
                Offset = offset;
            }
        }

        string[] symbols;
        byte[] data;

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

        private object GetDataValue(KowhaiNodeInfo? info)
        {
            if (info == null)
                return "null";
            int size = Kowhai.kowhai_get_data_size(info.Value.KowhaiNode.data_type);
            if (info.Value.Offset + size <= data.Length)
            {
                switch (info.Value.KowhaiNode.data_type)
                {
                    case Kowhai.DATA_TYPE_CHAR:
                        return BitConverter.ToChar(data, info.Value.Offset);
                    case Kowhai.DATA_TYPE_UCHAR:
                        return (byte)BitConverter.ToChar(data, info.Value.Offset);
                    case Kowhai.DATA_TYPE_INT16:
                        return BitConverter.ToInt16(data, info.Value.Offset);
                    case Kowhai.DATA_TYPE_UINT16:
                        return BitConverter.ToUInt16(data, info.Value.Offset);
                    case Kowhai.DATA_TYPE_INT32:
                        return BitConverter.ToInt32(data, info.Value.Offset);
                    case Kowhai.DATA_TYPE_UINT32:
                        return BitConverter.ToUInt32(data, info.Value.Offset);
                    case Kowhai.DATA_TYPE_FLOAT:
                        return BitConverter.ToSingle(data, info.Value.Offset);
                }
            }
            return "error";
        }

        string GetNodeName(Kowhai.kowhai_node_t node, KowhaiNodeInfo? info)
        {
            if (node.type == Kowhai.NODE_TYPE_BRANCH)
            {
                if (node.count > 1)
                    return string.Format("{0}[{1}]", symbols[node.symbol], node.count);
                else
                    return symbols[node.symbol];
            }
            if (info != null && info.Value.IsArrayItem)
                return string.Format("#{0} = {1}", info.Value.ArrayIndex, GetDataValue(info));
            else if (node.count > 1)
                return string.Format("{0}[1]: {2}", symbols[node.symbol], node.count, GetDataTypeString(node.data_type));
            else
                return string.Format("{0}: {1} = {2}", symbols[node.symbol], GetDataTypeString(node.data_type), GetDataValue(info));
        }

        void _UpdateDescriptor(Kowhai.kowhai_node_t[] descriptor, ref int index, ref int offset, TreeNode node)
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
                        index++;
                        if (descNode.count > 1)
                        {
                            int prevIndex = index;
                            for (int i = 0; i < descNode.count; i++)
                            {
                                index = prevIndex;
                                TreeNode arrayNode = node.Nodes.Add("#" + i.ToString());
                                _UpdateDescriptor(descriptor, ref index, ref offset, arrayNode);
                            }
                        }
                        else
                            _UpdateDescriptor(descriptor, ref index, ref offset, node);
                        node = node.Parent;
                        break;
                    case Kowhai.NODE_TYPE_LEAF:
                        TreeNode leaf = node.Nodes.Add(GetNodeName(descNode, null));
                        if (descNode.count > 1)
                        {
                            for (int i = 0; i < descNode.count; i++)
                            {
                                TreeNode child = leaf.Nodes.Add("#" + i.ToString());
                                child.Tag = new KowhaiNodeInfo(descNode, true, i, offset);
                                offset += Kowhai.kowhai_get_data_size(descNode.data_type);
                            }
                        }
                        else
                        {
                            leaf.Tag = new KowhaiNodeInfo(descNode, false, 0, offset);
                            offset += Kowhai.kowhai_get_data_size(descNode.data_type);
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
            int index = 0, offset = 0;
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
                    node.Text = GetNodeName(info.KowhaiNode, info);
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

            UpdateTreeNodeData(treeView1.Nodes);
        }
    }
}
