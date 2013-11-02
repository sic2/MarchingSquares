#include "DataAcquisition.h"
#include "Helper.h"
#include "Contours.h"
#include "Camera.h"

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

const float RADIANS_ACCURACY = 0.1f;
const float SCALING_FACTOR = 0.8f; // Scales all model
const float RADIANS_TO_DEGREES = 57.2957795f;
const float TEXT_POSITION_X = 0.3f;
const float TEXT_POSITION_Y = 0.92f;

int** data;
unsigned int rows;
unsigned int columns;

Contours* contours;
Camera* camera;

int minHeight;
int maxHeight;

float zRadians = 0.0;
float xRadians = 0.0;
float radians;
float xVect, yVect, zVect;

bool useOrthoProj = true;
bool usePredefinedCamera = false;

int width, height;

/*
* Preprocessors
*/
// Callbacks
void display();
void myReshape(int w, int h);
void keyboardFunc(unsigned char key, int x, int y);
void specialFunc(int key, int x, int y);
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
	if (!usePredefinedCamera)
	{
		glRotatef(RADIANS_TO_DEGREES * xRadians, 1.0, 0.0, 0.0);
		glRotatef(RADIANS_TO_DEGREES * zRadians, 0.0, 0.0, 1.0);
	}
	else
	{
		glRotatef(RADIANS_TO_DEGREES * radians, xVect, yVect, zVect);
	}

	glClear(GL_COLOR_BUFFER_BIT);

	contours->draw();
	glPopMatrix();
	
	// Textual information
	Helper::instance().displayText(TEXT_POSITION_X, TEXT_POSITION_Y, "#Contours: %i", contours->getNumberContours());
	glFlush();
}

/**
* Set the parameters for the ortho-projection
*/
void orthoProj(int w, int h)
{
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
}

/**
* FIXME - this does not change anything
*/
void prespectiveProj()
{
	printf("perspective\n");
	gluPerspective(120, 2, 1, -1); // FIXME
}

/**
*
*/
void myReshape(int w, int h)
{
	width = w;
	height = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
   	glLoadIdentity();
	if (useOrthoProj)
	{	
		orthoProj(w, h);
	}
	else
	{
		prespectiveProj();
	}
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

/**
*
*/
void keyboardFunc(unsigned char key, int x, int y)
{
	switch(key)
	{
		case PLUS_SIGN: contours->changeNumberContours(true);
			break;
		case MINUS_SIGN: contours->changeNumberContours(false);
			break;
		case C_SIGN: case c_SIGN: contours->changeColor();
			break;
		case SPACE_SIGN:
		{
			printf("dynamic path \n");
			break;
		}
		case TWO_SIGN:
		{
			printf("glu ortho 2D\n");
			useOrthoProj = true;
			setup();
			break;
		}
		case THREE_SIGN:
		{
			useOrthoProj = false;
			setup();
			break;
		}
		case OPEN_ANGULAR_SIGN: camera->getPrevCameraLocation(&radians, &xVect, &yVect, &zVect); usePredefinedCamera = true;
			break;
		case CLOSE_ANGULAR_SIGN: camera->getNextCameraLocation(&radians, &xVect, &yVect, &zVect); usePredefinedCamera = true;
			break;
		case ESC_SIGN: shutDown();
			break;
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
		case GLUT_KEY_LEFT: zRadians -= RADIANS_ACCURACY; usePredefinedCamera = false;
			break;
		case GLUT_KEY_RIGHT: zRadians += RADIANS_ACCURACY; usePredefinedCamera = false;
			break;
		case GLUT_KEY_UP: xRadians -= RADIANS_ACCURACY; usePredefinedCamera = false;
			break;
		case GLUT_KEY_DOWN: xRadians += RADIANS_ACCURACY; usePredefinedCamera = false;
			break;
		default: printf("Unused special key %d pressed\n", key);
		break;
	}
	
	glutPostRedisplay();
}

/**
* TODO: use constants
*/
void setup()
{
	// zRadians = 0.0;
	// xRadians = 0.0;
	// useOrthoProj = true;

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
   	glLoadIdentity();
	if (useOrthoProj)
	{	
		orthoProj(width, height);
	}
	else
	{
		prespectiveProj();
	}
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

/**
* Delete any allocated resources
*/
void cleanup()
{
	DataAcquisition::freeData(data, columns);
	delete contours;
	delete camera;
}

/**
* Shuts down the application.
* This involves a call to #cleanup() as well.
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
	contours = new Contours(data, columns, rows, 960, 0);
	camera = new Camera();

	glutInit(&argc, argv);

	glutInitWindowSize(500, 500);
	width = 500; height = 500;
	glutCreateWindow(fileName.c_str());

	// Registering callbacks
	glutReshapeFunc(myReshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboardFunc);
	glutSpecialFunc(specialFunc);

	glClearColor(0.0,0.0,0.0,1.0);
	glColor3f(1.0,1.0,1.0);

	glutMainLoop();

Helper::instance().STOP_PROFILING();
	return 0;
}
