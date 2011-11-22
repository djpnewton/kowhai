using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;

namespace kowhai_test
{
    class Sock
    {
        static IPAddress addr = IPAddress.Parse("127.0.0.1");
        static IPEndPoint ep = new IPEndPoint(addr, 55555);

        Socket sock;

        public Sock()
        {
        }

        public bool Connect()
        {
            try
            {
                sock = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                sock.Connect(ep);
                return true;
            }
            catch (Exception e)
            {
                System.Diagnostics.Debug.WriteLine(e.Message);
                sock = null;
                return false;
            }
        }

        public void Disconnect()
        {
            sock.Disconnect(true);
        }

        public int Send(byte[] buffer, int size)
        {
            return sock.Send(buffer, size, SocketFlags.None);
        }

        public int Receive(byte[] buffer, int size)
        {
            return sock.Receive(buffer, size, SocketFlags.None);
        }
    }
}
