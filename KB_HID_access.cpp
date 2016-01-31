#include "hidapi_mod.h"
#include "Logi510.h"

#include <iostream>

//this will return a device handle for the last device with vid and pid given.
GKeyboardDevice* kb_device_open(unsigned short vendor_id, unsigned short product_id)
{

	GKeyboardDevice *tmpDev = new GKeyboardDevice();

	tmpDev->device = NULL;
	tmpDev->product_id = product_id;

	
	libusb_init(NULL);

	if (!vendor_id || !product_id)
	{
		return NULL;
	}

	struct hid_device_info *devs, *cur_dev;
	std::string path_to_open = "";
	HANDLE handle = NULL;

	devs = hid_enumerate(vendor_id, product_id);
	cur_dev = devs;
	while (cur_dev) {

		//ensure the selected device is the last one found.
		if (cur_dev->vendor_id == vendor_id && 
			cur_dev->product_id == product_id)
		{
				path_to_open = cur_dev->path;
				//break;
		}
		cur_dev = cur_dev->next;
	}

	if (path_to_open.length() > 0) {
		/* Open the device */
		handle = open_device_handle(path_to_open);
		std::cout << "Found device: " << path_to_open << std::endl;
	}

	hid_free_enumeration(devs);

	tmpDev->device = handle;


	return tmpDev;

	//always check the returned value for a valid handle.
	//returns NULL if device could not be connected.
}

//close the keyboard device
void kb_device_close(GKeyboardDevice* kbdevice)
{
	if (NotValidHandle(kbdevice->device))
	{
		return;
	}
	
	/* release the interface */
	libusb_release_interface(kbdevice->device->device_handle, kbdevice->device->interface);
	
	//return device to the kernel
	libusb_attach_kernel_driver(kbdevice->device->device_handle, kbdevice->device->interface);

	/* Close the handle */
	libusb_close(kbdevice->device->device_handle);
	
	//free_hid_device(h);
	
	delete kbdevice->device;
	delete kbdevice;
	
	libusb_exit(NULL);

	std::cout << "Device stopped." << std::endl;
	std::cout << "Device returned to kernel, keyboard functionality restored." << std::endl;
}

//check if device handles are ok
bool NotValidHandle(HANDLE h)
{
	bool retval = false;

	//not null pointer check
	retval = (h != nullptr && h != NULL);

	if (retval)
	{
		//check libusb handle is non zero
		retval = (h->device_handle != 0);
	}

	return !retval; //returns true if bad, false if good
}


/*
auto NotValidHandle = [] (HANDLE h) -> bool
{
	bool retval = false;

	//not null pointer check
	retval = (h != nullptr && h != NULL);

	if (retval)
	{
		//check libusb handle is non zero
		retval = (h->device_handle != 0);
	}

	return !retval; //returns true if bad, false if good
};
*/
