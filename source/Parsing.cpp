#include "Parsing.h"
#include <fstream>
#include <stdio.h>

std::string language::nodeContent(const parse_tree::node& n, std::string program)
{
    auto begin = n.begin().column - 1;
    auto end = n.end().column - 1;
    std::string s;
    for(int i = begin; i < end; i ++)
    {
        auto c = program[i];
        s.push_back(c);
    }
    return s;
}

void language::printError(parse_error e, std::string program, std::string name)
{
    const auto p = e.positions().front();
    std::cerr << "ERROR -> " << name << " at instruction: " << std::endl;
    std::string wrongLine;
    int col = p.column;
    int size = 0;
    col = fmax(col - 1, 0);

    bool openBracket = false;
    while(program[col + size] != ';' && program[col + size] != '}')
    {
        size ++;
        if(program[col + size] == '{')
            openBracket = true;
    }

    if(openBracket)
    {
        while(program[col + size] != '}')
            size ++;
    }

    wrongLine = program.substr(col, size + 1);
    std::cerr << wrongLine << std::endl;
}

std::string language::readFile(std::string path)
{
    std::fstream file;
    file.open(path, std::ios::in);

    if(file.good() != true )
    {
        throw "Wrong" + path + "program file";
    }

    std::string program;
    bool comment = false;

    while(!file.eof())
    {
        std::string line;
        getline(file, line);
        for(auto c: line)
        {
            if(isspace(c))
                continue;
            if(c == '#')
            {
                comment ^= 1;
                continue;
            }
            if(!comment)
                program.push_back(c);
        }
    }
    return program;
}

std::unique_ptr<parse_tree::node> language::parseProgram(std::string program, std::string name)
{

    string_input inString(program, name);
    try {
        auto root = parse_tree::parse< grammar, parse_tree::node, selector >( inString );
        //parse_tree::print_dot( std::cout, *root );
        return root;
    }
    catch( const parse_error& e ) {
        printError(e, program, name);
        exit(0);
    }
    catch( const std::exception& e ) {
        std::cerr << e.what() << std::endl;
        exit(0);
    }
}
