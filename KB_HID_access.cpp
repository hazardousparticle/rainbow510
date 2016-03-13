#include "hidapi_mod.h"
#include "Logi510.h"

#include <iostream>


/*
 * return the pointer device in the list at index provided
 */
GKeyboardDevice* kb_device_open(GKeyboardDevice* list, int index = 0)
{
	int curr_dev = 0;

	//go through the list of devices and open the specified
	
	GKeyboardDevice *d = list;
	while (d)
	{

		if (curr_dev == index )
		{
			break;
		}

		curr_dev++;

		d = d->next;
	}

	return d;
}

//check if device handles are ok
bool NotValidHandle(void* h)
{
	bool retval = false;

	//not null pointer check
	retval = (h != nullptr && h != NULL);

	return !retval; //returns true if bad, false if good
}

