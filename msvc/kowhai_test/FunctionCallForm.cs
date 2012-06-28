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
    public partial class FunctionCallForm : Form
    {
        public class CallFunctionEventArgs : EventArgs
        {
            public ushort FunctionId;
            public byte[] Buffer;
            public CallFunctionEventArgs(ushort functionId, byte[] buffer)
            {
                FunctionId = functionId;
                Buffer = buffer;
            }
        }

        public delegate void CallFunctionEventHandler(object sender, CallFunctionEventArgs e);
        public event CallFunctionEventHandler CallFunction;

        string[] symbolStrings = null;
        public string[] SymbolStrings
        {
            get { return symbolStrings; }
            set { symbolStrings = value; }
        }

        SymbolName funcName;
        public SymbolName FunctionName
        {
            get { return funcName; }
            set 
            {
                funcName = value;
                Text = string.Format("{0} Function Call", FunctionName.Name);
            }
        }
        public kowhai_sharp.KowhaiProtocol.kowhai_protocol_function_details_t FunctionDetails { get; set; }
        public kowhai_sharp.Kowhai.kowhai_node_t[] TreeInDescriptor
        {
            get { return treeIn.GetDescriptor(); }
            set
            {
                treeIn.UpdateDescriptor(value, symbolStrings, null);
                int dataSize;
                Kowhai.GetNodeSize(value, out dataSize);
                treeIn.UpdateData(new byte[dataSize], 0);
            }
        }
        public kowhai_sharp.Kowhai.kowhai_node_t[] TreeOutDescriptor
        {
            get { return treeOut.GetDescriptor(); }
            set { treeOut.UpdateDescriptor(value, symbolStrings, null); }
        }

        public FunctionCallForm()
        {
            InitializeComponent();
        }

        private void btnCall_Click(object sender, EventArgs e)
        {
            if (CallFunction != null)
                CallFunction(this, new CallFunctionEventArgs(funcName.Symbol, treeIn.GetData()));
        }

        public void SetFunctionOutData(byte[] buf, int offset)
        {
            treeOut.UpdateData(buf, offset);
        }

        private void FunctionCallForm_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Escape)
                Close();
        }
    }
}
