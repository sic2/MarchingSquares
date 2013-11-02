#pragma once

#define NUMBER_CAMERA_LOCATIONS 3
#define CAMERA_LOCATION_ZERO 0 
#define CAMERA_LOCATION_ONE 1
#define CAMERA_LOCATION_TWO 2

class Camera
{
public:
	inline Camera()
	{
		_index = 0;
	}

	virtual ~Camera() {}

	/**
	* Calculate the next predefined camera location
	* @param angle [OUT] - angle of rotation in radians
	* @param x, y, z [OUT] - coordinates of a vector about which rotation happens
	*/
	inline void getNextCameraLocation(float* angle, float* x, float* y, float* z)
	{
		_index++;
		_index = _index % NUMBER_CAMERA_LOCATIONS;
		getCameraLocation(angle, x, y, z);
	}

	/**
	* Calculate the previous predefined camera location
	* @param angle [OUT] - angle of rotation in radians
	* @param x, y, z [OUT] - coordinates of a vector about which rotation happens
	*/
	inline void getPrevCameraLocation(float* angle, float* x, float* y, float* z)
	{
		_index--;
		if (_index < 0)
		{
			_index = NUMBER_CAMERA_LOCATIONS - 1;
		}
		getCameraLocation(angle, x, y, z);
	}

private:
	int _index;

	inline void getCameraLocation(float* angle, float* x, float* y, float* z)
	{
		switch(_index)
		{
			case CAMERA_LOCATION_ZERO:
				*angle = 1.0f;
				*x = 1.0f; *y = 0.0f; *z = 0.0f;
			break;
			case CAMERA_LOCATION_ONE:
				*angle = 0.78f;
				*x = 1.0f; *y = 0.0f; *z = 0.0f;
			break;
			case CAMERA_LOCATION_TWO:
				*angle = 0.38f;
				*x = 1.0f; *y = 0.0f; *z = 0.0f;
			break;
			default:
				printf("unknown camera location\n");
				break;
		}
	}
};