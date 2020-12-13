//g++ -g -w -Wall -std=c++11 -framework GLUT -framework OpenGL -Wno-deprecated *.cpp -o main

#include "main.h"
#include "Game.h"
#include <cstdlib>
#include "Parsing.h"

void testSilmpleParsing()
{
    string_input in1("aaa[bbb]=10+ccc;", "content");
    try {
        const auto root = parse_tree::parse< language::grammar, parse_tree::node, language::selector >( in1 );
        parse_tree::print_dot( std::cout, *root );
        return;
    }
    catch( const parse_error& e ) {
        const auto p = e.positions().front();
        std::cerr << e.what() << std::endl
                << in1.line_at( p ) << std::endl
                << std::setw( p.column ) << '^' << std::endl;
    }
    catch( const std::exception& e ) {
        std::cerr << e.what() << std::endl;
    }
}

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

    glClearColor(0.0, 0.0, 0.0, 0.0);

    board.fillFieldsCenters(100);
    board.neighbours(3);

    testSilmpleParsing();

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
