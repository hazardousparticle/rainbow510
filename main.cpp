#include "hidapi_mod.h"
#include "Logi510.h"

#include <unistd.h>
#include <csignal>

#include <cstdio>
#include <cstdlib>
#include <climits>
#include <iostream>

#include <cstring>

using namespace std;

//delay in ms between color cycles
#define SPEED 100

int main(int argc, char* argv[])
{
	//HANDLE dev_handle = kb_device_open(LOGI_510_VID, LOGI_510_PID);

	GKeyboardDevice *KBDev = kb_device_open(DEVICE_G510_VID, DEVICE_G510_PID);

	HANDLE dev_handle = KBDev->device;

	if (NotValidHandle(dev_handle))//check if g510 found
	{
		cout << "Error accessing G510 keyboard." << endl;

		cout << "    Try find a G110 device..." << endl;

		//get pointers ready to reassign

		delete dev_handle;
		delete KBDev;

		//if not check for a g110
		KBDev = kb_device_open(DEVICE_G510_VID, DEVICE_G110_PID);
		dev_handle = KBDev->device;
		if (NotValidHandle(dev_handle))
		{
			cout << "No G110 keyboard found." << endl;

			return 0;
		}
		else
		{
			cout << "TODO: modify LOGI_510_COLOR_CHANGE_CMD according to G110" << endl;
			cout << "G110 device found but not supported" << endl;
			
			delete dev_handle;
		    delete KBDev;
			
			return 0;
		}
	}

	//no options means read color

	Color *c = getL510_LEDColor(KBDev);

	unsigned char r = 0;
	unsigned char g = 0;
	unsigned char b = 0;
	c->separate(r, g, b);

	printf("Read LED color: red=%02X, green=%02X, blue=%02X\r\n", r, g, b);


	if (argc == 5 &&  !strncmp(argv[1], "write", 5))
	{
		//4 args command: $0 write $2 $3 $4
		//set rgb color of the keyboard
		PissOff_signal = true;

		auto StrToUint8 = [](const char * str) -> unsigned char
		{
			char * bad_chars;

			long l = strtol(str, &bad_chars, 0);

			if (strlen(bad_chars) > 0)
			{
				return 0;
			}


			if (l > UCHAR_MAX)
			{
				l = UCHAR_MAX;
			}
			if (l < 0)
			{
				l = 0;
			}

			return (unsigned char) l;
		};

		r = StrToUint8(argv[2]);
		g = StrToUint8(argv[3]);
		b = StrToUint8(argv[4]);

		delete c;
		c = new Color(r,g,b);

		setL510_LEDColor(KBDev, c);
		printf("Set LED color: red=%02X, green=%02X, blue=%02X\r\n", r, g, b);
	}
	else if (argc == 2 && !strncmp(argv[1], "rainbow", 7) )
	{
		//do rainbow loop until ctrl+c
		
		//quit signal
		
        bool PissOff_signal = false;
		
		auto signal_handler = [](int signal)
		{
		    cout << "Closing the program..." << endl;
	        PissOff_signal = true;
		}
		
		signal(SIGINT, signal_handler);
		signal(SIGHUP, signal_handler);

		Color rainbow = Color();
		HsvColor hsv = HsvColor();

		while (!PissOff_signal)
		{
			//TODO: detect disconnection of the device
			rainbow = Color::RGBFromHSV(&hsv);

			setL510_LEDColor(KBDev, &rainbow);
			hsv.h++;
			if (hsv.h >= 0xff)
			{
				hsv.h = 0;
			}

			usleep(SPEED * 1000);
		}

		setL510_LEDColor(KBDev, c);
	}

	//free the pointers
	kb_device_close(KBDev);
	delete c;
	c = nullptr;
	dev_handle = NULL;
	
	return 0;
}

