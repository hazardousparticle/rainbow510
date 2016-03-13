#include "Logi510.h"

//Temporarily detach from kernel to access then re-attach
//returns true if detached, false if not
static bool pinchFromKernel(GKeyboardDevice* dev)
{
	int res = 0;
	if (libusb_kernel_driver_active(dev->device_handle, dev->interface)) {
		res = libusb_detach_kernel_driver(dev->device_handle,  dev->interface);
	}

	res = libusb_claim_interface(dev->device_handle, dev->interface);

	if (res < 0) {
		return false;
	}

	return true;
}

//close the keyboard device enumeration list
static void kb_device_close(GKeyboardDevice* kbdevice)
{
	if (NotValidHandle(kbdevice->device_handle))
	{
		return;
	}


	if (!libusb_kernel_driver_active(kbdevice->device_handle, kbdevice->interface)) {
		/* release the interface */
		libusb_release_interface(kbdevice->device_handle, kbdevice->interface);

		//return device to the kernel
		libusb_attach_kernel_driver(kbdevice->device_handle, kbdevice->interface);
	}

	/* Close the handle */
	libusb_close(kbdevice->device_handle);
}


//set color: write the color information in the given color object to the device.
void setL510_LEDColor(GKeyboardDevice* dev, Color *c)
{
	if (NotValidHandle(dev->usb_dev))
	{
		return;
	}

	unsigned char cmd[4] = { 0 };


	if (dev->product_id == DEVICE_G510_PID)
	{
		cmd[0] = DEVICE_G510_COLOR_CHANGE_CMD;
	}
	else if (dev->product_id == DEVICE_G110_PID)
	{
		cmd[0] = DEVICE_G110_COLOR_CHANGE_CMD;
	}
	else
	{
		//error, wrong device
		return;
	}


	c->separate(cmd[1], cmd[2], cmd[3]);

	//printf_s("Set LED color: red=%02X, green=%02X, blue=%02X", r, g, b);

	//open the device
	if (libusb_open(dev->usb_dev, &dev->device_handle))
	{
		//failure
		return;
	}


	if (pinchFromKernel(dev))
	{
		libusb_control_transfer(dev->device_handle,
					LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE|LIBUSB_ENDPOINT_OUT,
					USB_TRANSFER_REQUEST_SET,
					USB_TRANSFER_VALUE,
					USB_TRANSFER_INDEX,
					(unsigned char *)cmd, sizeof(cmd)/sizeof(char),
					LIBUSB_TRANSFER_TIMEOUT);
	}

	kb_device_close(dev);
}


//get color: returns a pointer to a color object containing the color read from the device.
Color* getL510_LEDColor(GKeyboardDevice* dev)
{
	if (NotValidHandle(dev->usb_dev))
	{
		return new Color();
	}

	unsigned char reply[4] = { 0 };

	if (dev->product_id == DEVICE_G510_PID)
	{
		reply[0] = DEVICE_G510_COLOR_CHANGE_CMD;
	}
	else if (dev->product_id == DEVICE_G110_PID)
	{
		reply[0] = DEVICE_G110_COLOR_CHANGE_CMD;
	}
	else
	{
		//error, wrong device
		return new Color();
	}

	auto success = [] (int retval) -> bool
	{
		if (retval == LIBUSB_ERROR_TIMEOUT
			|| retval ==  LIBUSB_ERROR_PIPE
			|| retval == LIBUSB_ERROR_NO_DEVICE
			|| retval <= 0)
		{
			return false;
		}
		else
		{
			return true;
		}
	};


	//open the device
	if (libusb_open(dev->usb_dev, &dev->device_handle))
	{
		//Failure to open
		return new Color();
	}

        int result = 0;

	if (pinchFromKernel(dev))
	{
	    result = libusb_control_transfer(dev->device_handle,
			LIBUSB_REQUEST_TYPE_CLASS|
            LIBUSB_RECIPIENT_INTERFACE|LIBUSB_ENDPOINT_IN,
			USB_TRANSFER_REQUEST_GET,
			USB_TRANSFER_VALUE,
			USB_TRANSFER_INDEX,
			(unsigned char *)reply, sizeof(reply)/sizeof(char),
			LIBUSB_TRANSFER_TIMEOUT);
    }

	kb_device_close(dev);

	//if the usb transfer failed, return a color of all 0s.
	if (!success(result))
	{
		return new Color();
	}

	Color *c_new = new Color(reply[1], reply[2], reply[3]);
	return c_new;
}

