#include "Logi510.h"

//set color: write the color information in the given color object to the device.
void setL510_LEDColor(GKeyboardDevice* dev, Color *c)
{
	if (NotValidHandle(dev->device))
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

	libusb_control_transfer(dev->device->device_handle,
				LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE|LIBUSB_ENDPOINT_OUT,
				USB_TRANSFER_REQUEST_SET,
				USB_TRANSFER_VALUE,
				USB_TRANSFER_INDEX,
				(unsigned char *)cmd, sizeof(cmd)/sizeof(char),
				LIBUSB_TRANSFER_TIMEOUT);
}


//get color: returns a pointer to a color object containing the color read from the device.
Color* getL510_LEDColor(GKeyboardDevice* dev)
{
	if (NotValidHandle(dev->device))
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


	int result = libusb_control_transfer(dev->device->device_handle,
			LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE|LIBUSB_ENDPOINT_IN,
			USB_TRANSFER_REQUEST_GET,
			USB_TRANSFER_VALUE,
			USB_TRANSFER_INDEX,
			(unsigned char *)reply, sizeof(reply)/sizeof(char),
			LIBUSB_TRANSFER_TIMEOUT);


	//if the usb transfer failed, return a color of all 0s.
	if (!success(result))
	{
		return new Color();
	}

	Color *c_new = new Color(reply[1], reply[2], reply[3]);
	return c_new;
}
