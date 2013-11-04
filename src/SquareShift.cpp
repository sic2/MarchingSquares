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

/*
* Picking constants
*/
#define PICK_TOLLERANCE 5
#define PICK_BUFFER_SIZE 128
#define NO_HITS 0
#define HITS_OFFSET 3

/*
* Constants
*/
const float RADIANS_ACCURACY = 0.1f;
const float SCALING_FACTOR = 0.8f; // Scales all model
const float RADIANS_TO_DEGREES = 57.2957795f;
const float TEXT_POSITION_X = 0.3f;
const float TEXT_POSITION_Y = 0.92f;
const float ZOOM_FACTOR = 0.01;
const float PERSPECTIVE_ANGLE_OF_VIEW = 90.0f;
const float PERSPECTIVE_NEAR = 0.0f;
const float PERSPECTIVE_FAR = 1.0f;

/*
* Global variables
*/
// DATA
int** data;
unsigned int rows;
unsigned int columns;
int minHeight;
int maxHeight;
int heightLastPicked = 0.0f;

// Application objects
Contours* contours;
Camera* camera;
std::pair< std::string, std::vector< Rotation > > rotations;
std::vector< Rotation > dynamicPath;

// Model position values
float zRadians = 0.0;
float xRadians = 0.0;
float zZoom = 1.0;

// View settings
bool useOrthoProj = true;
bool usePredefinedCamera = false;
bool useDynamicPathView = false;

// Window dimensions
int width = 500; int height = 500;

// Picking
unsigned int pickBuffer[PICK_BUFFER_SIZE];

// Menu items
enum MENU_TYPE
{
	MENU_COLOR,
	MENU_PRESPECTIVE,
	MENU_DOUBLE_CONTOURS,
	MENU_HALVE_CONTOURS,
	MENU_DYNAMIC_PATH,
	MENU_EXIT,
	MENU_VIEW // Number of items in menu view specified by numberViews
};
unsigned int numberViews;

/*
* Preprocessors
*/
// Callbacks
void display();
void myReshape(int w, int h);
void keyboardFunc(unsigned char key, int x, int y);
void specialFunc(int key, int x, int y);
void mouse(int button, int state, int x, int y);
void menu(int);

// Projections
void orthoProj(int w, int h);
void prespectiveProj(int w, int h);

// Application defined
void setup(int w, int h);
void cleanup();
void shutDown();
void processPicks(GLint hits, GLuint buffer[], int x, int y);
void picking(int x, int y);

/* ----------- *
 *  Functions  *
 *  ---------- */ 

