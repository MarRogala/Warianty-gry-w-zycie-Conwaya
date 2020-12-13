#include "Parsing.h"
#include <fstream>
#include <stdio.h>

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

std::unique_ptr<parse_tree::node> language::parseINITprogram()
{
    std::fstream file;
    file.open("INIT.txt", std::ios::in);

    if(file.good() != true )
    {
        throw "Wrong INIT program file";
    }

    std::string program;

    while(!file.eof())
    {
        std::string line;
        getline(file, line);
        for(auto c: line)
        {
            if(!isspace(c))
                program.push_back(c);
        }
    }
    string_input inString(program, "INIT program");
    try {
        auto root = parse_tree::parse< grammar, parse_tree::node, selector >( inString );
        parse_tree::print_dot( std::cout, *root );
        return root;
    }
    catch( const parse_error& e ) {
        printError(e, program, "INIT");
        exit(0);
    }
    catch( const std::exception& e ) {
        std::cerr << e.what() << std::endl;
        exit(0);
    }
}

std::unique_ptr<parse_tree::node> language::parseTRANSITIONprogram()
{
    std::fstream file;
    file.open("TRANSITION.txt", std::ios::in);

    if(file.good() != true )
    {
        throw "Wrong TRANSITION program file";
    }

    std::string program;

    while(!file.eof())
    {
        std::string line;
        getline(file, line);
        for(auto c: line)
        {
            if(!isspace(c))
                program.push_back(c);
        }
    }
    string_input inString(program, "TRANSITION program");
    try {
        auto root = parse_tree::parse< grammar, parse_tree::node, selector >( inString );
        //parse_tree::print_dot( std::cout, *root );
        return root;
    }
    catch( const parse_error& e ) {
        printError(e, program, "TRANSITION");
        exit(0);
    }
    catch( const std::exception& e ) {
        std::cerr << e.what() << std::endl;
        exit(0);
    }
}


std::unique_ptr<parse_tree::node> language::parseCOLORprogram()
{
    std::fstream file;
    file.open("COLOR.txt", std::ios::in);

    if(file.good() != true )
    {
        throw "Wrong COLOR program file";
    }

    std::string program;

    while(!file.eof())
    {
        std::string line;
        getline(file, line);
        for(auto c: line)
        {
            if(!isspace(c))
                program.push_back(c);
        }
    }
    string_input inString(program, "COLOR program");
    try {
        auto root = parse_tree::parse< grammar, parse_tree::node, selector >( inString );
        //parse_tree::print_dot( std::cout, *root );
        return root;
    }
    catch( const parse_error& e ) {
        printError(e, program, "COLOR");
        exit(0);
    }
    catch( const std::exception& e ) {
        std::cerr << e.what() << std::endl;
        exit(0);
    }
}

std::vector<std::vector<float>> inputFile::parseInitData(std::string path)
{
    std::fstream file;
    file.open(path, std::ios::in);

    if(file.good() != true )
    {
        throw "Wrong file with initial data";
    }

    std::vector<std::vector<float>> result;
    while(!file.eof())
    {
        std::string line;
        getline(file, line);
        std::string cleanLine;
        for(auto c: line)
        {
            if(!isspace(c))
                cleanLine.push_back(c);
        }
        if(cleanLine.empty())
            break;
        string_input inLine(cleanLine, "file line");
        auto parsedLine = parseOneLine(inLine);
        result.push_back(parsedLine);
    }

    file.close();
    return result;
}
