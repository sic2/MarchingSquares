#pragma once

#include <stdio.h>
#include <cmath>

/**
* Color - RGB
*/
class Color
{
public:
	inline Color(float red, float green, float blue)
	{
		this->red = red;
		this->green = green;
		this->blue = blue;
	}

	virtual ~Color() {}

	float red;
	float green;
	float blue;
};

// Some constant definitions
#define NUMBER_COLOR_SCALES 4
#define STANDARD_COLOR_SCALE 0
#define RED_BLUE_COLOR_SCALE 1
#define GREY_COLOR_SCALE 2
// Works well for Honolulu map only
#define CARTO_COLOR_SCALE 3 

const float scalingFactor = 1.57f;

class Palette
{
public:
	inline Palette(int minHeight, int maxHeight)
	{
		_index = 0;

		this->_minHeight = minHeight;
		this->_maxHeight = maxHeight;

		_justConstructed = true;
	}

	inline virtual ~Palette() 
	{
		delete _color;
	}

	/**
	* Updates the palette to the next color scheme
	*/
	inline void nextColorScale()
	{
		_index++;
		_index = _index % NUMBER_COLOR_SCALES;
	}

	/**
	* Height is mapped to a different color for a given color scale.
	* @param height of the contour
	* @return color
	*/
	inline Color* getColor(int height)
	{
		float scaledHeight = height * scalingFactor / _maxHeight;

		Color* color;
		float red = 0.0f; 
		float green = 0.0f; 
		float blue = 0.0f;

		switch(_index)
		{
			case STANDARD_COLOR_SCALE:
				red = sin(scaledHeight);
				green = cos(scaledHeight);
				color = new Color(red, green, blue);
			break;
			case RED_BLUE_COLOR_SCALE:
				red = sin(scaledHeight);
				blue = cos(scaledHeight);
				color = new Color(red, green, blue);
			break;
			case GREY_COLOR_SCALE:
				blue = green = red = 0.5 * scaledHeight;
				color = new Color(red, green, blue);
			break;
			case CARTO_COLOR_SCALE:
				if (height < 150)
				{
					green = 0.2 + height / 150.0f;
				}
				else if (height < 500)
				{
					red = sin(height / 500.0f);
					green = height / 500.0f;
				}
				else if (height < 750)
				{
					red = 1.0f;
					green = height / 770.0f;
					blue = 0.10f;
				}
				else if (height < 900)
				{	
					red = sin(scaledHeight);
					green = cos(scaledHeight);
					blue = 0.20f;
				}
				else
				{
					red = sin(scaledHeight);
					blue = green = 0.70f;
				}
				color = new Color(red, green, blue);
				break;
			default:
				printf("palette index unknown\n");
			break;
		}
		if (!_justConstructed)
			delete _color;

		_justConstructed = false;
		_color = color;
		return color;
	}

private: 

	bool _justConstructed;

	int _index;

	int _minHeight;
	int _maxHeight;

	Color* _color;
};