//g++ -g -w -Wall -std=c++11 -framework GLUT -framework OpenGL -Wno-deprecated *.cpp -o main

#include "main.h"
#include "Game.h"
#include <cstdlib>

extern struct Board board;
Game game;

void initializeMultisample(int argc, char **argv)
{
	//glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);
    //glEnable(GL_MULTISAMPLE);
    //glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
    /*GLint iMultiSample = 0;
    GLint iNumSamples = 0;
    glGetIntegerv(GL_SAMPLE_BUFFERS, &iMultiSample);
    glGetIntegerv(GL_SAMPLES, &iNumSamples);*/
	//glutInitWindowPosition(100,100);
	//glutInitWindowSize(700, 700);
	//glutCreateWindow("Game of life");
}

void renderScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    board.print();
    glFlush();
}

int main(int argc, char **argv)
{
    GLFWwindow* window;

    if (!glfwInit())
        return -1;


    glfwWindowHint(GLFW_SAMPLES, 4);
    glEnable(GL_MULTISAMPLE);
    window = glfwCreateWindow(600, 600, "Game of life", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if(glewInit() != GLEW_OK)
        std::cout << "GLEW INIT ERROR\n";

    board.fillFieldsCenters(100000);
    board.neighbours(3);

    game.gameSetup();

    while (!glfwWindowShouldClose(window))
    {
        renderScene();
        game.doStep();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
