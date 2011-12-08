using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace kowhai_sharp
{
    public class KowhaiSerialize
    {
        [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet=CharSet.Ansi)]
        public delegate string kowhai_get_symbol_name_t(UInt16 value);

        [DllImport(Kowhai.dllname, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern int kowhai_serialize(IntPtr descriptor, byte[] data, int data_size, byte[] target_buffer, ref int target_size, kowhai_get_symbol_name_t get_name);

        [DllImport(Kowhai.dllname, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern int kowhai_deserialize(string buffer, byte[] scratch, int scratch_size, IntPtr descriptor, ref int descriptor_size, byte[] data, ref int data_size);

        public static int Serialize(Kowhai.kowhai_node_t[] descriptor, byte[] data, out string target, int targetBufferSize, kowhai_get_symbol_name_t getName)
        {
            byte[] targetBuf = new byte[targetBufferSize];
            GCHandle h = GCHandle.Alloc(descriptor, GCHandleType.Pinned);
            int result = kowhai_serialize(h.AddrOfPinnedObject(), data, data.Length, targetBuf, ref targetBufferSize, getName);
            h.Free();
            ASCIIEncoding enc = new ASCIIEncoding();
            target = enc.GetString(targetBuf, 0, targetBufferSize);
            return result;
        }

        public static int Deserialize(string buffer, out Kowhai.kowhai_node_t[] descriptor, out byte[] data)
        {
            //TODO: check for "buffer too small" errors and increase sizes
            byte[] scratch = new byte[0x1000];
            descriptor = new Kowhai.kowhai_node_t[0x1000];
            data = new byte[0x1000];
            int scratch_size = 0x1000;
            int descriptor_size = 0x1000;
            int data_size = 0x1000;

            GCHandle h = GCHandle.Alloc(descriptor, GCHandleType.Pinned);
            int result = kowhai_deserialize(buffer, scratch, scratch_size, h.AddrOfPinnedObject(), ref descriptor_size, data, ref data_size);
            h.Free();
            if (result == Kowhai.STATUS_OK)
            {
                Array.Resize(ref descriptor, descriptor_size);
                Array.Resize(ref data, data_size);
            }
            return result;
        }
    }
}
