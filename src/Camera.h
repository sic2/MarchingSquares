#pragma once

#include <vector>

#define NUMBER_CAMERA_LOCATIONS 3
#define CAMERA_LOCATION_ZERO 0 
#define CAMERA_LOCATION_ONE 1
#define CAMERA_LOCATION_TWO 2

typedef struct Rotation
{
	float angle;
	float x;
	float y; 
	float z;
} Rotation;

/*
* TODO - return pair <string, vector < Rotation> >
* where string is name of camera
*/
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
	inline std::vector< Rotation > getNextCameraLocation()
	{
		_index++;
		_index = _index % NUMBER_CAMERA_LOCATIONS;
		return getCameraLocation();
	}

	/**
	* Calculate the previous predefined camera location
	* @param angle [OUT] - angle of rotation in radians
	* @param x, y, z [OUT] - coordinates of a vector about which rotation happens
	*/
	inline std::vector< Rotation > getPrevCameraLocation()
	{
		_index--;
		if (_index < 0)
		{
			_index = NUMBER_CAMERA_LOCATIONS - 1;
		}
		return getCameraLocation();
	}

	// inline std::vector< Rotation > getDynamicPath()
	// {
	// 	// TODO
	// }

private:
	int _index;

	inline std::vector< Rotation > getCameraLocation()
	{
		std::vector< Rotation > rotations;
		Rotation r_0, r_1;
		switch(_index)
		{
			case CAMERA_LOCATION_ZERO: 
			printf("location 0\n");
				r_0.angle = -1.47f;
				r_0.x = 1.0f;
				r_0.y = 0.0f;
				r_0.z = 0.0f;
				rotations.push_back(r_0);
				r_1.angle = -3.14f;
				r_1.x = 0.0f;
				r_1.y = 0.0f;
				r_1.z = 1.0f;
				rotations.push_back(r_1);
			break;
			case CAMERA_LOCATION_ONE:
			printf("location 1\n");
				r_0.angle = -1.47f;
				r_0.x = 1.0f;
				r_0.y = 0.0f;
				r_0.z = 0.0f;
				rotations.push_back(r_0);
				r_1.angle = -1.47f;
				r_1.x = 0.0f;
				r_1.y = 0.0f;
				r_1.z = 1.0f;
				rotations.push_back(r_1);
			break;
			case CAMERA_LOCATION_TWO: // TOP VIEW
			printf("location 2\n");
				r_0.angle = 0.0f;
				r_0.x = 0.0f;
				r_0.y = 0.0f;
				r_0.z = 0.0f;
				rotations.push_back(r_0);
			break;
			default:
				printf("unknown camera location\n");
				break;
		}

		return rotations;
	}
};