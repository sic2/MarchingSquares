#include "Helper.h"

// OpenGL + GLUT
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// Std header files
#include <sstream>
#include <cstring>

#define WHITE_COLOR 1.0f

Helper Helper::sInstance;

Helper& Helper::instance()
{
        return sInstance;
}

void Helper::displayText(float x, float y, const char *fmt, ...)
{
	glPushAttrib(GL_COLOR_BUFFER_BIT);
	float color = WHITE_COLOR;
	glColor3f(color, color, color);
	glRasterPos2f(x, y);

	std::string textToDisplay;

	va_list args;
	va_start(args, fmt);
	while (*fmt != '\0')
	{
		const char* tmp = fmt;
		++tmp;
		if (*fmt == '%' && *tmp == 'i')
		{			
			int value = va_arg(args, int);
			textToDisplay += integerToString(value);
			++fmt;
		}
		else
		{
			textToDisplay += *fmt;
		}
		++fmt;
	}
	va_end(args);

	writeBitmapString(GLUT_BITMAP_9_BY_15, textToDisplay.c_str());
	glPopAttrib();
}

/*
* Profiling requires Google Perf Tools to be installed.
* The tools are installed in my Mac, but not in the 
* Lab Linux Machine I use. Therefore, profiling is always
* disabled for the latter machine.
*
* http://goog-perftools.sourceforge.net/
*/
#ifdef __APPLE__
#include <google/profiler.h>

void Helper::START_PROFILING(std::string fileName)
{
	if(PROFILER)
		ProfilerStart(fileName.c_str()); 
}

void Helper::STOP_PROFILING()
{
	if(PROFILER)
		ProfilerStop();
}
#else
 
void Helper::START_PROFILING(std::string fileName) {}
void Helper::STOP_PROFILING() {}

#endif
 

/*
 * PRIVATE METHODS
 */

/*
* This function is taken from the book:
* Computer Graphics Through OpenGL: From Theory to Experiments
* by Sumanta Guha
*/
void Helper::writeBitmapString(void *font, std::string str)
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

std::string Helper::integerToString(int value)
{
	std::ostringstream convert; 
	convert << value;
	return convert.str();
}
