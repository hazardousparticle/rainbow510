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

bool initialized = false;
static std::string make_path(libusb_device *dev, int interface_number);

hid_device_info *hid_enumerate(unsigned short vendor_id, unsigned short product_id)
{
	libusb_device **devs;
	libusb_device *dev;
	//libusb_device_handle *handle;
	ssize_t num_devs;
	int i = 0;
	
	hid_device_info *root = NULL; // return object
	hid_device_info *cur_dev = NULL;
	
	//setlocale(LC_ALL,"");
	
	//if (!initialized)
		//hid_init();

	num_devs = libusb_get_device_list(NULL, &devs);
	if (num_devs < 0)
		return NULL;
	while ((dev = devs[i++]) != NULL) {
		struct libusb_device_descriptor desc;
		struct libusb_config_descriptor *conf_desc = NULL;
		int j, k;
		int interface_num = 0;

		int res = libusb_get_device_descriptor(dev, &desc);
		unsigned short dev_vid = desc.idVendor;
		unsigned short dev_pid = desc.idProduct;
		
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
						interface_num = intf_desc->bInterfaceNumber;

						/* Check the VID/PID against the arguments */
						if (vendor_id == dev_vid && product_id == dev_pid) {
							hid_device_info *tmp;

							/* VID/PID match. Create the record. */
							tmp = new hid_device_info();
							if (cur_dev) {
								cur_dev->next = tmp;
							}
							else {
								root = tmp;
							}
							cur_dev = tmp;
							
							/* Fill out the record */
							cur_dev->next = NULL;
							cur_dev->path = make_path(dev, interface_num);
							
							/*
							res = libusb_open(dev, &handle);

							if (res >= 0) {
                                //Serial Number
								if (desc.iSerialNumber > 0)
									cur_dev->serial_number =
										get_usb_string(handle, desc.iSerialNumber);

								libusb_close(handle);
							}*/
							/* VID/PID */
							cur_dev->vendor_id = dev_vid;
							cur_dev->product_id = dev_pid;
						
							/* Interface Number */
							cur_dev->interface_number = interface_num;
						}
					}
				} /* altsettings */
			} /* interfaces */
			libusb_free_config_descriptor(conf_desc);
		}
	}

	libusb_free_device_list(devs, 1);

	return root;
}

void hid_free_enumeration(hid_device_info *devs)
{
	hid_device_info *d = devs;
	while (d) {
		hid_device_info *next = d->next;
		//free(d);
		delete d;
		d = next;
	}
}



static std::string make_path(libusb_device *dev, int interface_number)
{
	char str[64];
	snprintf(str, sizeof(str), "%04x:%04x:%02x",
		libusb_get_bus_number(dev),
		libusb_get_device_address(dev),
		interface_number);
	str[sizeof(str)-1] = '\0';
	
	std::string s = str;

	return s;
}


//opens devices from string path
HANDLE open_device_handle(std::string path)
{
	//TODO: cleanup open device routine

	if (path == "" || path.length() < 1)
	{
		//bad path to device
		return NULL;
	}

	HANDLE dev = new hid_device();//NULL;

	libusb_device **devs;
	libusb_device *usb_dev;
	ssize_t num_devs;
	int res;
	int d = 0;
	
	bool device_found = false;

	//setlocale(LC_ALL,"");
	
	//if (!initialized)
		//hid_init();

	num_devs = libusb_get_device_list(NULL, &devs);
	while ((usb_dev = devs[d++]) != NULL && !device_found) {
		libusb_device_descriptor desc;
		libusb_config_descriptor *conf_desc = NULL;
		int i,j,k;
		libusb_get_device_descriptor(usb_dev, &desc);

		if (libusb_get_active_config_descriptor(usb_dev, &conf_desc) < 0)
			continue;
		for (j = 0; j < conf_desc->bNumInterfaces; j++) {
			const libusb_interface *intf = &conf_desc->interface[j];
			for (k = 0; k < intf->num_altsetting; k++) {
				const libusb_interface_descriptor *intf_desc;
				intf_desc = &intf->altsetting[k];
				if (intf_desc->bInterfaceClass == LIBUSB_CLASS_HID) {
					std::string dev_path = make_path(usb_dev, intf_desc->bInterfaceNumber);
					if (dev_path == path) {
						/* Matched Paths. Open this device */
						device_found = true;
						// OPEN HERE //
						res = libusb_open(usb_dev, &dev->device_handle);
						if (res < 0) {
							//free(dev_path);
 							break;
						}
					
						/* Detach the kernel driver, but only if the
						   device is managed by the kernel */
						/*TODO: figure out how to open device without detaching kernel,
						 * need kernel attached to use as keyboard
						 */
#if 1
						if (libusb_kernel_driver_active(dev->device_handle, intf_desc->bInterfaceNumber) == 1) {
							res = libusb_detach_kernel_driver(dev->device_handle, intf_desc->bInterfaceNumber);
							if (res < 0) {
								libusb_close(dev->device_handle);
								//LOG("Unable to detach Kernel Driver\n");
								//free(dev_path);
								break;
							}
						}
#endif
						res = libusb_claim_interface(dev->device_handle, intf_desc->bInterfaceNumber);
						if (res < 0) {
							//LOG("can't claim interface %d: %d\n", intf_desc->bInterfaceNumber, res);
							//free(dev_path);
							libusb_close(dev->device_handle);
							break;
						}

						/* Store off the string descriptor indexes */
						//dev->serial_index       = desc.iSerialNumber;

						/* Store off the interface number */
						dev->interface = intf_desc->bInterfaceNumber;
												
						/* Find the INPUT and OUTPUT endpoints. An
						   OUTPUT endpoint is not required. */
						for (i = 0; i < intf_desc->bNumEndpoints; i++) {
							const struct libusb_endpoint_descriptor *ep
								= &intf_desc->endpoint[i];

							/* Determine the type and direction of this
							   endpoint. */
							int is_interrupt =
								(ep->bmAttributes & LIBUSB_TRANSFER_TYPE_MASK)
							      == LIBUSB_TRANSFER_TYPE_INTERRUPT;
							int is_output = 
								(ep->bEndpointAddress & LIBUSB_ENDPOINT_DIR_MASK)
							      == LIBUSB_ENDPOINT_OUT;
							int is_input = 
								(ep->bEndpointAddress & LIBUSB_ENDPOINT_DIR_MASK)
							      == LIBUSB_ENDPOINT_IN;

							/* Decide whether to use it for input or output. */
							if (dev->input_endpoint == 0 &&
							    is_interrupt && is_input) {
								/* Use this endpoint for INPUT */
								dev->input_endpoint = ep->bEndpointAddress;
								dev->input_ep_max_packet_size = ep->wMaxPacketSize;
							}
							if (dev->output_endpoint == 0 &&
							    is_interrupt && is_output) {
								/* Use this endpoint for OUTPUT */
								dev->output_endpoint = ep->bEndpointAddress;
							}
						}
					}
				}
			}
		}
		libusb_free_config_descriptor(conf_desc);

	}

	libusb_free_device_list(devs, 1);
	
	// If we have a good handle, return it.
	
	if (NotValidHandle(dev))
	{
	    delete dev;
	    dev = NULL;
//	    return NULL;
	}
	return dev;
}
