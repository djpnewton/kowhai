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
        string[] symbols;

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

        string GetNodeName(Kowhai.kowhai_node_t node)
        {
            if (node.type == Kowhai.NODE_TYPE_BRANCH)
            {
                if (node.count > 1)
                    return string.Format("{0}[{1}]", symbols[node.symbol], node.count);
                else
                    return symbols[node.symbol];
            }
            if (node.count > 1)
                return string.Format("{0}[{1}]: {2}", symbols[node.symbol], node.count, GetDataTypeString(node.data_type));
            else
                return string.Format("{0}, {1}", symbols[node.symbol], GetDataTypeString(node.data_type));
        }

        void _Update(Kowhai.kowhai_node_t[] descriptor, ref int index, TreeNode node)
        {
            while (index < descriptor.Length)
            {
                Kowhai.kowhai_node_t descNode = descriptor[index];
                switch (descNode.type)
                {
                    case Kowhai.NODE_TYPE_BRANCH:
                        if (node == null)
                            node = treeView1.Nodes.Add(GetNodeName(descNode));
                        else
                            node = node.Nodes.Add(GetNodeName(descNode));
                        index++;
                        if (descNode.count > 1)
                        {
                            int prevIndex = index;
                            for (int i = 0; i < descNode.count; i++)
                            {
                                index = prevIndex;
                                TreeNode arrayNode = node.Nodes.Add("#" + i.ToString());
                                _Update(descriptor, ref index, arrayNode);
                            }
                        }
                        else
                            _Update(descriptor, ref index, node);
                        node = node.Parent;
                        break;
                    case Kowhai.NODE_TYPE_LEAF:
                        TreeNode leaf = node.Nodes.Add(GetNodeName(descNode));
                        if (descNode.count > 1)
                            for (int i = 0; i < descNode.count; i++)
                                leaf.Nodes.Add("#" + i.ToString());
                        break;
                    case Kowhai.NODE_TYPE_END:
                        return;
                }
                index++;
            }
        }

        public void Update(Kowhai.kowhai_node_t[] descriptor, string[] symbols)
        {
            this.symbols = symbols;
            treeView1.Nodes.Clear();
            int index = 0;
            _Update(descriptor, ref index, null);
            treeView1.ExpandAll();
            return;
        }
    }
}
