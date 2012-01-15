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
            GCHandle h2 = GCHandle.Alloc(destination.Data, GCHandleType.Pinned);
            dst.data = h2.AddrOfPinnedObject();
            GCHandle h3 = GCHandle.Alloc(source.Descriptor, GCHandleType.Pinned);
            src.desc = h3.AddrOfPinnedObject();
            GCHandle h4 = GCHandle.Alloc(source.Data, GCHandleType.Pinned);
            src.data = h4.AddrOfPinnedObject();
            int result = kowhai_merge(ref dst, ref src);
            h4.Free();
            h3.Free();
            h2.Free();
            h1.Free();
            return result;
        }
    }
}
