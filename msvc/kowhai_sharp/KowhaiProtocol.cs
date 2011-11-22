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
        public const int CMD_WRITE_DATA = 0x00;
        public const int CMD_WRITE_DATA_ACK = 0x0F;
        public const int CMD_READ_DATA = 0x10;
        public const int CMD_READ_DATA_ACK = 0x1F;
        public const int CMD_READ_DATA_ACK_END = 0x1E;
        public const int CMD_READ_DESCRIPTOR = 0x20;
        public const int CMD_READ_DESCRIPTOR_ACK = 0x2F;
        public const int CMD_READ_DESCRIPTOR_ACK_END = 0x2E;
        public const int CMD_ERROR_INVALID_TREE_ID = 0xF0;
        public const int CMD_ERROR_INVALID_COMMAND = 0xF1;
        public const int CMD_ERROR_INVALID_SYMBOL_PATH = 0xF2;
        public const int CMD_ERROR_INVALID_PAYLOAD_OFFSET = 0xF3;
        public const int CMD_ERROR_INVALID_PAYLOAD_SIZE = 0xF4;
        public const int CMD_ERROR_UNKNOWN = 0xFF;

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct kowhai_protocol_header_t
        {
            public uint8_t tree_id;
            public uint8_t command;
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
            uint16_t type;
            uint16_t offset;
            uint16_t size;
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

        [StructLayout(LayoutKind.Explicit)]
        public struct kowhai_protocol_payload_spec_t
        {
            [FieldOffset(0)]
            public kowhai_protocol_data_payload_spec_t data;
            [FieldOffset(0)]
            public kowhai_protocol_descriptor_payload_spec_t descriptor;
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

        public static int Parse(byte[] protoPacket, int packetSize, out kowhai_protocol_t protocol)
        {
            GCHandle h = GCHandle.Alloc(protoPacket, GCHandleType.Pinned);
            int result = kowhai_protocol_parse(h.AddrOfPinnedObject(), packetSize, out protocol);
            h.Free();
            return result;
        }

        public static int Create(byte[] protoPacket, int packetSize, ref kowhai_protocol_t protocol, out int bytesRequired)
        {
            GCHandle h = GCHandle.Alloc(protoPacket, GCHandleType.Pinned);
            int result = kowhai_protocol_create(h.AddrOfPinnedObject(), packetSize, ref protocol, out bytesRequired);
            h.Free();
            return result;
        }

        public static void CopyDescriptor(Kowhai.kowhai_node_t[] target, kowhai_protocol_payload_t payload)
        {
            // have to copy twice as Marshal.Copy has no IntPtr->IntPtr overload :((((
            char[] c = new char[payload.spec.descriptor.size];
            Marshal.Copy(payload.buffer, c, 0, c.Length);
            GCHandle h = GCHandle.Alloc(target, GCHandleType.Pinned);
            Marshal.Copy(c, 0, new IntPtr(h.AddrOfPinnedObject().ToInt64() + payload.spec.descriptor.offset), c.Length);
            h.Free();
        }
    }
}
