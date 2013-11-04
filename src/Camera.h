#pragma once

#include <vector>
#include <cmath>

#define NUMBER_CAMERA_LOCATIONS 3
#define CAMERA_LOCATION_ZERO 0 
#define CAMERA_LOCATION_ONE 1
#define CAMERA_LOCATION_TWO 2

typedef struct Rotation
{
	float angle;
	float x; float y; float z;
} Rotation;

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
	* @return rotations to set the camera to next location
	*/
	inline std::pair< std::string, std::vector< Rotation > > getNextCameraLocation()
	{
		_index++;
		_index = _index % NUMBER_CAMERA_LOCATIONS;
		return getCameraLocation();
	}

	/**
	* Calculate the previous predefined camera location
	* @return rotations to set the camera to prev. location
	*/
	inline std::pair< std::string, std::vector< Rotation > > getPrevCameraLocation()
	{
		_index--;
		if (_index < 0)
		{
			_index = NUMBER_CAMERA_LOCATIONS - 1;
		}
		return getCameraLocation();
	}

	/**
	* @return rotations to dynamically move the camera
	*/
	inline std::vector< Rotation > getDynamicPath()
	{
		std::vector< Rotation > retval;
		for(int i = 0; i < 10000; i++)
		{
			Rotation r;
			r.angle = -0.001f;
			if (i < 1000)
				r.x  = 0.5f;
			else
				r.x = 0.0f;
			r.y = 0.0f;
			if (i < 1000)
				r.z = cos(i/100000);
			else
				r.z = cos(i/10000);
			retval.push_back(r);
		}
		return retval;
	}

	/**
	* @return a vector of all the view names (labels)
	*/
	inline std::vector< std::string > getViewsNames()
	{
		std::vector< std::string > retval;
		retval.push_back("Top");
		retval.push_back("Front Left");
		retval.push_back("Front Right");
		return retval;
	}

	inline void addMenuItem(int item, std::string label)
	{
		_menuIndices.insert(std::pair<int, std::string>(item, label));
	}

	inline std::pair< std::string, std::vector< Rotation > > getViewByIndex(int item)
	{
		std::string label = _menuIndices.find(item)->second;
		if(label.compare("Top") == 0)
		{
			_index = CAMERA_LOCATION_TWO;
			return getCameraLocation();
		}
		if(label.compare("Front Left") == 0)
		{
			_index = CAMERA_LOCATION_ONE;
			return getCameraLocation();
		}
		if(label.compare("Front Right") == 0)
		{
			_index = CAMERA_LOCATION_ZERO;
			return getCameraLocation();
		}
			 
	} 

private:
	int _index;

	inline std::pair< std::string, std::vector< Rotation > > getCameraLocation()
	{
		std::pair< std::string, std::vector< Rotation > > rotations;
		Rotation r_0, r_1;
		switch(_index)
		{
			case CAMERA_LOCATION_ZERO: 
				rotations.first = std::string("Front Right");
				r_0.angle = -1.47f; r_0.x = 1.0f; r_0.y = 0.0f; r_0.z = 0.0f;
				rotations.second.push_back(r_0);
				r_1.angle = -3.14f; r_1.x = 0.0f; r_1.y = 0.0f; r_1.z = 1.0f;
				rotations.second.push_back(r_1);
			break;
			case CAMERA_LOCATION_ONE:
				rotations.first = std::string("Front Left");
				r_0.angle = -1.47f; r_0.x = 1.0f; r_0.y = 0.0f; r_0.z = 0.0f;
				rotations.second.push_back(r_0);
				r_1.angle = -1.47f; r_1.x = 0.0f; r_1.y = 0.0f; r_1.z = 1.0f;
				rotations.second.push_back(r_1);
			break;
			case CAMERA_LOCATION_TWO: // TOP VIEW
				rotations.first = std::string("Top");
				r_0.angle = 0.0f; r_0.x = 0.0f; r_0.y = 0.0f; r_0.z = 0.0f;
				rotations.second.push_back(r_0);
			break;
			default:
				printf("unknown camera location\n");
				break;
		}

		return rotations;
	}

	std::map<int , std::string > _menuIndices;
};