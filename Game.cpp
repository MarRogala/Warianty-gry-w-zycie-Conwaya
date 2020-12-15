#include "Game.h"

Board board;

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
    //INITprogram = language::parseINITprogram();

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
