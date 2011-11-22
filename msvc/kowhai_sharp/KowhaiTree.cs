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
                    return string.Format("{0}[{1}]", node.symbol, node.count);
                else
                    return node.symbol.ToString();
            }
            if (node.count > 1)
                return string.Format("{0}[{1}], {2}", node.symbol, node.count, GetDataTypeString(node.data_type));
            else
                return string.Format("{0}, {1}", node.symbol, GetDataTypeString(node.data_type));
        }

        public void Update(Kowhai.kowhai_node_t[] descriptor)
        {
            treeView1.Nodes.Clear();
            TreeNode current = null;
            TreeNodeCollection col = treeView1.Nodes;
            foreach (Kowhai.kowhai_node_t node in descriptor)
            {
                switch (node.type)
                {
                    case Kowhai.NODE_TYPE_BRANCH:
                        if (current == null)
                            current = treeView1.Nodes.Add(GetNodeName(node));
                        else
                            current = current.Nodes.Add(GetNodeName(node));
                        break;
                    case Kowhai.NODE_TYPE_LEAF:
                        current.Nodes.Add(GetNodeName(node));
                        break;
                    case Kowhai.NODE_TYPE_END:
                        current = current.Parent;
                        break;
                }
            }
            treeView1.ExpandAll();
        }
    }
}
