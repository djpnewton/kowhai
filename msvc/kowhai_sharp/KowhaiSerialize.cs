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
        public static extern int kowhai_serialize(IntPtr descriptor, IntPtr data, int data_size, byte[] target_buffer, ref int target_size, kowhai_get_symbol_name_t get_name);

        public static int Serialize(Kowhai.kowhai_node_t[] descriptor, byte[] data, out string target, int targetBufferSize, kowhai_get_symbol_name_t getName)
        {
            byte[] targetBuf = new byte[targetBufferSize];
            GCHandle hDesc = GCHandle.Alloc(descriptor, GCHandleType.Pinned);
            GCHandle hData = GCHandle.Alloc(data, GCHandleType.Pinned);
            int result = kowhai_serialize(hDesc.AddrOfPinnedObject(), hData.AddrOfPinnedObject(), data.Length, targetBuf, ref targetBufferSize, getName);
            hData.Free();
            hDesc.Free();
            ASCIIEncoding enc = new ASCIIEncoding();
            target = enc.GetString(targetBuf, 0, targetBufferSize);
            return result;
        }
    }
}
