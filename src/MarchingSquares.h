/* E. Angel, Interactive Computer Graphics */
/* A Top-Down Approach with OpenGL, Third Edition */
/* Addison-Wesley Longman, 2003 */

/* Genarates contours using marching squares */
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

class MarchingSquares
{
public:
	inline MarchingSquares(int x, int y)
	{
		_xCells = x + WINDOW_OFFSET;
		_yCells = y + WINDOW_OFFSET;
		_threshold = INITIAL_THRESHOLD;
	}

	virtual ~MarchingSquares() {}

	/**
	*
	*/
	int cell(double a, double b, double c , double d);

	/**
	*
	*/
	void lines(int num, int i, int j, double a, double b, double c, double d);

	/**
	*
	*/
	void setThreshold(unsigned int threshold) { this->_threshold = threshold; }

private:
	int _xCells;
	int _yCells;

	/* contour value */
	unsigned int _threshold;

	std::map<unsigned int, int**> test; // FIXME

	void draw_one(int num, int i, int j, double a, double b, double c, double d);
	void draw_adjacent(int num, int i, int j, double a, double b, double c, double d);
	void draw_opposite(int num, int i, int j, double a, double b, double c, double d);
};