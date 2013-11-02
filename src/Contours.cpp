#include "Contours.h"

#include <algorithm>

#define INITIAL_NUMBER_CONTOURS 8

Contours::Contours(int** data, unsigned int columns, unsigned int rows, int maxHeight, int minHeight)
{
	this->_data = data;
	this->_columns = columns;
	this->_rows = rows;
	this->_maxHeight = maxHeight;
	this->_minHeight = minHeight;
	this->_numberContours = INITIAL_NUMBER_CONTOURS;

	_xCells = columns + WINDOW_OFFSET;
	_yCells = rows + WINDOW_OFFSET;

	addContours();

	_palette = new Palette(_minHeight, _maxHeight);
}

Contours::~Contours()
{
	// Removing any remaining data about vertices
	for(std::vector< std::pair<int, std::pair<int, float*> > >::iterator iter = _contoursData.begin(); 
		iter != _contoursData.end(); ++iter)
	{
		delete iter->second.second;
	}

	delete _palette;
}

/*
 * Calculate the vertices for all the contours once, 
 * until these change.
 *
 * NOTE: bug on halving the number of contours
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
	for(std::vector< std::pair< int, std::pair<int, float*> > >::iterator iter = _contoursData.begin(); 
		iter != _contoursData.end(); ++iter)
	{
		glVertexPointer(3, GL_FLOAT, 0, iter->second.second);

		Color* color = _palette->getColor(iter->first / 960.0); // FIXME
		glColor3f(color->red, color->green, color->blue);
		glDrawArrays(GL_LINES, 0, iter->second.first);
	}
	glDisableClientState(GL_VERTEX_ARRAY);
}

void Contours::changeColor()
{
	_palette->nextColorScale();
}

/* --------------- *
 * PRIVATE METHODS *
 * --------------- */

void Contours::addContours()
{
	int heightOffset = (_maxHeight - _minHeight) / (this->_numberContours * 1.0);
	if (heightOffset % 2 != 0) heightOffset++;

	unsigned int i, j;
	for(int height = _minHeight; height < _maxHeight; height += heightOffset)
	{
		if (_contoursData.size() > 0 && 
			(std::find_if(_contoursData.begin(),_contoursData.end(), CompareFirst(height)) != _contoursData.end()))
				continue;

		// Calculate the total number of lines for this contour.
		unsigned int totalNumberVertices = 0; 
		for(i = 0; i < _columns; ++i) for (j = 0; j < _rows; ++j) 
	    {
			if (i + 1 != _columns && j + 1 != _rows)
			{
				totalNumberVertices += numberVertices(cell(height, _data[i][j], _data[i+1][j], _data[i+1][j+1], _data[i][j+1]));
			}
		}
		
		unsigned int CASE;
		float scaledHeight = HEIGHT_SCALE * (height / 960.0); // FIXME 
		float* vertices = new float[3 * totalNumberVertices];
		unsigned int totalNumberCoordinates = 0; // Reset the total number of lines calculated.
		for(i = 0; i < _columns; ++i) for (j = 0; j < _rows; ++j)  // Calculate the lines for this contour.
	    {
			if (i + 1 != _columns && j + 1 != _rows)
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
		}
		_contoursData.push_back(std::pair<int, std::pair<int, float*> > (height, std::make_pair<int, float*> (totalNumberVertices, vertices)));
	}
}

void Contours::removeContours()
{
	/*
	* Free previously allocated memory, then erase elements in vector
	*/
	int half = _contoursData.size() / 2;
	for(std::vector< std::pair<int, std::pair<int, float*> > >::iterator iter = _contoursData.begin() + half; 
		iter != _contoursData.end(); ++iter)
	{
		delete iter->second.second;
	}
	_contoursData.erase(_contoursData.begin()+half, _contoursData.end());
}

unsigned int Contours::cell(unsigned int height, double a, double b, double c , double d)
{
	int n = 0;
	if(a > height) n+=1;
	if(b > height) n+=8;
	if(c > height) n+=4;
	if(d > height) n+=2;
	return n;
}

unsigned int Contours::numberVertices(unsigned int num)
{
	unsigned int retval = 0;
	switch(num)
	{
	case 5: case 10: 
		retval += 2;
	case 1: case 2: case 4: case 7: case 8: case 11: case 13: case 14: case 3: case 6: case 9: case 12: 
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