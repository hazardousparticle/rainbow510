/*******************************************************
 HIDAPI - Multi-Platform library for
 communication with HID devices.

 Alan Ott
 Signal 11 Software

 8/22/2009
 Linux Version - 6/2/2010
 Libusb Version - 8/13/2010

 Copyright 2009, All Rights Reserved.

 At the discretion of the user of this library,
 this software may be licensed under the terms of the
 GNU Public License v3, a BSD-Style license, or the
 original HIDAPI license as outlined in the LICENSE.txt,
 LICENSE-gpl3.txt, LICENSE-bsd.txt, and LICENSE-orig.txt
 files located at the root of the source distribution.
 These files may also be found in the public source
 code repository located at:
        http://github.com/signal11/hidapi .
********************************************************/

#include "hidapi_mod.h"

GKeyboardDevice *hid_enumerate(unsigned short vendor_id, unsigned short product_id)
{
	//the root device, return object
	GKeyboardDevice* kbdevice = NULL;
	GKeyboardDevice* tmpdev = NULL;

	libusb_device **dev_list;
	libusb_device *dev;
	//libusb_device_handle *handle;
	ssize_t num_devs;
	int i = 0;

	num_devs = libusb_get_device_list(NULL, &dev_list);
	if (num_devs < 0)
		return NULL;
	while ((dev = dev_list[i++]) != NULL) {
		struct libusb_device_descriptor desc;
		struct libusb_config_descriptor *conf_desc = NULL;
		int j, k;

		int res = libusb_get_device_descriptor(dev, &desc);

		
		/* HID's are defined at the interface level. */
		if (desc.bDeviceClass != LIBUSB_CLASS_PER_INTERFACE)
			continue;

		res = libusb_get_active_config_descriptor(dev, &conf_desc);
		if (res < 0)
			libusb_get_config_descriptor(dev, 0, &conf_desc);
		if (conf_desc) {
			for (j = 0; j < conf_desc->bNumInterfaces; j++) {
				const libusb_interface *intf = &conf_desc->interface[j];
				for (k = 0; k < intf->num_altsetting; k++) {
					const libusb_interface_descriptor *intf_desc;
					intf_desc = &intf->altsetting[k];
					if (intf_desc->bInterfaceClass == LIBUSB_CLASS_HID) {

						/* Check the VID/PID against the arguments */
						if (vendor_id == desc.idVendor && product_id == desc.idProduct) {
							
							//found a device, add it to the list
							tmpdev = kbdevice;
							kbdevice = new GKeyboardDevice();

							kbdevice->vendor_id = desc.idVendor;
							kbdevice->product_id = desc.idProduct;
							kbdevice->usb_dev = dev;
							kbdevice->interface = intf_desc->bInterfaceNumber;
							kbdevice->next = tmpdev;
						}
					}
				} /* altsettings */
			} /* interfaces */
			libusb_free_config_descriptor(conf_desc);
		}
	}

	libusb_free_device_list(dev_list, 1);

	return kbdevice;
}

void hid_free_enumeration(GKeyboardDevice *devs)
{
	GKeyboardDevice *d = devs;
	while (d) {
		GKeyboardDevice *next = d->next;

		delete d;
		d = next;
	}
	devs = NULL;
}
