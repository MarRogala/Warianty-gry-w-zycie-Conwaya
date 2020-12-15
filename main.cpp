//g++ -g -w -Wall -std=c++11 -framework GLUT -framework OpenGL -Wno-deprecated *.cpp -o main

#include "main.h"
#include "Game.h"
#include <cstdlib>
//#include "Parsing.h"

void testSilmpleParsing()
{
    language::parseINITprogram();
    //string_input in1("aaa[bbb]=10.0+ccc(1,2.0);", "content");
    /*string_input in1("if(name){aaa=10.0;tab[0]=10.0+ccc(1,2.0);}fun(13.13);a=1;", "content");
    try {
        const auto root = parse_tree::parse< language::grammar, parse_tree::node, language::selector >( in1 );
        parse_tree::print_dot( std::cout, *root );

        auto r = inputFile::parseInitData("input.txt");
        for(auto x: r)
        {
            for(auto y: x)
                std::cout << y << " ";
            std::cout << "\n";
        }
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
    }*/
}

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
