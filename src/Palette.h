#pragma once

class Color
{
public:
	inline Color(float redBase, float greenBase, float blueBase, float redTop, float greenTop, float blueTop)
	{
		this->redBase = redBase;
		this->greenBase = greenBase;
		this->blueBase = blueBase;

		this->redTop = redTop;
		this->greenTop = greenTop;
		this->blueTop = blueTop;
	}

	virtual ~Color() {}

	float redBase;
	float greenBase;
	float blueBase;

	float redTop;
	float greenTop;
	float blueTop;
};

#define NUMBER_COLOR_SCALES 3

class Palette
{
public:
	inline Palette()
	{
		_index = 0;
	}

	virtual ~Palette() {}

	inline Color* nextColorScale()
	{
		_index++;
		_index = _index % NUMBER_COLOR_SCALES;
		printf("index is %d\n", _index);
		switch(_index)
		{
			case 0:
				return new Color(1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
			break;
			case 1:
				return new Color(0.0, 1.0, 0.0, 1.0, 0.0, 0.0);
			break;
			case 2:
				return new Color(1.0, 0.0, 0.0, 0.0, 1.0, 1.0);
			break;
			default:
				printf("palette index unknown\n");
			break;
		}

		return new Color(1.0, 1.0, 1.0, 0.0, 0.0, 0.0);
	}

private: 
	int _index;
};