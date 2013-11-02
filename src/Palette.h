#pragma once

#include <stdio.h>
#include <cmath>

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

#define NUMBER_COLOR_SCALES 2
#define STANDARD_COLOR_SCALE 0
#define RED_BLUE_COLOR_SCALE 1

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
	inline Color* getColor(float height)
	{
		float h = height * scalingFactor;

		Color* color;
		float red = 0.0f; 
		float green = 0.0f; 
		float blue = 0.0f;

		switch(_index)
		{
			case STANDARD_COLOR_SCALE:
				red = sin(h);
				green = cos(h);
				color = new Color(red, green, blue);
			break;
			case RED_BLUE_COLOR_SCALE:
				red = sin(h);
				blue = cos(h);
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