/* E. Angel, Interactive Computer Graphics */
/* A Top-Down Approach with OpenGL, Third Edition */
/* Addison-Wesley Longman, 2003 */

/* Genarates contours using marching squares */
#include "MarchingSquares.h"

int MarchingSquares::cell(double a, double b, double c , double d)
{
    int n=0;
	if(a>this->_threshold) n+=1;
	if(b>this->_threshold) n+=8;
	if(c>this->_threshold) n+=4;
	if(d>this->_threshold) n+=2;
	return n;
}

/* draw line segments for each case */
void MarchingSquares::lines(int num, int i, int j, double a, double b, double c, double d)
{
     switch(num)
	 {
	 case 1: case 2: case 4: case 7: case 8: case 11: case 13: case 14: 
		draw_one(num, i,j,a,b,c,d); 
		break;
	 case 3: case 6: case 9: case 12: 
	    draw_adjacent(num,i,j,a,b,c,d); 
		break;
	 case 5: case 10: 
		draw_opposite(num, i,j,a,b,c,d); 
		break;
	 case 0: case 15: break;
	 }
}

void MarchingSquares::draw_one(int num, int i, int j, double a, double b, double c, double d)
{
  double x1 = 0.0, y1 = 0.0, x2 = 0.0, y2 = 0.0;
  double ox, oy;
  double dx, dy;
  dx=(X_MAX-(X_MIN))/(_xCells-1.0);
  dy=(Y_MAX-(Y_MIN))/(_yCells-1.0);
  ox=X_MIN+i*(X_MAX-(X_MIN))/(_xCells-1.0);
  oy=Y_MIN+j*(Y_MAX-(Y_MIN))/(_yCells-1.0);
    switch(num)
	{
	case 1: case 14:
	  x1=ox;
	  y1=oy+dy*(this->_threshold-a)/(d-a);
	  x2=ox+dx*(this->_threshold-a)/(b-a);
	  y2=oy;
	  break;
	case 2: case 13:
	  x1=ox;
	  y1=oy+dy*(this->_threshold-a)/(d-a);
	  x2=ox+dx*(this->_threshold-d)/(c-d);
	  y2=oy+dy;
	  break;
	case 4: case 11:
	  x1=ox+dx*(this->_threshold-d)/(c-d);
	  y1=oy+dy;
	  x2=ox+dx;
	  y2=oy+dy*(this->_threshold-b)/(c-b);
	  break;
	case 7: case 8:
	  x1=ox+dx*(this->_threshold-a)/(b-a);
	  y1=oy;
	  x2=ox+dx;
	  y2=oy+dy*(this->_threshold-b)/(c-b);
	  break;
	}
  glBegin(GL_LINES);
	glVertex3f(x1, y1, this->_threshold / 960.0); // FIXME - divide by height range
	glVertex3f(x2, y2, this->_threshold / 960.0);
  glEnd();
}

void MarchingSquares::draw_adjacent(int num, int i, int j, double a, double b, double c, double d)
{
  double x1 = 0.0, y1 = 0.0, x2 = 0.0, y2 = 0.0;
  double ox, oy;
  double dx, dy;
  dx=(X_MAX-(X_MIN))/(_xCells-1.0);
  dy=(Y_MAX-(Y_MIN))/(_yCells-1.0);
  ox=X_MIN+i*(X_MAX-(X_MIN))/(_xCells-1.0);
  oy=Y_MIN+j*(Y_MAX-(Y_MIN))/(_yCells-1.0);
  switch(num)
  {
	case 3: case 12:
	  x1=ox+dx*(this->_threshold-a)/(b-a);
	  y1=oy;
	  x2=ox+dx*(this->_threshold-d)/(c-d);
	  y2=oy+dy;
	  break;
	case 6: case 9:
	  x1=ox;
	  y1=oy+dy*(this->_threshold-a)/(d-a);
	  x2=ox+dx;
	  y2=oy+dy*(this->_threshold-b)/(c-b);
	  break;
  }
  glBegin(GL_LINES);
    glVertex3f(x1, y1, this->_threshold / 960.0);
    glVertex3f(x2, y2, this->_threshold/ 960.0);
  glEnd();

}
void MarchingSquares::draw_opposite(int num, int i, int j, double a, double b, double c, double d)
{
  double x1 = 0.0,y1 = 0.0,x2 = 0.0,y2 = 0.0,x3 = 0.0,y3 = 0.0,x4 = 0.0,y4 = 0.0;
  double ox, oy;
  double dx, dy;
  dx=(X_MAX-(X_MIN))/(_xCells-1.0);
  dy=(Y_MAX-(Y_MIN))/(_yCells-1.0);
  ox=X_MIN+i*(X_MAX-(X_MIN))/(_xCells-1.0);
  oy=Y_MIN+j*(Y_MAX-(Y_MIN))/(_yCells-1.0);
  switch(num)
  {
    case 5: 
	  x1=ox;
	  y1=oy+dy*(this->_threshold-a)/(d-a);
	  x2=ox+dx*(this->_threshold-a)/(b-a);
	  y2=oy;
	  x3=ox+dx*(this->_threshold-d)/(c-d);
	  y3=oy+dy;
	  x4=ox+dx;
	  y4=oy+dy*(this->_threshold-b)/(c-b);
	  break;
	case 10:
	  x1=ox;
	  y1=oy+dy*(this->_threshold-a)/(d-a);
	  x2=ox+dx*(this->_threshold-d)/(c-d);
	  y2=oy+dy;
	  x3=ox+dy*(this->_threshold-a)/(b-a);
	  y3=oy;
	  x4=ox+dx;
	  y4=oy+dy*(this->_threshold-b)/(c-b);
	  break;
  }

  glBegin(GL_LINES);
    glVertex3f(x1, y1, this->_threshold / 960.0);
    glVertex3f(x2, y2, this->_threshold/ 960.0);
    glVertex3f(x3, y3, this->_threshold/ 960.0);
    glVertex3f(x4, y4, this->_threshold/ 960.0);
  glEnd();
}