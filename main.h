#define GLFW_INCLUDE_GLU
#define GLFW_DLL
#include "GL/glew.h"
#include <GLFW/glfw3.h>
#include <GL/glut.h>
#include <GL/gl.h>

void initializeOpenGL(int argc, char **argv);
void initializeMenu();
void processMenuEvents(int option);