/**
*
*/
void display()
{
	glPushMatrix();

	if (!useDynamicPathView)
	{
		if (!useOrthoProj)
		{
			gluLookAt(0.0, 0.0, zZoom,
				0.0, 0.0, 0.0, 
				0.0, 1.0, 0.0);
		}
		glScalef(SCALING_FACTOR, SCALING_FACTOR, SCALING_FACTOR);
		if (!usePredefinedCamera)
		{
			glRotatef(RADIANS_TO_DEGREES * xRadians, 1.0, 0.0, 0.0);
			glRotatef(RADIANS_TO_DEGREES * zRadians, 0.0, 0.0, 1.0);
		}
		else
		{
			for (std::vector< Rotation >::iterator iter = rotations.second.begin(); 
			iter != rotations.second.end(); ++iter)
			{
				glRotatef(RADIANS_TO_DEGREES * iter->angle, iter->x, iter->y, iter->z);
			}
		}
		glClear(GL_COLOR_BUFFER_BIT);

		contours->draw();
	}
	else
	{
		for(std::vector< Rotation >::iterator iter = dynamicPath.begin(); 
		iter != dynamicPath.end(); ++iter)
		{
			glRotatef(RADIANS_TO_DEGREES * iter->angle, iter->x, iter->y, iter->z);
			glClear(GL_COLOR_BUFFER_BIT);
			contours->draw();
			glFlush();
		}
		useDynamicPathView = false;
	}
	glPopMatrix();
	
	// Textual information - not affected by roations since outside of push/pop matrices
	Helper::instance().displayText(TEXT_POSITION_X, TEXT_POSITION_Y, "#Contours: %i", contours->getNumberContours());
	Helper::instance().displayText(TEXT_POSITION_X - 0.9, TEXT_POSITION_Y, "#Height Last Picked: %i", heightLastPicked);
	
	std::string MODE = useOrthoProj ? std::string("ORTHO") : std::string("PRESPECTIVE");
	Helper::instance().displayText(TEXT_POSITION_X - 0.9, -1.0f * TEXT_POSITION_Y, MODE.c_str());

	if (usePredefinedCamera) Helper::instance().displayText(TEXT_POSITION_X, -1.0f * TEXT_POSITION_Y, rotations.first.c_str());

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
* Set the prespective projection
*
* Note: this works only on Linux - Lab machines
*/
void prespectiveProj(int w, int h)
{
	gluPerspective(PERSPECTIVE_ANGLE_OF_VIEW, w / h, PERSPECTIVE_NEAR, PERSPECTIVE_FAR); 
}

/**
* Ensures that reshaping the windows keep the model in the correct ratio
*/
void myReshape(int w, int h)
{	
	width = w; height = h;
	setup(w, h);
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
		case SPACE_SIGN: dynamicPath = camera->getDynamicPath(); useDynamicPathView = true;
			break;
		case TWO_SIGN: useOrthoProj = true; setup(width, height);
			break;
		case THREE_SIGN: useOrthoProj = false; setup(width, height);
			break;
		case OPEN_ANGULAR_SIGN: rotations =  camera->getPrevCameraLocation(); usePredefinedCamera = true;
			break;
		case CLOSE_ANGULAR_SIGN: rotations = camera->getNextCameraLocation(); usePredefinedCamera = true;
			break;
		case ESC_SIGN: shutDown();
			break;
		default: printf("key %d unknown \n", key);
			break;
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

// Mouse scrolling snippet from stackoverflow.com
// @see http://stackoverflow.com/questions/14378/using-the-mouse-scrollwheel-in-glut
void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{	
		picking(x, y);
	}
	else if ((button == 3) || (button == 4)) // Wheel reports as button 3(scroll up) and button 4(scroll down)
	{
		if (state == GLUT_UP) return; // Disregard redundant GLUT_UP events
		(button == 3) ? zZoom -= ZOOM_FACTOR : zZoom += ZOOM_FACTOR;
	}
	glutPostRedisplay();
}

/*  
 * Note: this is the exact same method used in practical 2.
 *
 * Note: This method would not work for heights < 0,
 * 		unless index is changed to GLint*
 */
void processPicks(GLint hits, GLuint buffer[], int x, int y)
{
	GLuint* index = (GLuint *)buffer;
	unsigned int maxHeightHit = 0;
	for (unsigned int i = 0; i < (unsigned int) hits && hits > (int) NO_HITS ; ++i)
	{
		index += (int) HITS_OFFSET; // skip zmin and zmax
		if (*index > maxHeightHit) maxHeightHit = *index;
		index++; // next hit 
	}
	heightLastPicked = maxHeightHit;
	if (hits > (int) NO_HITS) contours->updatePickedContour(maxHeightHit);
}

/*
* Note: this is the exact same method used in practical 2.
*/
void picking(int x, int y)
{
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	glSelectBuffer(PICK_BUFFER_SIZE, pickBuffer);
	glRenderMode(GL_SELECT);

	glInitNames();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	gluPickMatrix((GLdouble) x, (GLdouble) (viewport[3] - y), PICK_TOLLERANCE, PICK_TOLLERANCE, viewport);
	
	if (width <= height)
	{
		gluOrtho2D(X_MIN, X_MAX, Y_MIN * (GLfloat) height / (GLfloat) width, Y_MAX * (GLfloat) height / (GLfloat) width);
	}
	else
	{
		gluOrtho2D(X_MIN * (GLfloat) width / (GLfloat) height, X_MAX * (GLfloat) width / (GLfloat) height, Y_MIN, Y_MAX);
	}
	display();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glFlush();

	processPicks(glRenderMode(GL_RENDER), pickBuffer, x, y);
}

// Menu handling function definition
void menu(int item)
{
	if (item >= MENU_VIEW)
	{
		rotations = camera->getViewByIndex(item);
		usePredefinedCamera = true;
	}
	else
	{
		switch (item)
		{
		case MENU_COLOR: contours->changeColor();
			break;
		case MENU_PRESPECTIVE: useOrthoProj = !useOrthoProj; setup(width, height);
			break;
		case MENU_DOUBLE_CONTOURS: contours->changeNumberContours(true);
			break;
		case MENU_HALVE_CONTOURS: contours->changeNumberContours(false);
			break;
		case MENU_DYNAMIC_PATH: dynamicPath = camera->getDynamicPath(); useDynamicPathView = true;
	 		break;
		case MENU_EXIT: shutDown();
		break;
		default: printf("Not valid Menu item \n");
			break;
		} // end switch
	}
	
	glutPostRedisplay();
}

/**
* Setup the projections and other variables (if any)
*/
void setup(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
   	glLoadIdentity();
	if (useOrthoProj)
	{	
		orthoProj(w, h);
	}
	else
	{
		prespectiveProj(w, h);
	}
	glMatrixMode(GL_MODELVIEW);
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
	data = DataAcquisition::getData(fileName, &rows, &columns, &minHeight, &maxHeight);
	contours = new Contours(data, columns, rows, minHeight, maxHeight);
	camera = new Camera();

	glutInit(&argc, argv);

	glutInitWindowSize(width, height);
	glutCreateWindow(fileName.c_str());

	// Registering callbacks
	glutReshapeFunc(myReshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboardFunc);
	glutSpecialFunc(specialFunc);
	glutMouseFunc(mouse);

	// Create menus and submenus: 
	// @see http://www.lighthouse3d.com/opengl/glut/index.php3?11
	int subMenu = glutCreateMenu(menu);
	std::vector< std::string > views = camera->getViewsNames();
	numberViews = views.size();
	int i = 0;
	for (std::vector< std::string >::iterator iter = views.begin(); iter != views.end(); ++iter)
	{
		glutAddMenuEntry((*iter).c_str(), MENU_VIEW + i);
		camera->addMenuItem(MENU_VIEW + i, (*iter).c_str());
		i++;
	}

	glutCreateMenu(menu);
	 // Add menu items
	glutAddMenuEntry("Change Color", MENU_COLOR);
	glutAddSubMenu("Change View", subMenu);
		glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutAddMenuEntry("Change Perspective", MENU_PRESPECTIVE);
	glutAddMenuEntry("Double Contours", MENU_DOUBLE_CONTOURS);
 	glutAddMenuEntry("Halve Contours", MENU_HALVE_CONTOURS);
 	glutAddMenuEntry("Dynamic Path", MENU_DYNAMIC_PATH);
	glutAddMenuEntry("Exit", MENU_EXIT);

        // Associate a mouse button with menu
        glutAttachMenu(GLUT_RIGHT_BUTTON);

	glClearColor(0.0,0.0,0.0,1.0);
	glColor3f(1.0,1.0,1.0);

	glutMainLoop();

Helper::instance().STOP_PROFILING();
	return 0;
}
