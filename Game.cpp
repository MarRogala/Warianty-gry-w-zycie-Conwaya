#include "Game.h"

Board board;

void Game::clearEnv()
{
    state.clear();
    newState.clear();
    color.clear();
    newColor.clear();
    variables.clear();
}

void Game::setEnv(int field)
{
    clearEnv();
    color[0] = board.fields[field].color.r;
    color[1] = board.fields[field].color.g;
    color[2] = board.fields[field].color.b;
    for(unsigned int i = 0; i < board.fields[field].state.size(); i ++)
        state.push_back(board.fields[field].state[i]);
}

std::string Game::nodeContent(const parse_tree::node& n)
{
    auto begin = n.begin().column - 1;
    auto end = n.end().column - 1;
    int size = end - begin;

    std::string s;
    if(n.source == "INIT")
    {
        s = INITstring.substr(begin, size);
    }
    else if(n.source == "COLOR")
    {
        s = COLORstring.substr(begin, size);
    }
    else
    {
        s = TRANSITIONstring.substr(begin, size);
    }
    return s;
}

void Game::evaluateINITProgram()
{
    if(INITprogram->is_root())
    {
        // nothing to do here
    }
}

void Game::evaluateCOLORProgram(int field)
{
    setEnv(field);


}

void Game::evaluateTRANSITIONProgram(int field)
{
    setEnv(field);
}

void Game::loadData()
{
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(-0.0001, 0.0001);

    if(fileData.empty())
    {
        std::cerr << "Input file is empty!";
        exit(0);
    }
    int lineSize = fileData[0].size();
    for(auto line: fileData)
    {
        if(line.size() != lineSize)
        {
            std::cerr << "Lines with wrong size in input file!";
            exit(0);
        }
    }

    for(unsigned int i = 0; i < fileData.size(); i ++)
    {
        Field field = Field(i, {fileData[i][0], fileData[i][1] + dist(mt)});
        for(unsigned int j = 2; j < fileData[i].size(); j ++)
        {
            field.state.push_back(fileData[i][j]);
        }
        board.fields.push_back(field);
    }
}

void Game::gameSetup()
{
    INITstring = language::readFile("programs/INIT.txt");
    COLORstring = language::readFile("programs/COLOR.txt");
    TRANSITIONstring = language::readFile("programs/TRANSITION.txt");

    INITprogram = language::parseProgram(INITstring, "INIT");
    COLORprogram = language::parseProgram(COLORstring, "COLOR");
    TRANSITIONprogram = language::parseProgram(TRANSITIONstring, "TRANS");

    //print_dot(std::cout, *INITprogram);
    //print_dot( std::cout, *INITprogram);
    print_node(*INITprogram, INITstring);
    evaluateINITProgram();

    fileData = inputFile::parseInitData("input.txt");

    loadData();
    board.generateVoronoi();

    for(unsigned int i = 0; i < board.fields.size(); i ++)
    {
        if(board.fields[i].state[0] == 1)
        {
            candidates.push_back(i);
            board.changeFieldColor(board.fields[i].fieldId, red);
        }
        else
        {
            board.changeFieldColor(board.fields[i].fieldId, white);
        }
    }
}

void Game::doStep()
{
    std::vector<int> tmp;
    for(auto x: candidates)
    {
        for(auto u: board.fields[x].neighbours)
        {
            if(board.fields[u].state[0] == 0)
            {
                board.fields[u].state[0] = 1;
                board.changeFieldColor(board.fields[u].fieldId, red);
                tmp.push_back(u);
            }
        }
    }
    candidates.clear();
    for(auto x: tmp)
        candidates.push_back(x);
    tmp.clear();
}
