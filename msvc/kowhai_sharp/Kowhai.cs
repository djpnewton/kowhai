using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;

namespace kowhai_sharp
{
    using uint16_t = UInt16;
    using uint32_t = UInt32;
    using int16_t = Int16;
    using int32_t = Int32;

    public class Kowhai
    {
#if (LINUX)
        public const string dllname = "libkowhai.so";
#else
        public const string dllname = "kowhai.dll";
#endif

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct kowhai_symbol_parts_t
        {
            public uint16_t name;
            public uint16_t array_index;
        }

        [StructLayout(LayoutKind.Explicit)]
        public struct kowhai_symbol_t
        {
            [FieldOffset(0)]
            public uint32_t symbol;
            [FieldOffset(0)]
            public kowhai_symbol_parts_t parts;
            public kowhai_symbol_t(uint32_t symbol)
            {
                this.parts.name = 0;
                this.parts.array_index = 0;
                this.symbol = symbol;
            }
            public kowhai_symbol_t(uint16_t name, uint16_t array_index)
            {
                this.symbol = 0;
                this.parts.name = name;
                this.parts.array_index = array_index;
            }

            public override string ToString()
            {
                return string.Format("name: {0}, array_index: {1}", parts.name, parts.array_index);
            }
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct kowhai_node_t
        {
            public uint16_t type;
            public uint16_t symbol;
            public uint16_t count;
            public uint16_t tag;
            public override string ToString()
            {
                return string.Format("kowhai_node_t - type: {0}, symbol: {1}, count: {2}, tag: {3}", type, symbol, count, tag);
            }
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct kowhai_tree_t
        {
            public IntPtr desc;
            public IntPtr data;
        }

        public struct Tree
        {
            public kowhai_node_t[] Descriptor;
            public byte[] Data;

            public Tree(kowhai_node_t[] desc, byte[] data)
            {
                Descriptor = desc;
                Data = data;
            }

            public static Tree CreateFromNativeTree(kowhai_tree_t nativeTree)
            {
                Tree tree = new Tree();
                int count;
                if (kowhai_get_node_count(nativeTree.desc, out count) == STATUS_OK)
                {
                    // copy descriptor
                    tree.Descriptor = new kowhai_node_t[count];
                    byte[] temp = new byte[count * Marshal.SizeOf(typeof(kowhai_node_t))];
                    Marshal.Copy(nativeTree.desc, temp, 0, temp.Length);
                    GCHandle p = GCHandle.Alloc(tree.Descriptor, GCHandleType.Pinned);
                    Marshal.Copy(temp, 0, p.AddrOfPinnedObject(), temp.Length);
                    p.Free();
                    // copy data
                    if (kowhai_get_node_size(nativeTree.desc, out count) == STATUS_OK)
                    {
                        tree.Data = new byte[count];
                        Marshal.Copy(nativeTree.data, tree.Data, 0, tree.Data.Length);
                    }
                }
                return tree;
            }
        }

        public const int KOW_UNDEFINED_SYMBOL = 65535;
        public const int KOW_TREE_FOR_FUNCTION_CALL_ONLY = 1;

        public const int BRANCH_START = 0;
        public const int BRANCH_END = 1;
        public const int BRANCH_U_START = 2;

        public const int INT8 = 0x0070;
        public const int UINT8 = 0x0071;
        public const int INT16 = 0x0072;
        public const int UINT16 = 0x0073;
        public const int INT32 = 0x0074;
        public const int UINT32 = 0x0075;
        public const int FLOAT = 0x0076;
        public const int CHAR = 0x0077;
        public const int READONLY = 0x8000;

        public static int RawDataType(int dataType)
        {
            return dataType = dataType & (~READONLY);
        }

        public const int STATUS_OK = 0;
        public const int STATUS_INVALID_SYMBOL_PATH = 1;
        public const int STATUS_INVALID_DESCRIPTOR = 2;
        public const int STATUS_INVALID_OFFSET = 3;
        public const int STATUS_NODE_DATA_TOO_SMALL = 4;
        public const int STATUS_INVALID_NODE_TYPE = 5;
        public const int STATUS_PACKET_BUFFER_TOO_SMALL = 6;
        public const int STATUS_INVALID_PROTOCOL_COMMAND = 7;
        public const int STATUS_PACKET_BUFFER_TOO_BIG = 8;
        public const int STATUS_TARGET_BUFFER_TOO_SMALL = 9;
        public const int STATUS_BUFFER_INVALID = 10;
        public const int STATUS_SCRATCH_TOO_SMALL = 11;
        public const int STATUS_NOT_FOUND = 12;
        public const int STATUS_INVALID_SEQUENCE = 13;
        public const int STATUS_NO_DATA = 14;

        [DllImport(dllname, CallingConvention = CallingConvention.Cdecl)]
        public static extern uint32_t kowhai_version();

        [DllImport(dllname, CallingConvention = CallingConvention.Cdecl)]
        public static extern int kowhai_get_node_type_size(uint16_t type);

        [DllImport(dllname, CallingConvention = CallingConvention.Cdecl)]
        public static extern int kowhai_get_node(IntPtr tree_descriptor, int num_symbols, IntPtr symbols, out int offset, ref IntPtr target_node);

        [DllImport(dllname, CallingConvention = CallingConvention.Cdecl)]
        public static extern int kowhai_get_node_size(IntPtr tree_descriptor, out int size);

        [DllImport(dllname, CallingConvention = CallingConvention.Cdecl)]
        public static extern int kowhai_get_node_count(IntPtr tree_descriptor, out int count);

        [DllImport(dllname, CallingConvention = CallingConvention.Cdecl)]
        public static extern int kowhai_read(IntPtr tree_descriptor, IntPtr tree_data, int num_symbols, IntPtr symbols, int read_offset, IntPtr result, int read_size);

        [DllImport(dllname, CallingConvention = CallingConvention.Cdecl)]
        public static extern int kowhai_write(IntPtr tree_descriptor, IntPtr tree_data, int num_symbols, IntPtr symbols, int write_offset, IntPtr value, int write_size);

        [DllImport(dllname, CallingConvention = CallingConvention.Cdecl)]
        public static extern int kowhai_get_int8(IntPtr tree_descriptor, IntPtr tree_data, int num_symbols, IntPtr symbols, out byte result);

        [DllImport(dllname, CallingConvention = CallingConvention.Cdecl)]
        public static extern int kowhai_get_char(IntPtr tree_descriptor, IntPtr tree_data, int num_symbols, IntPtr symbols, out sbyte result);

        [DllImport(dllname, CallingConvention = CallingConvention.Cdecl)]
        public static extern int kowhai_get_int16(IntPtr tree_descriptor, IntPtr tree_data, int num_symbols, IntPtr symbols, out int16_t result);

        [DllImport(dllname, CallingConvention = CallingConvention.Cdecl)]
        public static extern int kowhai_get_int32(IntPtr tree_descriptor, IntPtr tree_data, int num_symbols, IntPtr symbols, out int32_t result);

        [DllImport(dllname, CallingConvention = CallingConvention.Cdecl)]
        public static extern int kowhai_get_float(IntPtr tree_descriptor, IntPtr tree_data, int num_symbols, IntPtr symbols, out float result);

        [DllImport(dllname, CallingConvention = CallingConvention.Cdecl)]
        public static extern int kowhai_set_int8(IntPtr tree_descriptor, IntPtr tree_data, int num_symbols, IntPtr symbols, byte value);

        [DllImport(dllname, CallingConvention = CallingConvention.Cdecl)]
        public static extern int kowhai_set_char(IntPtr tree_descriptor, IntPtr tree_data, int num_symbols, IntPtr symbols, sbyte value);

        [DllImport(dllname, CallingConvention = CallingConvention.Cdecl)]
        public static extern int kowhai_set_int16(IntPtr tree_descriptor, IntPtr tree_data, int num_symbols, IntPtr symbols, int16_t value);

        [DllImport(dllname, CallingConvention = CallingConvention.Cdecl)]
        public static extern int kowhai_set_int32(IntPtr tree_descriptor, IntPtr tree_data, int num_symbols, IntPtr symbols, int32_t value);

        [DllImport(dllname, CallingConvention = CallingConvention.Cdecl)]
        public static extern int kowhai_set_float(IntPtr tree_descriptor, IntPtr tree_data, int num_symbols, IntPtr symbols, float value);

        public static int GetNode(kowhai_node_t[] descriptor, kowhai_symbol_t[] symbols, out int offset, out kowhai_node_t node)
        {
            GCHandle hDesc = GCHandle.Alloc(descriptor, GCHandleType.Pinned);
            GCHandle hSyms = GCHandle.Alloc(symbols, GCHandleType.Pinned);
            IntPtr targetNode = IntPtr.Zero;
            int result = kowhai_get_node(hDesc.AddrOfPinnedObject(), symbols.Length, hSyms.AddrOfPinnedObject(), out offset, ref targetNode);
            hSyms.Free();
            hDesc.Free();
            if (result == Kowhai.STATUS_OK)
                node = (kowhai_node_t)Marshal.PtrToStructure(targetNode, typeof(kowhai_node_t));
            else
                node = new kowhai_node_t();
            return result;
        }

        public static int GetNodeSize(kowhai_node_t[] descriptor, out int size)
        {
            GCHandle h = GCHandle.Alloc(descriptor, GCHandleType.Pinned);
            int result = kowhai_get_node_size(h.AddrOfPinnedObject(), out size);
            h.Free();
            return result;
        }
    }
}
