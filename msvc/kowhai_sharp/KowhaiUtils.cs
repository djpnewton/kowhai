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
        public delegate int kowhai_on_diff_t(ref Kowhai.kowhai_node_t left_node, int left_node_index, IntPtr left_data, int left_offset, ref Kowhai.kowhai_node_t right_node, int right_node_index, IntPtr right_data, int right_offset, int index, int depth);

        [DllImport(Kowhai.dllname, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern int kowhai_diff(ref Kowhai.kowhai_tree_t left, ref Kowhai.kowhai_tree_t right, kowhai_on_diff_t on_diff);

        [DllImport(Kowhai.dllname, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern int kowhai_merge(ref Kowhai.kowhai_tree_t dst, ref Kowhai.kowhai_tree_t src);

        [DllImport(Kowhai.dllname, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern int kowhai_create_symbol_path(ref Kowhai.kowhai_node_t[] descriptor, ref Kowhai.kowhai_node_t node, ref Kowhai.kowhai_symbol_t[] target, ref int target_size);

        [DllImport(Kowhai.dllname, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern int kowhai_create_symbol_path2(ref Kowhai.kowhai_tree_t tree, IntPtr target_location, ref Kowhai.kowhai_symbol_t[] target, ref int target_size);

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

        public static int CreateSymbolPath(Kowhai.Tree tree, IntPtr targetLocation, out Kowhai.kowhai_symbol_t[] symbolPath)
        {
            int result, symbolPathLength = 5;
            Kowhai.kowhai_tree_t _tree;
            GCHandle hTreeDesc = GCHandle.Alloc(tree.Descriptor, GCHandleType.Pinned);
            _tree.desc = hTreeDesc.AddrOfPinnedObject();
            GCHandle hTreeData = GCHandle.Alloc(tree.Data, GCHandleType.Pinned);
            _tree.data = hTreeData.AddrOfPinnedObject();
            do
            {
                symbolPath = new Kowhai.kowhai_symbol_t[symbolPathLength];
                result = kowhai_create_symbol_path2(ref _tree, targetLocation, ref symbolPath, ref symbolPathLength);
                symbolPathLength *= 2;
            }
            while (result == Kowhai.STATUS_TARGET_BUFFER_TOO_SMALL);
            hTreeData.Free();
            hTreeDesc.Free();
            return result;
        }
    }
}
