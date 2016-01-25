/*******************************************************
 HIDAPI - Multi-Platform library for
 communication with HID devices.

 Alan Ott
 Signal 11 Software

 8/22/2009

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

/** @file
 * @defgroup API hidapi API
 */

#pragma once

#ifndef HIDAPI_H__
#define HIDAPI_H__

#include <libusb-1.0/libusb.h>
#include "iconv.h"


#include <string>

struct hid_device {
	//TODO: remove unnecessary parameters

	/* Handle to the actual device. */
	libusb_device_handle *device_handle;
	
	/* Endpoint information */
	int input_endpoint;
	int output_endpoint;
	int input_ep_max_packet_size;

	/* The interface number of the HID */	
	int interface;
	
	/* Indexes of Strings */
	int manufacturer_index;
	int product_index;
	int serial_index;
	
	/* Whether blocking reads are used */
	int blocking; /* boolean */
	
	/* Read thread objects */
	//pthread_t thread;
	//pthread_mutex_t mutex; /* Protects input_reports */
	//pthread_cond_t condition;
	//pthread_barrier_t barrier; /* Ensures correct startup sequence */
	// shutdown_thread;
	//struct libusb_transfer *transfer;

	/* List of received input reports. */
	//struct input_report *input_reports;

};


//cause i can't be bothered removing windaz references
typedef hid_device* HANDLE;



typedef struct
{
	//the LIBUSB device handle
	HANDLE device;

	//store the PID, to determine which commands to use for each device type
	//G510 or G110
	unsigned short product_id;

}GKeyboardDevice;



/** hidapi info structure */
struct hid_device_info {

	/** Platform-specific device path */
	std::string path;

	/** Device Vendor ID */
	unsigned short vendor_id;
	/** Device Product ID */
	unsigned short product_id;
	
	//std::string serial_number;
	int interface_number;
	
	/** Pointer to the next device */
	struct hid_device_info *next;

	//reserved for future use
	
	/** Manufacturer String */
	//wchar_t *manufacturer_string;
	/** Product string */
	//wchar_t *product_string;
};

/** @brief Enumerate the HID Devices.

	This function returns a linked list of all the HID devices
	attached to the system which match vendor_id and product_id.
	If @p vendor_id and @p product_id are both set to 0, then
	all HID devices will be returned.

	@ingroup API
	@param vendor_id The Vendor ID (VID) of the types of device
		to open.
	@param product_id The Product ID (PID) of the types of
		device to open.

	@returns
		This function returns a pointer to a linked list of type
		struct #hid_device, containing information about the HID devices
		attached to the system, or NULL in the case of failure. Free
		this linked list by calling hid_free_enumeration().
*/
hid_device_info * hid_enumerate(unsigned short vendor_id, unsigned short product_id);

/** @brief Free an enumeration Linked List

	This function frees a linked list created by hid_enumerate().

	@ingroup API
	@param devs Pointer to a list of struct_device returned from
		    	hid_enumerate().
*/
void hid_free_enumeration(struct hid_device_info *devs);

/** @brief Get The Manufacturer String from a HID device.

	@ingroup API
	@param device A device handle returned from hid_open().
	@param string A wide string buffer to put the data into.
	@param maxlen The length of the buffer in multiples of wchar_t.
*/

HANDLE open_device_handle(std::string path);

bool NotValidHandle(HANDLE h);

#endif
