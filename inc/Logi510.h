#pragma once
#ifndef _LOGI_510_H_
#define _LOGI_510_H_

//constants

#define DEVICE_G510_COLOR_CHANGE_CMD 0x05

//TODO: find the correct cmd value for device G110
#define DEVICE_G110_COLOR_CHANGE_CMD 0x04

//devices
#define DEVICE_G510_VID 0x046d
#define DEVICE_G510_PID 0xc22d

#define DEVICE_G110_VID DEVICE_G510_VID
//all logitech devices have the same vendor id ;)

#define DEVICE_G110_PID 0xc22a


//usb constants, not to change
//control transfer parameters
#define USB_TRANSFER_REQUEST_GET 0x01
#define USB_TRANSFER_REQUEST_SET 0x09
#define USB_TRANSFER_VALUE 0x0305
#define USB_TRANSFER_INDEX 0x01



//includes
#include "Color.h"
#include "hidapi_mod.h"

//function prototypes
void setL510_LEDColor(GKeyboardDevice* dev, Color *c);
Color* getL510_LEDColor(GKeyboardDevice* dev);

//HANDLE kb_device_open();
GKeyboardDevice*  kb_device_open(unsigned short vendor_id, unsigned short product_id);
void kb_device_close(GKeyboardDevice* kbdevice);

//bool NotValidHandle(HANDLE h); //now a lamda


//time out for libusb control tansfers (ms)
#define LIBUSB_TRANSFER_TIMEOUT 200

#endif
