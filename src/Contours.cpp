#include "Contours.h"

#include <algorithm>

#define INITIAL_NUMBER_CONTOURS 8

Contours::Contours(int** data, unsigned int columns, unsigned int rows, int minHeight, int maxHeight)
{
	this->_data = data;
	this->_columns = columns;
	this->_rows = rows;
	this->_maxHeight = maxHeight;
	this->_minHeight = minHeight;
	this->_numberContours = INITIAL_NUMBER_CONTOURS;

	_xCells = columns + WINDOW_OFFSET;
	_yCells = rows + WINDOW_OFFSET;

	_palette = new Palette(_minHeight, _maxHeight);

	addContours();

	_pickedOnce = false;
}

Contours::~Contours()
{
	// Removing any remaining data about vertices
	for(std::vector< Contour* >::iterator iter = _contoursData.begin(); 
		iter != _contoursData.end(); ++iter)
	{
		delete (*iter);
	}

	delete _palette;
}

/*
 * Calculate the vertices for all the contours once, 
 * until these change.
 */
void Contours::changeNumberContours(bool doubleNumberContours)
{
	/*
	* Even if compilers usually optimise multiplications and divisions by 2,
	* it is always good practice to do this within the source code.
	*/
	if (doubleNumberContours)
	{
		this->_numberContours <<= 1; // multiply by two
		if ((int) this->_numberContours > this->_maxHeight)
		{
			this->_numberContours >>= 1; // divide by two
		}
		addContours();
	}
	else
	{
		this->_numberContours >>= 1; // divide by two
		if (this->_numberContours < 2)
		{
			this->_numberContours <<= 1; // multiply by two
		}
		else
		{
			removeContours();
		}
	}
}

/*
 * Use glDrawArrays to reduce the number of gl-Calls
 * and speedup rendering.
 * 
 * Note: see if glDrawElements can be used, since it should be faster
 */
void Contours::draw()
{
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_COLOR_ARRAY );
	for(std::vector< Contour* >::iterator iter = _contoursData.begin(); 
		iter != _contoursData.end(); ++iter)
	{
		Contour* contour = (*iter);
		glColorPointer(3, GL_FLOAT, 0, contour->colors);
		glVertexPointer(3, GL_FLOAT, 0, contour->vertices);

		glPushName(contour->height);
		glDrawArrays(GL_LINES, 0, contour->numberVertices);
		glPopName();
	}
	glDisableClientState( GL_COLOR_ARRAY );
	glDisableClientState( GL_VERTEX_ARRAY );
}

void Contours::changeColor()
{
	_palette->nextColorScale();

	// Update colors of all contours.
	float RED, GREEN, BLUE;
	for(std::vector< Contour* >::iterator iter = _contoursData.begin(); iter != _contoursData.end(); ++iter)
	{
		Color* color = _palette->getColor((*iter)->height, false);
		RED = color->red; GREEN = color->green; BLUE = color->blue;
		float* colors = (*iter)->colors;
		for(unsigned int i = 0; i < (*iter)->numberVertices * 3; i += 3)
		{
			colors[i] = RED;
			colors[i + 1] = GREEN;
			colors[i + 2] = BLUE;
		}
	}
}

void Contours::updatePickedContour(unsigned int hitIndex)
{
	Contour* contour = (*std::find_if(_contoursData.begin(),_contoursData.end(), CompareFirst(hitIndex)));
	contour->colors = getColorArray(contour->numberVertices * 3, hitIndex, true);
	if (_pickedOnce)
		_lastPickedContour->colors =  getColorArray(_lastPickedContour->numberVertices * 3, _lastPickedContour->height, false);
	_pickedOnce = true;
	_lastPickedContour = contour;
}

/* --------------- *
 * PRIVATE METHODS *
 * --------------- */

