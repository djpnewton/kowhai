using System;
using System.Runtime.InteropServices;

namespace kowhai_sharp
{
    using uint8_t = Byte;
    using uint16_t = UInt16;
    using uint32_t = UInt32;
    using int16_t = Int16;
    using int32_t = Int32;

    public static class KowhaiProtocol
    {
        public const int CMD_GET_TREE_COUNT = 0x00;
        public const int CMD_GET_TREE_COUNT_ACK = 0x0F;
        public const int CMD_WRITE_DATA = 0x10;
        public const int CMD_WRITE_DATA_ACK = 0x1F;
        public const int CMD_READ_DATA = 0x20;
        public const int CMD_READ_DATA_ACK = 0x2F;
        public const int CMD_READ_DATA_ACK_END = 0x2E;
        public const int CMD_READ_DESCRIPTOR = 0x30;
        public const int CMD_READ_DESCRIPTOR_ACK = 0x3F;
        public const int CMD_READ_DESCRIPTOR_ACK_END = 0x3E;
        public const int CMD_GET_FUNCTION_LIST = 0x40;
        public const int CMD_GET_FUNCTION_LIST_ACK = 0x4F;
        public const int CMD_GET_FUNCTION_LIST_ACK_END = 0x4E;
        public const int CMD_GET_FUNCTION_DETAILS = 0x50;
        public const int CMD_GET_FUNCTION_DETAILS_ACK = 0x5F;
        public const int CMD_CALL_FUNCTION = 0x60;
        public const int CMD_CALL_FUNCTION_END = 0x61;
        public const int CMD_CALL_FUNCTION_ACK = 0x6F;
        public const int KOW_CMD_CALL_FUNCTION_RESULT = 0x6E;
        public const int KOW_CMD_CALL_FUNCTION_RESULT_END = 0x6D;
        public const int CMD_ERROR_INVALID_TREE_ID = 0xF0;
        public const int CMD_ERROR_INVALID_COMMAND = 0xF1;
        public const int CMD_ERROR_INVALID_SYMBOL_PATH = 0xF2;
        public const int CMD_ERROR_INVALID_PAYLOAD_OFFSET = 0xF3;
        public const int CMD_ERROR_INVALID_PAYLOAD_SIZE = 0xF4;
        public const int CMD_ERROR_UNKNOWN = 0xFF;

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct kowhai_protocol_header_t
        {
            public uint8_t command;
            public uint16_t id;
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct kowhai_protocol_symbol_spec_t
        {
            public uint8_t count;
            public IntPtr array_;
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct kowhai_protocol_data_payload_memory_spec_t
        {
            public uint16_t type;
            public uint16_t offset;
            public uint16_t size;
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct kowhai_protocol_data_payload_spec_t
        {
            public kowhai_protocol_symbol_spec_t symbols;
            public kowhai_protocol_data_payload_memory_spec_t memory;
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct kowhai_protocol_descriptor_payload_spec_t
        {
            public uint16_t node_count;
            public uint16_t offset;
            public uint16_t size;
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct kowhai_protocol_function_list_t
        {
            public uint16_t list_count;
            public uint16_t offset;
            public uint16_t size;
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct kowhai_protocol_function_details_t
        {
            public uint16_t tree_in_id;
            public uint16_t tree_out_id;
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct kowhai_protocol_function_call_t
        {
            public uint16_t offset;
            public uint16_t size;
        }

        [StructLayout(LayoutKind.Explicit)]
        public struct kowhai_protocol_payload_spec_t
        {
            [FieldOffset(0)]
            public uint8_t tree_count;
            [FieldOffset(0)]
            public kowhai_protocol_data_payload_spec_t data;
            [FieldOffset(0)]
            public kowhai_protocol_descriptor_payload_spec_t descriptor;
            [FieldOffset(0)]
            public kowhai_protocol_function_list_t function_list;
            [FieldOffset(0)]
            public kowhai_protocol_function_details_t function_details;
            [FieldOffset(0)]
            public kowhai_protocol_function_call_t function_call;
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct kowhai_protocol_payload_t
        {
            public kowhai_protocol_payload_spec_t spec;
            public IntPtr buffer;
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct kowhai_protocol_t
        {
            public kowhai_protocol_header_t header;
            public kowhai_protocol_payload_t payload;
        }

        [DllImport(Kowhai.dllname, CallingConvention = CallingConvention.Cdecl)]
        public static extern int kowhai_protocol_get_tree_id(IntPtr proto_packet, int packet_size, out uint8_t tree_id);

        [DllImport(Kowhai.dllname, CallingConvention = CallingConvention.Cdecl)]
        public static extern int kowhai_protocol_parse(IntPtr proto_packet, int packet_size, out kowhai_protocol_t protocol);

        [DllImport(Kowhai.dllname, CallingConvention = CallingConvention.Cdecl)]
        public static extern int kowhai_protocol_create(IntPtr proto_packet, int packet_size, ref kowhai_protocol_t protocol, out int bytes_required);

        [DllImport(Kowhai.dllname, CallingConvention = CallingConvention.Cdecl)]
        public static extern int kowhai_protocol_get_overhead(ref kowhai_protocol_t protocol, out int overhead);

        static void CopyIntPtrs(IntPtr dest, IntPtr source, int size)
        {
            // have to copy twice as Marshal.Copy has no IntPtr->IntPtr overload :((((
            uint8_t[] b = new uint8_t[size];
            Marshal.Copy(source, b, 0, size);
            Marshal.Copy(b, 0, dest, size);
        }

        public static int Parse(byte[] protoPacket, int packetSize, out kowhai_protocol_t protocol)
        {
            GCHandle h = GCHandle.Alloc(protoPacket, GCHandleType.Pinned);
            int result = kowhai_protocol_parse(h.AddrOfPinnedObject(), packetSize, out protocol);
            h.Free();
            return result;
        }

        public static int Parse(byte[] protoPacket, int packetSize, out kowhai_protocol_t protocol, out Kowhai.kowhai_symbol_t[] symbols)
        {
            GCHandle h = GCHandle.Alloc(protoPacket, GCHandleType.Pinned);
            int result = kowhai_protocol_parse(h.AddrOfPinnedObject(), packetSize, out protocol);
            h.Free();
            if (result == Kowhai.STATUS_OK && IsSymbolCommand(protocol.header.command))
            {
                symbols = new Kowhai.kowhai_symbol_t[protocol.payload.spec.data.symbols.count];
                h = GCHandle.Alloc(symbols, GCHandleType.Pinned);
                CopyIntPtrs(h.AddrOfPinnedObject(), protocol.payload.spec.data.symbols.array_, Marshal.SizeOf(typeof(Kowhai.kowhai_symbol_t)) * symbols.Length);
                h.Free();
            }
            else
                symbols = null;
            return result;
        }

        private static bool IsDescriptorCommand(uint8_t command)
        {
            if (command == KowhaiProtocol.CMD_READ_DESCRIPTOR)
                return true;
            if (command == KowhaiProtocol.CMD_READ_DESCRIPTOR_ACK)
                return true;
            if (command == KowhaiProtocol.CMD_READ_DESCRIPTOR_ACK_END)
                return true;
            return false;
        }

        private static bool IsSymbolCommand(uint8_t command)
        {
            switch (command)
            {
                case KowhaiProtocol.CMD_READ_DATA:
                case KowhaiProtocol.CMD_READ_DATA_ACK:
                case KowhaiProtocol.CMD_READ_DATA_ACK_END:
                case KowhaiProtocol.CMD_WRITE_DATA:
                case KowhaiProtocol.CMD_WRITE_DATA_ACK:
                    return true;
            }
            return false;
        }

        public static int Create(byte[] protoPacket, int packetSize, ref kowhai_protocol_t protocol, out int bytesRequired)
        {
            GCHandle h = GCHandle.Alloc(protoPacket, GCHandleType.Pinned);
            int result = kowhai_protocol_create(h.AddrOfPinnedObject(), packetSize, ref protocol, out bytesRequired);
            h.Free();
            return result;
        }

        public static int Create(byte[] protoPacket, int packetSize, ref kowhai_protocol_t protocol, Kowhai.kowhai_symbol_t[] symbols, out int bytesRequired)
        {
            protocol.payload.spec.data.symbols.count = (uint8_t)symbols.Length;
            GCHandle h = GCHandle.Alloc(symbols, GCHandleType.Pinned);
            protocol.payload.spec.data.symbols.array_ = h.AddrOfPinnedObject();
            int result = Create(protoPacket, packetSize, ref protocol, out bytesRequired);
            h.Free();
            return result;
        }

        public static int Create(byte[] protoPacket, int packetSize, ref kowhai_protocol_t protocol, Kowhai.kowhai_symbol_t[] symbols, byte[] buffer, uint16_t offset, out int bytesRequired)
        {
            protocol.payload.spec.data.memory.size = (uint16_t)buffer.Length;
            protocol.payload.spec.data.memory.offset = offset;
            GCHandle h = GCHandle.Alloc(buffer, GCHandleType.Pinned);
            protocol.payload.buffer = h.AddrOfPinnedObject();
            int result = Create(protoPacket, packetSize, ref protocol, symbols, out bytesRequired);
            h.Free();
            return result;
        }

        public static void CopyDescriptor(Kowhai.kowhai_node_t[] target, kowhai_protocol_payload_t payload)
        {
            GCHandle h = GCHandle.Alloc(target, GCHandleType.Pinned);
            CopyIntPtrs(new IntPtr(h.AddrOfPinnedObject().ToInt64() + payload.spec.descriptor.offset), payload.buffer, payload.spec.descriptor.size);
            h.Free();
        }

        public static uint8_t[] GetBuffer(kowhai_protocol_t prot)
        {
            byte[] buffer;
            if (IsDescriptorCommand(prot.header.command))
                buffer = new byte[prot.payload.spec.descriptor.size];
            else
                buffer = new byte[prot.payload.spec.data.memory.size];
            Marshal.Copy(prot.payload.buffer, buffer, 0, buffer.Length);
            return buffer;
        }
    }
}
