import sys
sys.path.append("../../python")

import kowhai
import kowhai_protocol

import hidapi

#TODO - more stuff!
print kowhai.kowhai_lib

orig_devinfo = devinfo = hidapi.HID.enumerate(0, 0)

while devinfo:
    print "Path:      ", devinfo.contents.path
    print "Vendor Id: ", hex(devinfo.contents.vendor_id)
    print "Product Id:", hex(devinfo.contents.product_id)
    print "Usage Page:", devinfo.contents.usage_page
    print "Usage:     ", devinfo.contents.usage
    print
    devinfo = devinfo.contents.next

hidapi.HID.free_enumeration(orig_devinfo)