void Contours::addContours()
{
	int heightOffset = (_maxHeight - _minHeight) / (this->_numberContours * 1.0);
	if (heightOffset % 2 != 0) heightOffset++; // Correct division of odd numbers

	unsigned int i, j;
	for(int height = _minHeight; height < _maxHeight; height += heightOffset)
	{
		if (_contoursData.size() > 0 && 
			(std::find_if(_contoursData.begin(),_contoursData.end(), CompareFirst(height)) != _contoursData.end()))
				continue;

		// Calculate the total number of lines for this contour.
		unsigned int totalNumberVertices = 0; 
		for(i = 0; i < _columns - 1; ++i) for (j = 0; j < _rows - 1; ++j) 
	    {
			totalNumberVertices += numberVertices(cell(height, _data[i][j], _data[i+1][j], _data[i+1][j+1], _data[i][j+1]));
		}
		
		// Calculate this value here once, rather than multiple times.
		unsigned int totalNumberVertices_X3 = 3 * totalNumberVertices;

		/*
		* Initialise color array for this contour
		* This contour will have always the same color (unless it is picked) 
		*/
		float* colors = getColorArray(totalNumberVertices_X3, height, false);

		/*
		* Initialise and populate vertices array for this contour
		*/
		unsigned int CASE;
		float scaledHeight = HEIGHT_SCALE * (height / (_maxHeight * 1.0));
		float* vertices = new float[totalNumberVertices_X3];
		unsigned int totalNumberCoordinates = 0; // Reset the total number of lines calculated.
		for(i = 0; i < _columns - 1; ++i) for (j = 0; j < _rows - 1; ++j)  // Calculate the lines for this contour.
	    {
			/*
			 * Access array values once, reducing memory fetching.
			 * While this may not significant when fetching 
			 * array[i][j] and then array[i][j+1], since they are adjacent,
			 * it may make the difference when accessing
			 * array[i][j] and then array[i+1][j] since they are not adjacent 
			 * and therefore there is scarse spatial locality.
			 */
			int value_ij = _data[i][j];
			int value_i1j = _data[i+1][j];
			int value_i1j1 = _data[i+1][j+1];
			int value_ij1 = _data[i][j+1];

			CASE = cell(height, value_ij, value_i1j, value_i1j1, value_ij1);
			lines(height, scaledHeight, vertices, totalNumberCoordinates, 
				  CASE, i, j, value_ij, value_i1j, value_i1j1, value_ij1);
			totalNumberCoordinates += numberVertices(CASE) * 3;
		}
	
		Contour* contour = new Contour(height, totalNumberVertices, vertices, colors);
		_contoursData.push_back(contour);
	}
}

void Contours::removeContours()
{
	/*
	* Free previously allocated memory, then erase elements in vector
	*/
	unsigned int half = _contoursData.size() >> 1;
	if (half % 2 != 0) ++half; // Correct division of odd numbers
  	for(std::vector< Contour* >::iterator iter = _contoursData.begin() + half; iter != _contoursData.end(); ++iter) delete (*iter);
	_contoursData.erase(_contoursData.begin() + half, _contoursData.end());
}

float* Contours::getColorArray(unsigned int totalNumberVertices_X3, int height, bool invertColor)
{
	float* colors = new float[totalNumberVertices_X3];
	float RED, GREEN, BLUE;
	Color* color = _palette->getColor(height, invertColor);
	RED = color->red; GREEN = color->green; BLUE = color->blue;
	for(unsigned int i = 0; i < totalNumberVertices_X3; i += 3)
	{
		colors[i] = RED;
		colors[i + 1] = GREEN;
		colors[i + 2] = BLUE;
	}
	return colors;
}

unsigned int Contours::cell(unsigned int height, double a, double b, double c , double d)
{
	/*
	 * Height is cast to double, so that comparison with parameters a, b, c, d
	 * is faster. 
	 * On profiling with pprof I observed that the function cell, on comparing 
	 * integers with double, took about 28-30% of the time within the callee #addContours() 
	 * However, by casting height to double a reduction of about 6-8% was observed.
	 */
	double h = height; 
	unsigned int n = 0;
	if(a > h) n+=1;
	if(b > h) n+=8;
	if(c > h) n+=4;
	if(d > h) n+=2;
	return n;
}

unsigned int Contours::numberVertices(unsigned int num)
{
	unsigned int retval = 0;
	switch(num)
	{
	case 5: case 10: 
		retval += 2;
	case 1: case 2: case 3: case 4: case 6: case 7: case 8: case 9: case 11: case 12: case 13: case 14:    
		retval += 2;
		break;
	case 0: case 15: break;
	}
	return retval;
}

void Contours::lines(unsigned int height, float scaledHeight, float* vertices, unsigned int totalNumberLines,
					unsigned int num, int i, int j, double a, double b, double c, double d)
{
	 switch(num)
	 {
	 case 1: case 2: case 4: case 7: case 8: case 11: case 13: case 14: 
		draw_one(height, scaledHeight, vertices, totalNumberLines, num, i,j,a,b,c,d); 
		break;
	 case 3: case 6: case 9: case 12: 
	    draw_adjacent(height, scaledHeight, vertices, totalNumberLines, num,i,j,a,b,c,d); 
		break;
	 case 5: case 10: 
		draw_opposite(height, scaledHeight, vertices, totalNumberLines, num, i,j,a,b,c,d); 
		break;
	 case 0: case 15: break;
	 }
}

