﻿using System;
using System.Runtime.InteropServices;

namespace kowhai_sharp
{
    using uint16_t = UInt16;
    using uint32_t = UInt32;
    using int16_t = Int16;
    using int32_t = Int32;

    public class Kowhai
    {
        public const string dllname = "kowhai_dll.dll";

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
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct kowhai_node_t
        {
            public uint16_t type;
            public uint16_t symbol;
            public uint16_t count;
            public uint16_t data_type;
        }

        public const int NODE_TYPE_BRANCH = 0;
        public const int NODE_TYPE_LEAF = 1;
        public const int NODE_TYPE_END = 2;

        public const int DATA_TYPE_CHAR = 0;
        public const int DATA_TYPE_UCHAR = 1;
        public const int DATA_TYPE_INT16 = 2;
        public const int DATA_TYPE_UINT16 = 3;
        public const int DATA_TYPE_INT32 = 4;
        public const int DATA_TYPE_UINT32 = 5;
        public const int DATA_TYPE_FLOAT = 6;
        public const int DATA_TYPE_READONLY = 0x8000;

        public static int RawDataType(int dataType)
        {
            return dataType = dataType & (~DATA_TYPE_READONLY);
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

        [DllImport(dllname, CallingConvention = CallingConvention.Cdecl)]
        public static extern int kowhai_get_data_size(int data_type);

        [DllImport(dllname, CallingConvention = CallingConvention.Cdecl)]
        public static extern int kowhai_get_node(IntPtr tree_descriptor, int num_symbols, IntPtr symbols, out int offset, IntPtr target_node);

        [DllImport(dllname, CallingConvention = CallingConvention.Cdecl)]
        public static extern int kowhai_get_node_size(IntPtr tree_descriptor, out int size);

        [DllImport(dllname, CallingConvention = CallingConvention.Cdecl)]
        public static extern int kowhai_read(IntPtr tree_descriptor, IntPtr tree_data, int num_symbols, IntPtr symbols, int read_offset, IntPtr result, int read_size);

        [DllImport(dllname, CallingConvention = CallingConvention.Cdecl)]
        public static extern int kowhai_write(IntPtr tree_descriptor, IntPtr tree_data, int num_symbols, IntPtr symbols, int write_offset, IntPtr value, int write_size);

        [DllImport(dllname, CallingConvention = CallingConvention.Cdecl)]
        public static extern int kowhai_get_char(IntPtr tree_descriptor, IntPtr tree_data, int num_symbols, IntPtr symbols, out byte result);

        [DllImport(dllname, CallingConvention = CallingConvention.Cdecl)]
        public static extern int kowhai_get_int16(IntPtr tree_descriptor, IntPtr tree_data, int num_symbols, IntPtr symbols, out int16_t result);

        [DllImport(dllname, CallingConvention = CallingConvention.Cdecl)]
        public static extern int kowhai_get_int32(IntPtr tree_descriptor, IntPtr tree_data, int num_symbols, IntPtr symbols, out int32_t result);

        [DllImport(dllname, CallingConvention = CallingConvention.Cdecl)]
        public static extern int kowhai_get_float(IntPtr tree_descriptor, IntPtr tree_data, int num_symbols, IntPtr symbols, out float result);

        [DllImport(dllname, CallingConvention = CallingConvention.Cdecl)]
        public static extern int kowhai_set_char(IntPtr tree_descriptor, IntPtr tree_data, int num_symbols, IntPtr symbols, char value);

        [DllImport(dllname, CallingConvention = CallingConvention.Cdecl)]
        public static extern int kowhai_set_int16(IntPtr tree_descriptor, IntPtr tree_data, int num_symbols, IntPtr symbols, int16_t value);

        [DllImport(dllname, CallingConvention = CallingConvention.Cdecl)]
        public static extern int kowhai_set_int32(IntPtr tree_descriptor, IntPtr tree_data, int num_symbols, IntPtr symbols, int32_t value);

        [DllImport(dllname, CallingConvention = CallingConvention.Cdecl)]
        public static extern int kowhai_set_float(IntPtr tree_descriptor, IntPtr tree_data, int num_symbols, IntPtr symbols, float value);
    }
}