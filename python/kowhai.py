#!/usr/bin/env python

import sys
import ctypes

# load library
if sys.platform == "win32":
    libname = "kowhai.dll"
else:
    libname = "kowhai.so"
kowhai_lib = ctypes.cdll.LoadLibrary(libname)

# basic type alias'
uint8_t = ctypes.c_uint8
uint16_t = ctypes.c_uint16
uint32_t = ctypes.c_uint32

# kowhai node types
KOW_BRANCH_START = 0x00
KOW_BRANCH_END = 0x01
KOW_INT8 = 0x70
KOW_UINT8 = 0x71
KOW_INT16 = 0x72
KOW_UINT16 = 0x73
KOW_INT32 = 0x74
KOW_UINT32 = 0x75
KOW_FLOAT = 0x76
KOW_CHAR = 0x77

# kowhai node structure
class kowhai_node_t(ctypes.Structure):
    _pack_ = 1
    _fields_ = [('type', uint16_t),
                ('symbol', uint16_t),
                ('count', uint16_t),
                ('tag', uint16_t)]

# kowhai tree structure
class kowhai_tree_t(ctypes.Structure):
    _pack_ = 1
    _fields_ = [('desc', ctypes.POINTER(kowhai_node_t)),
                ('data', ctypes.c_void_p)]

class _kowhai_symbol_parts_t(ctypes.Structure):
    _fields_ = [('name', uint16_t),
                ('array_index', uint16_t)]

# kowhai symbol type
class kowhai_symbol_t(ctypes.Union):
    _fields_ = [('symbol', uint32_t),
                ('parts', _kowhai_symbol_parts_t)]

def KOWHAI_SYMBOL(name, array_index):
    return (array_index << 16) + name

# kowhai function return values
KOW_STATUS_OK                       = 0
KOW_STATUS_INVALID_SYMBOL_PATH      = 1
KOW_STATUS_INVALID_DESCRIPTOR       = 2
KOW_STATUS_INVALID_OFFSET           = 3
KOW_STATUS_NODE_DATA_TOO_SMALL      = 4
KOW_STATUS_INVALID_NODE_TYPE        = 5
KOW_STATUS_PACKET_BUFFER_TOO_SMALL  = 6
KOW_STATUS_INVALID_PROTOCOL_COMMAND = 7
KOW_STATUS_PACKET_BUFFER_TOO_BIG    = 8
KOW_STATUS_TARGET_BUFFER_TOO_SMALL  = 9
KOW_STATUS_BUFFER_INVALID           = 10
KOW_STATUS_SCRATCH_TOO_SMALL        = 11
KOW_STATUS_NOT_FOUND                = 12

#int kowhai_get_node_type_size(uint16_t type);
def get_node_type_size(type_):
    return kowhai_lib.kowhai_get_node_type_size(uint16_t(type_))



if __name__ == "__main__":
    print "test kowhai wrapper"
    print "  KOW_INT32 size is", get_node_type_size(KOW_INT32)




