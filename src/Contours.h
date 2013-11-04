#pragma once

#include "Palette.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <vector>
#include <map>

/* region size */
#define X_MAX 1.0
#define Y_MAX 1.0
#define X_MIN -1.0
#define Y_MIN -1.0

#define WINDOW_OFFSET 50
#define INITIAL_THRESHOLD 0
#define HEIGHT_SCALE 0.35

/**
* Contour represent a contour in an easy and fast way to render
* using glDrawArrays. 
* The following data are stored per contour:
*	- height
* 	- number of vertices making up this contour
* 	- an array of vertices
* 	- an array of colors (mapping the vertices)
*/
class Contour
{
public:
	inline Contour(int height, unsigned int numberVertices, float* vertices, float* colors)
	{
		this->height = height; 
		this->numberVertices = numberVertices;
		this->vertices = vertices;
		this->colors = colors;
	}

	inline virtual ~Contour()
	{
		delete vertices;
		delete colors;
	}

	int height;
	unsigned int numberVertices;
	float* vertices;
	float* colors;
};

// @see http://stackoverflow.com/questions/12008059/find-if-and-stdpair-but-just-one-element
struct CompareFirst
{
  CompareFirst(int val) : val_(val) {}
  bool operator()(const Contour* elem) const {
    return val_ == elem->height;
  }
  private:
    int val_;
};


/**
* The class Contours calculates the contours and stores them in an appropriate data structure.
* One dubling the number of contours, only the new ones are calculated and the opposite is true
* when halving the number of contours.
* By storing the contours once, the draw method is significantly more efficient. 
* In addition, the draw method uses drawArrays rather than multiple calls to glVertex3f. 
* This is another improvements that it is possible to observe when moving around the model, for example.
*/
class Contours
{
public:
	Contours(int** data, unsigned int columns, unsigned int rows, int minHeight, int maxHeight);
	virtual ~Contours();

	/**
	* @param doubleNumberContours
	*		True if the number of contours has to be doubled.
	*		False if the number of contours has to be halved.
	*/
	void changeNumberContours(bool doubleNumberContours);

	/**
	* Draw all contours
	*/
	void draw();

	/**
	* Return the number of current contours
	*/
	unsigned int getNumberContours() { return this->_numberContours; }

	/**
	* Change the color of the contours
	*/
	void changeColor();

	/*
	* Update the color of the picked contour
	* @param ID of picked contour
	* @return the height of the picked contour
	*/
	void updatePickedContour(unsigned int hitIndex);

private:
	Palette* _palette;

	int _xCells;
	int _yCells;
	
	int** _data;

	/*
	* Vector of (height, (number of vertices, vertices))
	*/
	std::vector< Contour* > _contoursData;
	Contour* _lastPickedContour;
	bool _pickedOnce; // False is no contour has ever been picked

	unsigned int _numberContours;
	unsigned int _columns;
	unsigned int _rows;
	int _maxHeight;
	int _minHeight;

	/*
	* Add contours if necessary
	*/
	void addContours();

	/*
	* Remove contours if necessary.
	*/
	void removeContours();

	/*
	* Create a color array for a contour
	* @param
	* @param
	*/
	float* getColorArray(unsigned int totalNumberVertices_X3, int height, bool invertColor);

	/**
	* Returns the number of vertices for a given marching square case
	* @param num 	marching square case
	* @return 		number of vertices
	*/
	unsigned int numberVertices(unsigned int num);

	/**
	* Returns the marching square case for a given height and 4 sample points
	* @param height
	* @param a, b, c, d 	sample points
	*/
	unsigned int cell(unsigned int height, double a, double b, double c , double d);

	/**
	*
	* Note:
	*	Vertices are calculated only once per threshold, unless the 
	* 	overall model threshold is reset.
	*/
	void lines(unsigned int height, float scaledHeight, float* vertices, unsigned int totalNumberLines,
				unsigned int num, int i, int j, double a, double b, double c, double d);

	void draw_one(unsigned int height, float scaledHeight, float* vertices, unsigned int totalNumberLines, 
					unsigned int num, int i, int j, double a, double b, double c, double d);

	void draw_adjacent(unsigned int height, float scaledHeight, float* vertices, unsigned int totalNumberLines,
					unsigned int num, int i, int j, double a, double b, double c, double d);

	void draw_opposite(unsigned int height, float scaledHeight, float* vertices, unsigned int totalNumberLines,
					unsigned int num, int i, int j, double a, double b, double c, double d);
};
