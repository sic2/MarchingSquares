#pragma once

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <map>

/* region size */
#define X_MAX 1.0
#define Y_MAX 1.0
#define X_MIN -1.0
#define Y_MIN -1.0

#define WINDOW_OFFSET 50
#define INITIAL_THRESHOLD 0
#define HEIGHT_SCALE 0.35

class Contours
{
public:
	Contours(int** data, unsigned int columns, unsigned int rows, int maxHeight, int minHeight);
	virtual ~Contours();

	/**
	* @param doubleNumberContours
	*		True if the number of contours has to be doubled.
	*		False if the number of contours has to be halved.
	*/
	void changeNumberContours(bool doubleNumberContours);

	void draw();

	unsigned int getNumberContours() { return this->_numberContours; }

private:
	int _xCells;
	int _yCells;
	
	int** _data;

	/*
	* Map height to (number of vertices, vertices)
	* 
	*/
	std::map< int, std::pair<int, float*> > _contoursData;

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

	/**
	*
	*/
	unsigned int numberVertices(unsigned int num);

	/**
	* 
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