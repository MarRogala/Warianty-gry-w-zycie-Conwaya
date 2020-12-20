#define GLFW_INCLUDE_GLU
#define GLFW_DLL
#include "GL/glew.h"
#include <GLFW/glfw3.h>
#include <GL/glut.h>
#include <GL/gl.h>

#include "Game.h"
#include <cstdlib>

extern struct Board board;
Game game;

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

    glClearColor(0.0, 0.0, 0.0, 0.0);

    if(argc == 1)
    {
        game.gameSetup("input.csv");
    }
    if(argc == 2)
    {
        game.gameSetup(argv[1]);
    }

    while (!glfwWindowShouldClose(window))
    {
        if(game.stepsCounter % game.printEvery == 0)
            renderScene();

        game.doStep();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
