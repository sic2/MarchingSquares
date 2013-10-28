#include "MarchingSquares.h"
#include "DataAcquisition.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <sstream>

#define PLUS_SIGN 43
#define MINUS_SIGN 45

#define ACCURACY_OFFSET 5

int** data;
int rows;
int columns;

MarchingSquares* marchingSquares;

int accuracy;
int minHeight;
int maxHeight;

/*
* Preprocessors
*/
void display();
void myReshape(int w, int h);
void setup();
void cleanup();
void writeBitmapString(void *font, std::string str);
std::string integerToString(int value);

/* ----------- *
 *  Functions  *
 *  ---------- */ 

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);

	int i,j;
	int c;
	int numberContours = 0;
	// TODO - process for different heights
	// set minHeight and maxHeight be looking at file
	for(int h = minHeight; h < maxHeight; h = h + accuracy)
	{
		marchingSquares->setThreshold(h);
		for(i = 0; i < columns; i++) for (j = 0; j < rows; j++) 
	    {
			if (i + 1 != columns && j + 1 != rows)
			{
				c = marchingSquares->cell(data[i][j], data[i+1][j], data[i+1][j+1], data[i][j+1]);
				marchingSquares->lines(c,i,j,data[i][j], data[i+1][j], data[i+1][j+1], data[i][j+1]);
			}
		}
		numberContours++;
	} // outer for loop

	// Display the number of countours currently displayed
	glPushAttrib(GL_COLOR_BUFFER_BIT);
	float color = 1.0f;
	glColor3f(color, color, color);
	glRasterPos2f(0.3f, 0.92f);
   	writeBitmapString(GLUT_BITMAP_9_BY_15, std::string("# Countours: " + integerToString(numberContours)).c_str());
	glPopAttrib();

	glFlush();
}

void myReshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w <= h)
		gluOrtho2D(X_MIN, X_MAX, Y_MIN * (GLfloat) h / (GLfloat) w,
		Y_MAX * (GLfloat) h / (GLfloat) w);
	else
		gluOrtho2D(X_MIN * (GLfloat) w / (GLfloat) h,
		X_MAX * (GLfloat) w / (GLfloat) h, Y_MIN, Y_MAX);
	glMatrixMode(GL_MODELVIEW);
}

void keyboardFunc(unsigned char key, int x, int y)
{
	switch(key)
	{
		case PLUS_SIGN:
		{
			accuracy -= ACCURACY_OFFSET;
		}
		break;
		case MINUS_SIGN:
		{
			accuracy += ACCURACY_OFFSET;
		}
		break;
		default:
		break;
	} // end switch
	glutPostRedisplay();
}

void setup()
{
	accuracy = 100;
	minHeight = 0;
	maxHeight = 800;
}

void cleanup()
{
	DataAcquisition::freeData(data, columns);
	delete marchingSquares;
}

/*
* This function is taken from the book:
* Computer Graphics Through OpenGL: From Theory to Experiments
* by Sumanta Guha
*/
void writeBitmapString(void *font, std::string str)
{  
	int len = str.length() + 1;
	char *cstr = new char[len];
	strcpy(cstr, str.c_str());
	for(int i = 0; i < len && cstr[i] != '\0'; i++)
	{
		glutBitmapCharacter(font, cstr[i]);
	}
	delete [] cstr;
}

std::string integerToString(int value)
{
	std::ostringstream convert; 
	convert << value;
	return convert.str();
}

int main(int argc, char **argv)
{	
	if (argc < 2) // No arguments given, then return with error
	{
		printf("[ ERROR ] :: Please provide a data set file\n");
		//return 1;  // TODO - uncomment
	}

	// Get data before starting any graphics
	std::string fileName = "Data/honolulu_raw.txt"; //argv[1];
	data = DataAcquisition::getData(fileName, &rows, &columns);
	marchingSquares = new MarchingSquares(columns, rows);

	setup();
	glutInit(&argc, argv);

	glutInitWindowSize(500, 500);
	glutCreateWindow(fileName.c_str());

	// Registering callbacks
	glutReshapeFunc(myReshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboardFunc);

	glClearColor(0.0,0.0,0.0,1.0);
	glColor3f(1.0,1.0,1.0);

	glutMainLoop();

	return 0;
}