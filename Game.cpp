#include "Game.h"

Board board;

void Game::gameSetup()
{
    for(unsigned int i = 0; i < board.fields.size(); i ++)
    {
        if(board.fields[i].state == 1)
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
            if(board.fields[u].state == 0)
            {
                board.fields[u].state = 1;
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
