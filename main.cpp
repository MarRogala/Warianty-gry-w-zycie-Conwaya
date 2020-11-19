//g++ -g -w -Wall -std=c++11 -framework GLUT -framework OpenGL -Wno-deprecated *.cpp -o main

#include "main.h"
#include "Game.h"
#include <cstdlib>

extern struct Board board;
Game game;

void initializeOpenGL(int argc, char **argv)
{
    glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);

    glEnable(GL_MULTISAMPLE);
    glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);

    GLint iMultiSample = 0;
    GLint iNumSamples = 0;
    glGetIntegerv(GL_SAMPLE_BUFFERS, &iMultiSample);
    glGetIntegerv(GL_SAMPLES, &iNumSamples);

	glutInitWindowPosition(100,100);
	glutInitWindowSize(700, 700);
	glutCreateWindow("Game of life");
}

void processMenuEvents(int option)
{
    switch (option) {
		case 1:
            std::exit(0);
            break;
        case 2:
            glutFullScreen();
            break;
        case 3:
            glutReshapeWindow(700, 700);
            glutPositionWindow(100, 100);
            break;
    }
}

void initializeMenu()
{
    int menu = glutCreateMenu(processMenuEvents);
    glutAddMenuEntry("Close", 1);
    glutAddMenuEntry("Full Screen", 2);
    glutAddMenuEntry("Window", 3);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void timerFunc(int data)
{
    game.doStep(data);
    glutTimerFunc(1, timerFunc, 1);
    glutPostRedisplay();
}

void renderScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(0.0, 0.0, 0.0);
    glPointSize(1.0);

    glColor3f(1.0, 0.0, 0.0);
    glPointSize(3.0);

    board.print();

    glutSwapBuffers();
    glFlush();
}

int main(int argc, char **argv)
{
    board.fillFieldsCenters(100000);
    board.neighbours(3);

    initializeOpenGL(argc, argv);
    initializeMenu();

    glClearColor(1.0, 1.0, 1.0, 0.0);
    glShadeModel(GL_FLAT);

    glutDisplayFunc(renderScene);
    timerFunc(-1);
    glutMainLoop();
    return 0;
}
