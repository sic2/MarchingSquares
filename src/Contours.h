#pragma once

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <vector>

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
	virtual ~Contours() 
	{

	} // TODO - make sure to delete allocated space in _contoursData

	/**
	* FIXME - rename threshold to height
	* @param threshold
	*/
	void resetNumberContours(unsigned int numberContours);

	void draw();

private:

	int _xCells;
	int _yCells;
	
	int** _data;

	/*
	* Map height to (number of vertices, vertices)
	* 
	* XXX - maybe the number of vertices is not necessary
	*/
	std::vector< std::pair<int, float*> > _contoursData;


	unsigned int _numberContours;

	unsigned int _columns;
	unsigned int _rows;
	unsigned int _maxHeight;
	unsigned int _minHeight;


	unsigned int numberLines(int num);

	/**
	* 
	*/
	int cell(unsigned int height, double a, double b, double c , double d);

	/**
	*
	* Note:
	*	Vertices are calculated only once per threshold, unless the 
	* 	overall model threshold is reset.
	*/
	void lines(unsigned int height, float* vertices, unsigned int totalNumberLines,
					int num, int i, int j, double a, double b, double c, double d);

	void draw_one(unsigned int height, float* vertices, unsigned int totalNumberLines, 
		int num, int i, int j, double a, double b, double c, double d);
	void draw_adjacent(unsigned int height, float* vertices, unsigned int totalNumberLines,
		int num, int i, int j, double a, double b, double c, double d);
	void draw_opposite(unsigned int height, float* vertices, unsigned int totalNumberLines,
		int num, int i, int j, double a, double b, double c, double d);
};