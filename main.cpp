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
#define SPEED 500

volatile bool PissOff_signal = true;
//will skip loop unless told not to
//Signal ,loop to quit
void signal_handler(int signum)
{
    PissOff_signal = true;
}


int main(int argc, char* argv[])
{
    libusb_init(NULL);

    GKeyboardDevice *devs = hid_enumerate(DEVICE_G510_VID, DEVICE_G510_PID);

    GKeyboardDevice *devToUse = kb_device_open(devs, 0); //use the first device

    if (NotValidHandle(devToUse)) //check if g510 found
    {
        cout << "Error accessing G510 keyboard." << endl;

        cout << "    Try find a G110 device..." << endl;

        //get pointers ready to reassign

        //delete devToUse;
        hid_free_enumeration(devs);


        //if not check for a g110

        devs = hid_enumerate(DEVICE_G110_VID, DEVICE_G110_PID);

        devToUse = kb_device_open(devs, 0); //use the first device


        if (NotValidHandle(devToUse))
        {
            cout << "No G110 keyboard found." << endl;
            hid_free_enumeration(devs);
            return 0;
        }
        else
        {
            cout << "TODO: modify LOGI_510_COLOR_CHANGE_CMD according to G110" << endl;
                cout << "G110 device found but not supported" << endl;
            hid_free_enumeration(devs);
            return 0;
        }

    }
    //no options means read color

    Color *c = getL510_LEDColor(devToUse);

    unsigned char r = 0;
    unsigned char g = 0;
    unsigned char b = 0;
    c->separate(r, g, b);

    printf("Read LED color: red=%02X, green=%02X, blue=%02X\r\n", r, g, b);

    //write the color
    if (argc == 4)
    {
        //4 args command: $0 r g b
        //set rgb color of the keyboard

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

        r = StrToUint8(argv[1]);
        g = StrToUint8(argv[2]);
        b = StrToUint8(argv[3]);

        delete c;
        c = new Color(r,g,b);

        setL510_LEDColor(devToUse, c);
        printf("Set LED color: red=%02X, green=%02X, blue=%02X\r\n", r, g, b);
    }
    else if (argc == 2 && !strncmp(argv[1], "rainbow", 7) )
    {
        //do rainbow loop until ctrl+c
        PissOff_signal = false;
        //quit signal
        signal(SIGINT, signal_handler);
        signal(SIGHUP, signal_handler);
        signal(SIGTERM, signal_handler);

        Color rainbow = Color();
        HsvColor hsv = HsvColor();

        while (!PissOff_signal)
        {
            //TODO: detect disconnection of the device
            rainbow = Color::RGBFromHSV(&hsv);

            setL510_LEDColor(devToUse, &rainbow);
            hsv.h++;
            if (hsv.h >= 0xff)
            {
                hsv.h = 0;
            }

            usleep(SPEED * 1000);
        }

        setL510_LEDColor(devToUse, c);
    }

    //free the pointers
    if (devs)
    {
        hid_free_enumeration(devs);
    }

    delete c;
    c = nullptr;
    
    libusb_exit(NULL);
    return 0;
}

