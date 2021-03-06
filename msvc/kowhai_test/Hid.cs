﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace kowhai_test
{
    class Hid: IComms
    {
        string path;
        byte reportId;
        byte[] reportPrefix;
        int reportSize;
        IntPtr device = new IntPtr(0);
        Thread readThread;
        bool readThreadKeepGoing;
        byte[] readBuffer;
        int readBufferSize;
        public event EventHandler Connected;
        public event EventHandler Disconnected;
        public event CommsReceiveEventHandler CommsReceived;

        public Hid(string path, byte reportId, byte[] reportPrefix, int reportSize)
        {
            this.path = path;
            this.reportId = reportId;
            this.reportPrefix = reportPrefix;
            this.reportSize = reportSize;
        }

        ~Hid()
        {
            Disconnect();
        }

        public bool Connect()
        {
            device = HidAPI.hid_open_path(path);
            if (!device.Equals(IntPtr.Zero))
            {
                DoConnected();
                return true;
            }
            return false;
        }

        public void Disconnect()
        {
            KillReadThread();
            if (!device.Equals(IntPtr.Zero))
            {
                HidAPI.hid_close(device);
                DoDisconnected();
            }
        }

        private void KillReadThread()
        {
            readThreadKeepGoing = false;
            if (readThread != null)
                readThread.Join();
        }

        public int Send(byte[] buffer, int size)
        {
            if (device.Equals(IntPtr.Zero))
                return -1;
            byte[] report = new byte[reportSize];
            report[0] = reportId;
            Array.Copy(reportPrefix, 0, report, 1, Math.Min(reportSize, reportPrefix.Length));
            Array.Copy(buffer, 0, report, reportPrefix.Length + 1, Math.Min(size, reportSize - reportPrefix.Length - 1));
            return HidAPI.hid_write(device, report, new UIntPtr((uint)reportSize));
        }

        public int Receive(byte[] buffer, int size)
        {
            byte[] report = new byte[reportSize];
            int res = HidAPI.hid_read(device, report, new UIntPtr((uint)reportSize));
            if (res > 0)
            {
                byte[] reportPrefixCheck = new byte[reportPrefix.Length];
                Array.Copy(report, 1, reportPrefixCheck, 0, Math.Min(reportPrefix.Length, reportSize - 1));
                if (!reportPrefix.SequenceEqual(reportPrefixCheck))
                    return -2;
                Array.Copy(report, reportPrefix.Length + 1, buffer, 0, Math.Min(size, reportSize - reportPrefix.Length - 1));
                return Math.Min(size, reportSize - 1);
            }
            return res;
        }

        public void StartAsyncReceives(byte[] buffer, int size)
        {
            readThreadKeepGoing = true;
            readBuffer = buffer;
            readBufferSize = size;
            readThread = new Thread(new ThreadStart(ReadThread));
            readThread.Start();
        }

        void ReadThread()
        {
            byte[] report = new byte[reportSize];
            int res = 0;
            while (readThreadKeepGoing && res >= 0)
            {
                res = HidAPI.hid_read_timeout(device, report, new UIntPtr((uint)reportSize), 1000);
                if (res > 0 && CommsReceived != null)
                {
                    byte[] reportPrefixCheck = new byte[reportPrefix.Length];
                    Array.Copy(report, 1, reportPrefixCheck, 0, Math.Min(reportPrefix.Length, reportSize - 1));
                    if (!reportPrefix.SequenceEqual(reportPrefixCheck))
                        continue;
                    int reportIdSize = 0;
                    if (reportId != 0)
                        reportIdSize = 1;
                    Array.Copy(report, reportPrefix.Length + reportIdSize, readBuffer, 0, Math.Min(readBufferSize, reportSize - reportPrefix.Length - reportIdSize));
                    CommsReceived(this, new CommsReceiveEventArgs(readBuffer, Math.Min(readBufferSize, reportSize - reportPrefix.Length - reportIdSize)));
                }
                else if (res == -1)
                    DoDisconnected();
            }
        }

        private void DoConnected()
        {
            if (Connected != null)
                Connected(this, new EventArgs());
        }

        private void DoDisconnected()
        {
            if (Disconnected != null)
                Disconnected(this, new EventArgs());
            device = IntPtr.Zero;
        }

        public string GetErrorString()
        {
            return HidAPI.hid_error(device);
        }
    }
}
