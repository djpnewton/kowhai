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
    _fields_ = [('type_', uint16_t),
                ('symbol', uint16_t),
                ('count', uint16_t),
                ('tag', uint16_t)]
    def __str__(self):
        return "kowhai_node_t(%d, %d, %d, %d)" % (self.type_, self.symbol, self.count, self.tag)

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

#uint32_t kowhai_version(void);
def version():
    return kowhai_lib.kowhai_version()

#int kowhai_get_node_type_size(uint16_t type);
def get_node_type_size(type_):
    return kowhai_lib.kowhai_get_node_type_size(uint16_t(type_))

#int kowhai_get_node(const struct kowhai_node_t *node, int num_symbols, const union kowhai_symbol_t *symbols, uint16_t *offset, struct kowhai_node_t **target_node);
def get_node(node, num_symbols, symbols, offset, target_node):
    return kowhai_lib.kowhai_get_node(ctypes.byref(node), ctypes.c_int(num_symbols), ctypes.byref(symbols),
            ctypes.byref(offset), ctypes.byref(target_node))

#int kowhai_get_node_size(const struct kowhai_node_t *node, int *size);
def get_node_size(node, size):
    return kowhai_lib.kowhai_get_node_size(ctypes.byref(node), ctypes.byref(size))

#int kowhai_get_node_count(const struct kowhai_node_t *node, int *count);
def get_node_count(node, count):
    return kowhai_lib.kowhai_get_node_count(ctypes.byref(node), ctypes.byref(count))

if __name__ == "__main__":
    print "test kowhai wrapper"
    print "  kowhai_version() =", version()
    print "  KOW_INT32 size is", get_node_type_size(KOW_INT32)
    descriptor = (kowhai_node_t * 7)(
            kowhai_node_t(KOW_BRANCH_START, 0, 1, 0),
            kowhai_node_t(KOW_UINT8,        1, 1, 1),
            kowhai_node_t(KOW_BRANCH_START, 2, 1, 2),
            kowhai_node_t(KOW_FLOAT,        3, 1, 3),
            kowhai_node_t(KOW_UINT8,        4, 1, 4),
            kowhai_node_t(KOW_BRANCH_END,   2, 0, 5),
            kowhai_node_t(KOW_BRANCH_END,   0, 0, 6)
            )
    num_symbols = 3
    symbols = (kowhai_symbol_t * num_symbols)(
            kowhai_symbol_t(0),
            kowhai_symbol_t(2),
            kowhai_symbol_t(4)
            )
    offset = uint16_t()
    target_node = ctypes.pointer(kowhai_node_t())
    res = get_node(descriptor, num_symbols, symbols, offset, target_node)
    print "  kowhai_get_node() - res: %d, offset: %s, target_node: %s" % (res, offset, target_node.contents)
    size = ctypes.c_int()
    res = get_node_size(descriptor, size)
    print "  kowhai_get_node_size() - res: %d, size: %d" % (res, size.value)
    count = ctypes.c_int()
    res = get_node_count(descriptor, count)
    print "  kowhai_get_node_count() - res: %d, count: %d" % (res, count.value)