void Contours::draw_one(unsigned int height, float scaledHeight, float* vertices, unsigned int totalNumberLines,
	unsigned int num, int i, int j, double a, double b, double c, double d)
{
  float x1 = 0.0, y1 = 0.0, x2 = 0.0, y2 = 0.0;
  float ox, oy;
  float dx, dy;
  dx=(X_MAX-(X_MIN))/(_xCells-1.0);
  dy=(Y_MAX-(Y_MIN))/(_yCells-1.0);
  ox=X_MIN+i*(X_MAX-(X_MIN))/(_xCells-1.0);
  oy=Y_MIN+j*(Y_MAX-(Y_MIN))/(_yCells-1.0);
    switch(num)
	{
	case 1: case 14:
	  x1=ox;
	  y1=oy+dy*(height-a)/(d-a);
	  x2=ox+dx*(height-a)/(b-a);
	  y2=oy;
	  break;
	case 2: case 13:
	  x1=ox;
	  y1=oy+dy*(height-a)/(d-a);
	  x2=ox+dx*(height-d)/(c-d);
	  y2=oy+dy;
	  break;
	case 4: case 11:
	  x1=ox+dx*(height-d)/(c-d);
	  y1=oy+dy;
	  x2=ox+dx;
	  y2=oy+dy*(height-b)/(c-b);
	  break;
	case 7: case 8:
	  x1=ox+dx*(height-a)/(b-a);
	  y1=oy;
	  x2=ox+dx;
	  y2=oy+dy*(height-b)/(c-b);
	  break;
	}

	vertices[totalNumberLines] = x1;
	vertices[totalNumberLines + 1] = y1;
	vertices[totalNumberLines + 2] = scaledHeight;

	vertices[totalNumberLines + 3] = x2;
	vertices[totalNumberLines + 4] = y2;
	vertices[totalNumberLines + 5] = scaledHeight;
}

void Contours::draw_adjacent(unsigned int height, float scaledHeight, float* vertices, unsigned int totalNumberLines,
	unsigned int num, int i, int j, double a, double b, double c, double d)
{
	float x1 = 0.0, y1 = 0.0, x2 = 0.0, y2 = 0.0;
	float ox, oy;
	float dx, dy;
	dx=(X_MAX-(X_MIN))/(_xCells-1.0);
	dy=(Y_MAX-(Y_MIN))/(_yCells-1.0);
	ox=X_MIN+i*(X_MAX-(X_MIN))/(_xCells-1.0);
	oy=Y_MIN+j*(Y_MAX-(Y_MIN))/(_yCells-1.0);
	switch(num)
	{
	case 3: case 12:
	  x1=ox+dx*(height-a)/(b-a);
	  y1=oy;
	  x2=ox+dx*(height-d)/(c-d);
	  y2=oy+dy;
	  break;
	case 6: case 9:
	  x1=ox;
	  y1=oy+dy*(height-a)/(d-a);
	  x2=ox+dx;
	  y2=oy+dy*(height-b)/(c-b);
	  break;
	}

	vertices[totalNumberLines] = x1;
	vertices[totalNumberLines + 1] = y1;
	vertices[totalNumberLines + 2] = scaledHeight;

	vertices[totalNumberLines + 3] = x2;
	vertices[totalNumberLines + 4] = y2;
	vertices[totalNumberLines + 5] = scaledHeight;

}
void Contours::draw_opposite(unsigned int height, float scaledHeight, float* vertices, unsigned int totalNumberLines,
	unsigned int num, int i, int j, double a, double b, double c, double d)
{
	float x1 = 0.0,y1 = 0.0,x2 = 0.0,y2 = 0.0,x3 = 0.0,y3 = 0.0,x4 = 0.0,y4 = 0.0;
	float ox, oy;
	float dx, dy;
	dx=(X_MAX-(X_MIN))/(_xCells-1.0);
	dy=(Y_MAX-(Y_MIN))/(_yCells-1.0);
	ox=X_MIN+i*(X_MAX-(X_MIN))/(_xCells-1.0);
	oy=Y_MIN+j*(Y_MAX-(Y_MIN))/(_yCells-1.0);
	switch(num)
	{
	case 5: 
	  x1=ox;
	  y1=oy+dy*(height-a)/(d-a);
	  x2=ox+dx*(height-a)/(b-a);
	  y2=oy;
	  x3=ox+dx*(height-d)/(c-d);
	  y3=oy+dy;
	  x4=ox+dx;
	  y4=oy+dy*(height-b)/(c-b);
	  break;
	case 10:
	  x1=ox;
	  y1=oy+dy*(height-a)/(d-a);
	  x2=ox+dx*(height-d)/(c-d);
	  y2=oy+dy;
	  x3=ox+dy*(height-a)/(b-a);
	  y3=oy;
	  x4=ox+dx;
	  y4=oy+dy*(height-b)/(c-b);
	  break;
	}

	vertices[totalNumberLines] = x1;
	vertices[totalNumberLines + 1] = y1;
	vertices[totalNumberLines + 2] = scaledHeight;

	vertices[totalNumberLines + 3] = x2;
	vertices[totalNumberLines + 4] = y2;
	vertices[totalNumberLines + 5] = scaledHeight;

	vertices[totalNumberLines + 6] = x3;
	vertices[totalNumberLines + 7] = y3;
	vertices[totalNumberLines + 8] = scaledHeight;

	vertices[totalNumberLines + 9] = x4;
	vertices[totalNumberLines + 10] = y4;
	vertices[totalNumberLines + 11] = scaledHeight;
}
