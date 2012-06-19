using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace kowhai_test
{
    class CommsReceiveEventArgs : EventArgs
    {
        public byte[] Buffer;
        public int Size;
        public CommsReceiveEventArgs(byte[] buffer, int size)
        {
            Buffer = buffer;
            Size = size;
        }
    }

    delegate void CommsReceiveEventHandler(object sender, CommsReceiveEventArgs e);

    interface IComms
    {
        bool Connect();
        void Disconnect();
        int Send(byte[] buffer, int size);
        int Receive(byte[] buffer, int size);
        void StartAsyncReceives(byte[] buffer, int size);
        event EventHandler Connected;
        event EventHandler Disconnected;
        event CommsReceiveEventHandler CommsReceived;
    }
}
