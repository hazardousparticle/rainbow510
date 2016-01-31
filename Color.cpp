#include "Color.h"

//constructors for color type
Color::Color()
{
	this->red = 0;
	this->green = 0;
	this->blue = 0;
}

Color::Color(unsigned char red, unsigned char green, unsigned char blue)
{
	this->red = red;
	this->green = green;
	this->blue = blue;
}

void Color::separate(unsigned char & r, unsigned char & g, unsigned char & b)
{
	r = this->red;
	g = this->green;
	b = this->blue;
}

//convert a hsv color to rgb
Color Color::RGBFromHSV(HsvColor *hsv)
{
	if (hsv->s == 0)
		{
			//black
			return Color();
		}

		unsigned char region, remainder, p, q, t;

		region = hsv->h / 43;
		remainder = (hsv->h - (region * 43)) * 6;

		p = (hsv->v * (255 - hsv->s)) >> 8;
		q = (hsv->v * (255 - ((hsv->s * remainder) >> 8))) >> 8;
		t = (hsv->v * (255 - ((hsv->s * (255 - remainder)) >> 8))) >> 8;

		Color c;

		switch (region)
		{
		case 0:
			c = Color(hsv->v, t, p);
			break;
		case 1:
			c = Color(q, hsv->v, p);
			break;
		case 2:
			c = Color(p, hsv->v, t);
			break;
		case 3:
			c = Color(p, q, hsv->v);
			break;
		case 4:
			c = Color(t, p, hsv->v);
			break;
		default:
			c = Color(hsv->v, p, q);
			break;
		}

		return c;
}


//copy operator
void Color::operator=(const Color& c)
{
	this->red = c.red;
	this->blue = c.blue;
	this->green = c.green;
}

HsvColor::HsvColor(unsigned char hue, unsigned char sat, unsigned char var)
{
	this->h = hue;
	this->s = sat;
	this->v = var;
}

HsvColor::HsvColor()
{
	this->h=0;
	this->s=0xff;
	this->v=0xff;
}


