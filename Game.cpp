#include "Game.h"

Board board;

void Game::gameSetup(int n)
{
}

void Game::doStep(int data)
{
    if(data > 0)
    {
        std::vector<int> tmp;
        for(auto x: candidates)
        {
            for(auto u: board.fields[x].neighbours)
            {
                if(board.fields[u].state == 0)
                {
                    board.fields[u].state = 1;
                    tmp.push_back(u);
                }
            }
        }
        candidates.clear();
        for(auto x: tmp)
            candidates.push_back(x);
        tmp.clear();
    }
    else
    {
        for(unsigned int i = 0; i < board.fields.size(); i ++)
        {
            if(board.fields[i].state == 1)
                candidates.push_back(i);
        }
    }

}
