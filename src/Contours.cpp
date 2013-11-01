#include "Contours.h"

Contours::Contours(int** data, unsigned int columns, unsigned int rows, int maxHeight, int minHeight)
{
	this->_data = data;
	this->_columns = columns;
	this->_rows = rows;
	this->_maxHeight = maxHeight;
	this->_minHeight = minHeight;

	_xCells = columns + WINDOW_OFFSET;
	_yCells = rows + WINDOW_OFFSET;
}

/*
 * Calculate the vertices for all the contours once, 
 * until these change.
 */
void Contours::resetNumberContours(unsigned int numberContours)
{
	this->_numberContours = numberContours;	
	int heightOffset = (_maxHeight - _minHeight) / (numberContours * 1.0);
	int i, j, c;
	for(int h = _minHeight; h < _maxHeight; h = h + heightOffset)
	{
		// Calculate the total number of lines for this contour.
		unsigned int totalNumberVertices = 0; 
		for(i = 0; i < _columns; i++) for (j = 0; j < _rows; j++) 
	    {
			if (i + 1 != _columns && j + 1 != _rows)
			{
				c = cell(h, _data[i][j], _data[i+1][j], _data[i+1][j+1], _data[i][j+1]);
				totalNumberVertices += numberLines(c);
			}
		}
		
		float* vertices = new float[3 * totalNumberVertices];
		unsigned int totalNumberCoordinates = 0; // Reset the total number of lines calculated.
		// Calculate the lines for this contour.
		for(i = 0; i < _columns; i++) for (j = 0; j < _rows; j++) 
	    {
			if (i + 1 != _columns && j + 1 != _rows)
			{
				c = cell(h, _data[i][j], _data[i+1][j], _data[i+1][j+1], _data[i][j+1]);
				
				lines(h, vertices, totalNumberCoordinates, 
					  c,i,j, _data[i][j], _data[i+1][j], _data[i+1][j+1], _data[i][j+1]);
				totalNumberCoordinates += numberLines(c) * 3;
			}
		}
		_contoursData.push_back(std::make_pair<int, float*> (totalNumberVertices, vertices));
	}
}

/*
 * Use glDrawArrays to reduce the number of gl-Calls.
 */
void Contours::draw()
{
	glEnableClientState( GL_VERTEX_ARRAY );
	for(std::vector< std::pair<int, float*> >::iterator iter = _contoursData.begin(); 
		iter != _contoursData.end(); ++iter)
	{
		glVertexPointer(3, GL_FLOAT, 0, iter->second);
		glDrawArrays(GL_LINES, 0, iter->first);
	}
	glDisableClientState(GL_VERTEX_ARRAY);
}

/*
 * PRIVATE METHODS
 */

int Contours::cell(unsigned int height, double a, double b, double c , double d)
{
	int n = 0;
	if(a > height) n+=1;
	if(b > height) n+=8;
	if(c > height) n+=4;
	if(d > height) n+=2;
	return n;
}

unsigned int Contours::numberLines(int num)
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

void Contours::lines(unsigned int height, float* vertices, unsigned int totalNumberLines,
					int num, int i, int j, double a, double b, double c, double d)
{
	 switch(num)
	 {
	 case 1: case 2: case 4: case 7: case 8: case 11: case 13: case 14: 
		draw_one(height, vertices, totalNumberLines, num, i,j,a,b,c,d); 
		break;
	 case 3: case 6: case 9: case 12: 
	    draw_adjacent(height, vertices, totalNumberLines, num,i,j,a,b,c,d); 
		break;
	 case 5: case 10: 
		draw_opposite(height, vertices, totalNumberLines, num, i,j,a,b,c,d); 
		break;
	 case 0: case 15: break;
	 }
}

void Contours::draw_one(unsigned int height, float* vertices, unsigned int totalNumberLines,
	int num, int i, int j, double a, double b, double c, double d)
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
	vertices[totalNumberLines + 2] = height / 960.0; // FIXME - scale only once

	vertices[totalNumberLines + 3] = x2;
	vertices[totalNumberLines + 4] = y2;
	vertices[totalNumberLines + 5] = height / 960.0;
}

void Contours::draw_adjacent(unsigned int height, float* vertices, unsigned int totalNumberLines,
	int num, int i, int j, double a, double b, double c, double d)
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
	vertices[totalNumberLines + 2] = height / 960.0;

	vertices[totalNumberLines + 3] = x2;
	vertices[totalNumberLines + 4] = y2;
	vertices[totalNumberLines + 5] = height / 960.0;

}
void Contours::draw_opposite(unsigned int height, float* vertices, unsigned int totalNumberLines,
	int num, int i, int j, double a, double b, double c, double d)
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
	vertices[totalNumberLines + 2] = height / 960.0;

	vertices[totalNumberLines + 3] = x2;
	vertices[totalNumberLines + 4] = y2;
	vertices[totalNumberLines + 5] = height / 960.0;

	vertices[totalNumberLines + 6] = x3;
	vertices[totalNumberLines + 7] = y3;
	vertices[totalNumberLines + 8] = height / 960.0;

	vertices[totalNumberLines + 9] = x4;
	vertices[totalNumberLines + 10] = y4;
	vertices[totalNumberLines + 11] = height / 960.0;
}