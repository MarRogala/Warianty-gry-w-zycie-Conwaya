#include "Parsing.h"
#include <fstream>
#include <stdio.h>

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
