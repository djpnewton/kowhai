using System;
using System.Runtime.InteropServices;

namespace kowhai_sharp
{
    using uint16_t = UInt16;
    using uint32_t = UInt32;
    using int16_t = Int16;
    using int32_t = Int32;

    public class Kowhai
    {
        const string dllname = "kowhai_dll.dll";

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct kowhai_symbol_parts_t
        {
            uint16_t name;
            uint16_t array_index;
        }

        [StructLayout(LayoutKind.Explicit)]
        public struct kowhai_symbol_t
        {
            [FieldOffset(0)]
            uint32_t symbol;
            [FieldOffset(0)]
            kowhai_symbol_parts_t parts;
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct kowhai_node_t
        {
            uint16_t type;
            uint16_t symbol;
            uint16_t count;
            uint16_t data_type;
        }

        const int NODE_TYPE_BRANCH = 0;
        const int NODE_TYPE_LEAF = 1;
        const int NODE_TYPE_END = 2;

        const int DATA_TYPE_CHAR = 0;
        const int DATA_TYPE_UCHAR = 1;
        const int DATA_TYPE_INT16 = 2;
        const int DATA_TYPE_UINT16 = 3;
        const int DATA_TYPE_INT32 = 4;
        const int DATA_TYPE_UINT32 = 5;
        const int DATA_TYPE_FLOAT = 6;
        const int DATA_TYPE_READONLY = 0x8000;

        const int STATUS_OK = 0;
        const int STATUS_INVALID_SYMBOL_PATH = 1;
        const int STATUS_INVALID_DESCRIPTOR = 2;
        const int STATUS_INVALID_OFFSET = 3;
        const int STATUS_NODE_DATA_TOO_SMALL = 4;
        const int STATUS_INVALID_NODE_TYPE = 5;
        const int STATUS_PACKET_BUFFER_TOO_SMALL = 6;
        const int STATUS_INVALID_PROTOCOL_COMMAND = 7;
        const int STATUS_PACKET_BUFFER_TOO_BIG = 8;

        [DllImport(dllname)]
        public static extern int kowhai_get_data_size();

        [DllImport(dllname)]
        public static extern int kowhai_get_node(IntPtr tree_descriptor, int num_symbols, IntPtr symbols, out int offset, IntPtr target_node);

        [DllImport(dllname)]
        public static extern int kowhai_get_node_size(IntPtr tree_descriptor, out int size);

        [DllImport(dllname)]
        public static extern int kowhai_read(IntPtr tree_descriptor, IntPtr tree_data, int num_symbols, IntPtr symbols, int read_offset, IntPtr result, int read_size);

        [DllImport(dllname)]
        public static extern int kowhai_write(IntPtr tree_descriptor, IntPtr tree_data, int num_symbols, IntPtr symbols, int write_offset, IntPtr value, int write_size);

        [DllImport(dllname)]
        public static extern int kowhai_get_char(IntPtr tree_descriptor, IntPtr tree_data, int num_symbols, IntPtr symbols, out byte result);

        [DllImport(dllname)]
        public static extern int kowhai_get_int16(IntPtr tree_descriptor, IntPtr tree_data, int num_symbols, IntPtr symbols, out int16_t result);

        [DllImport(dllname)]
        public static extern int kowhai_get_int32(IntPtr tree_descriptor, IntPtr tree_data, int num_symbols, IntPtr symbols, out int32_t result);

        [DllImport(dllname)]
        public static extern int kowhai_get_float(IntPtr tree_descriptor, IntPtr tree_data, int num_symbols, IntPtr symbols, out float result);

        [DllImport(dllname)]
        public static extern int kowhai_set_char(IntPtr tree_descriptor, IntPtr tree_data, int num_symbols, IntPtr symbols, char value);

        [DllImport(dllname)]
        public static extern int kowhai_set_int16(IntPtr tree_descriptor, IntPtr tree_data, int num_symbols, IntPtr symbols, int16_t value);

        [DllImport(dllname)]
        public static extern int kowhai_set_int32(IntPtr tree_descriptor, IntPtr tree_data, int num_symbols, IntPtr symbols, int32_t value);

        [DllImport(dllname)]
        public static extern int kowhai_set_float(IntPtr tree_descriptor, IntPtr tree_data, int num_symbols, IntPtr symbols, float value);
    }
}
