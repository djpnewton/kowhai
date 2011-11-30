using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace kowhai_sharp
{
    public partial class NodeEditForm : Form
    {
        public NodeEditForm()
        {
            InitializeComponent();
            kowhaiTree1.ContextMenuEnabled = false;
            kowhaiTree1.DataChange += new KowhaiTree.DataChangeEventHandler(kowhaiTree1_DataChange);
            kowhaiTree1.NodeRead += new KowhaiTree.NodeReadEventHandler(kowhaiTree1_NodeRead);
        }

        void kowhaiTree1_DataChange(object sender, KowhaiTree.DataChangeEventArgs e)
        {
            kowhaiTree1.UpdateData(e.Buffer, e.Info.Offset);
        }

        void kowhaiTree1_NodeRead(object sender, KowhaiTree.NodeReadEventArgs e)
        {
        }

        public void UpdateTree(Kowhai.kowhai_node_t[] descriptor, string[] symbols, byte[] data)
        {
            kowhaiTree1.UpdateDescriptor(descriptor, symbols);
            kowhaiTree1.UpdateData(data, 0);
        }

        public byte[] GetData()
        {
            return kowhaiTree1.GetData();
        }
    }
}
