#include "DataAcquisition.h"
#include "Palette.h"
#include "Helper.h"

#include "Contours.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <sstream>

/*
* Profiling files
*/
#define PROFILE_FOLDER "Profiling/"
#define PROFILE_FILE PROFILE_FOLDER "squareShift.prof"

/*
* Other definitions
*/ 
#define DATA_FOLDER "Data/"
#define DATA_FILE(file) DATA_FOLDER file

/*
* Key bindings
*/
#define ESC_SIGN 27
#define PLUS_SIGN 43
#define MINUS_SIGN 45
#define C_SIGN 67
#define c_SIGN 99
#define TWO_SIGN 50
#define THREE_SIGN 51
#define SPACE_SIGN 32
#define OPEN_ANGULAR_SIGN 60
#define CLOSE_ANGULAR_SIGN 62

#define RADIANS_ACCURACY 0.1
#define SCALING_FACTOR 0.8
#define RADIANS_TO_DEGREES 57.2957795 // FIXME - use unsigned int for efficiency?

int** data;
int rows;
int columns;

//MarchingSquares* marchingSquares;
Contours* contours;
Palette* palette;

int numberContours;
int minHeight;
int maxHeight;

Color* colorScale;

double zDegrees;
double xDegrees;

bool useOrthoProj;

/*
* Preprocessors
*/
// Callbacks
void display();
void myReshape(int w, int h);
void keyboardFunc(unsigned char key, int x, int y);
void specialFunc(int key, int x, int y);
void idleFunc();
void orthoProj(int w, int h);
void prespectiveProj();

// User defined
void setup();
void cleanup();
void shutDown();
void writeBitmapString(void *font, std::string str);
std::string integerToString(int value);

/* ----------- *
 *  Functions  *
 *  ---------- */ 

/**
*
*/
void display()
{
	glPushMatrix();
	glScalef(SCALING_FACTOR, SCALING_FACTOR, SCALING_FACTOR);
	glRotatef(RADIANS_TO_DEGREES * xDegrees, 1.0, 0.0, 0.0);
	glRotatef(RADIANS_TO_DEGREES * zDegrees, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	contours->draw();
	glPopMatrix();
	
	// Textual information
	Helper::instance().displayText(0.3f, 0.92f, "#Contours: %i", numberContours);
	glFlush();
}

/**
*
*/
void orthoProj(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (w <= h)
	{
		gluOrtho2D(X_MIN, X_MAX, 
			Y_MIN * (GLfloat) h / (GLfloat) w, Y_MAX * (GLfloat) h / (GLfloat) w);
	}
	else
	{
		gluOrtho2D(X_MIN * (GLfloat) w / (GLfloat) h, X_MAX * (GLfloat) w / (GLfloat) h, 
			Y_MIN, Y_MAX);
	}
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

/**
*
*/
void prespectiveProj()
{
	// TODO
}

/**
*
*/
void myReshape(int w, int h)
{
	glViewport(0, 0, w, h);
	if (useOrthoProj)
	{	
		orthoProj(w, h);
	}
	else
	{
		prespectiveProj();
	}
}

/**
*
*/
void keyboardFunc(unsigned char key, int x, int y)
{
	switch(key)
	{
		case PLUS_SIGN:
		{
			numberContours *= 2;
			contours->resetNumberContours(numberContours);
			break;
		}
		case MINUS_SIGN:
		{
			numberContours /= 2;
			if (numberContours < 2)
			{
				numberContours *= 2;
			}
			else {
				
				contours->resetNumberContours(numberContours);
			}
			break;
		}
		case C_SIGN:
		case c_SIGN:
		{
			// TODO  - change color
			break;
		}
		case SPACE_SIGN:
		{
			printf("dynamic path \n");
			break;
		}
		case TWO_SIGN:
		{
			printf("glu ortho 2D\n");
			useOrthoProj = true;
			break;
		}
		case THREE_SIGN:
		{
			printf("perspective projection \n");
			useOrthoProj = false;
			break;
		}
		case OPEN_ANGULAR_SIGN:
		{
			printf("change camera loc - predefined\n");
			break;
		}
		case CLOSE_ANGULAR_SIGN:
		{
			printf("change camera loc - predefined\n");
			break;
		}
		case ESC_SIGN:
		{
			printf("Shutting down\n"); 
			shutDown();
			break;
		}
		default:
		{
			printf("key %d unknown \n", key);
			break;
		}
	} // end switch

	glutPostRedisplay();
}

/**
* Handles the following keys:
* 	- left, right, up, down arrows
* which are used to navigate around the model
* @param
* @param
* @param
*/
void specialFunc(int key, int x, int y)
{
	switch(key)
	{
		// Change color scale
		case GLUT_KEY_LEFT:
			zDegrees -= RADIANS_ACCURACY;
		break;
		case GLUT_KEY_RIGHT:
			zDegrees += RADIANS_ACCURACY;
		break;
		case GLUT_KEY_UP:
			xDegrees -= RADIANS_ACCURACY;
		break;
		case GLUT_KEY_DOWN:
			xDegrees += RADIANS_ACCURACY;
		break;
		default:
			printf("Unused special key %d pressed\n", key);
		break;
	}
	
	glutPostRedisplay();
}

/**
*
*/
void idleFunc()
{	
	// TODO - rotate - needed?
	// zDegrees -= RADIANS_ACCURACY;
	//glutPostRedisplay();
}

/**
* TODO: use constants
*/
void setup()
{
	numberContours = 10; // Initial number of contours
	minHeight = 0;
	maxHeight = 960;
	zDegrees = 0.0;
	xDegrees = 0.0;
	colorScale = palette->nextColorScale(); // FIXME - delete prev allocated color 
	useOrthoProj = true;
}

/**
*
*/
void cleanup()
{
	DataAcquisition::freeData(data, columns);
	//delete marchingSquares;
	delete palette;
}

/**
*
*/
void shutDown()
{
	cleanup();
	exit(EXIT_SUCCESS);
}

/**
*
*/
int main(int argc, char **argv)
{	

Helper::instance().START_PROFILING(PROFILE_FILE);

	if (argc < 2) // No arguments given, then return with error
	{
		printf("[ ERROR ] :: Please provide a data set file\n");
		//return 1;  // TODO - uncomment
	}

	// Get data before starting any graphics
	std::string fileName = DATA_FILE("honolulu_raw.txt"); //argv[1];
	data = DataAcquisition::getData(fileName, &rows, &columns);
	//marchingSquares = new MarchingSquares(columns, rows);
	palette = new Palette();

	contours = new Contours(data, columns, rows, 960, 0);
	contours->resetNumberContours(10);

	setup();
	glutInit(&argc, argv);

	glutInitWindowSize(500, 500);
	glutCreateWindow(fileName.c_str());

	// Registering callbacks
	glutReshapeFunc(myReshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboardFunc);
	glutSpecialFunc(specialFunc);
	glutIdleFunc(idleFunc);

	glClearColor(0.0,0.0,0.0,1.0);
	glColor3f(1.0,1.0,1.0);

	glutMainLoop();

Helper::instance().STOP_PROFILING();

	return 0;
}