#pragma once
#ifndef COLOR_H_
#define COLOR_H_

//somewhere to store a HSV color
class HsvColor
{
private:

public:
	HsvColor(unsigned char hue, unsigned char sat, unsigned char var); //set all
	//HsvColor(unsigned char hue); //assume s and v = 255
	HsvColor(); //assume h=0, s=v=255

	unsigned char h;
	unsigned char s;
	unsigned char v;
};

//somewhere to store an RGB color and make conversions
class Color
{
private:
	unsigned char red;
	unsigned char green;
	unsigned char blue;
public:
	Color();

	Color(unsigned char red, unsigned char green, unsigned char blue);

	//copy operator
	void operator=(const Color& c);

	//separate into rgb components
	void separate(unsigned char & r, unsigned char & g, unsigned char & b);
	
	//get a new rgb color from a hsv color type
	static Color RGBFromHSV(HsvColor *hsv);
};

#endif

