#ifndef COLORMAP_H
#define COLORMAP_H

// for choosing the options in "Color Map"


enum ColorMap { CMAP_none, CMAP_blue_green, CMAP_dip, CMAP_grey, CMAP_BGYR }; //used to replace index (clearer reading)


struct color_f { float r, g, b; };

inline float clamp(float v) // clamp to [0,1]
{
	float t = v < 0.0f ? 0.0f : v;
	return t > 1.0f ? 1.0f : t;
}

inline color_f getRGB(float v, ColorMap cmap) // map v ([0,1]) to rgb values
{
	color_f c;

	switch (cmap) {
	case(CMAP_none):
		//c.r = c.g = c.b = 0.0f;
		break;
	case(CMAP_blue_green):
		c.r = 0.0f;
		c.g = v;
		c.b = 1 - v;
		break;
	case(CMAP_dip):
		if (v >= 0.0f && v < 0.33f) {
			c.r = ((v / 0.33f) * (255.0f - 129.0f) + 129.0f) / 255.0f;
			c.g = ((v / 0.33f) * 68.0f) / 255.0f;
			c.b = 0.0f;
		}
		else if (v >= 0.33f && v < 0.66) {
			c.r = 1.0f;
			c.g = (((v - 0.33f) / 0.33f) * (255.0f - 68.0f) + 68.0f) / 255.0f;
			c.b = 0.0f;
		}
		else {
			c.r = 1.0f;
			c.g = 1.0f;
			c.b = (v - 0.66f) / 0.33f;
		}
		break;
	case(CMAP_grey):
		c.r = c.g = c.b = v;
		break;
	case(CMAP_BGYR):
		if (v < 0.25) {
			c.r = 0;
			c.g = 4 * v;
			c.b = 1;
		}
		else if (v < 0.5) {
			c.r = 0;
			c.b = 2 - 4 * v;
			c.g = 1;
		}
		else if (v < 0.75) {
			c.r = 4 * v - 2;
			c.b = 0;
			c.g = 1;
		}
		else {
			c.g = 4 * (1 - v);
			c.b = 0;
			c.r = 1;
		}
		break;
	}

	return c;
}

#endif // COLORMAP_H