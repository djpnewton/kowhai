using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace kowhai_sharp
{
    public class KowhaiUtils
    {
        [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet=CharSet.Ansi)]
        public delegate int kowhai_on_diff_t(ref Kowhai.kowhai_node_t left_node, IntPtr left_data, int left_offset, ref Kowhai.kowhai_node_t right_node, IntPtr right_data, int right_offset, int index, int depth);

        [DllImport(Kowhai.dllname, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern int kowhai_diff(ref Kowhai.kowhai_tree_t left, ref Kowhai.kowhai_tree_t right, kowhai_on_diff_t on_diff);

        [DllImport(Kowhai.dllname, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern int kowhai_merge(ref Kowhai.kowhai_tree_t dst, ref Kowhai.kowhai_tree_t src);

        public static int Diff(Kowhai.Tree left, Kowhai.Tree right, kowhai_on_diff_t onDiff)
        {
            Kowhai.kowhai_tree_t l, r;
            GCHandle h1 = GCHandle.Alloc(left.Descriptor, GCHandleType.Pinned);
            l.desc = h1.AddrOfPinnedObject();
            GCHandle h2 = GCHandle.Alloc(left.Data, GCHandleType.Pinned);
            l.data = h2.AddrOfPinnedObject();
            GCHandle h3 = GCHandle.Alloc(right.Descriptor, GCHandleType.Pinned);
            r.desc = h3.AddrOfPinnedObject();
            GCHandle h4 = GCHandle.Alloc(right.Data, GCHandleType.Pinned);
            r.data = h4.AddrOfPinnedObject();
            int result = kowhai_diff(ref l, ref r, onDiff);
            h4.Free();
            h3.Free();
            h2.Free();
            h1.Free();
            return result;
        }

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
