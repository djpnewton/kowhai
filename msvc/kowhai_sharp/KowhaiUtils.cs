using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace kowhai_sharp
{
    public class KowhaiUtils
    {
        [DllImport(Kowhai.dllname, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern int kowhai_merge(ref Kowhai.kowhai_tree_t dst, ref Kowhai.kowhai_tree_t src);

        public static int Merge(Kowhai.Tree destination, Kowhai.Tree source)
        {
            Kowhai.kowhai_tree_t dst, src;
            GCHandle h1 = GCHandle.Alloc(destination.Descriptor, GCHandleType.Pinned);
            dst.desc = h1.AddrOfPinnedObject();
            dst.data = destination.Data;
            GCHandle h2 = GCHandle.Alloc(source.Descriptor, GCHandleType.Pinned);
            src.desc = h2.AddrOfPinnedObject();
            src.data = source.Data;
            int result = kowhai_merge(ref dst, ref src);
            h2.Free();
            h1.Free();
            return result;
        }
    }
}
