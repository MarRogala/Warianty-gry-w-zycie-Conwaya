#include "Board.h"

#include <unordered_set>

struct Game {

    std::vector<int> candidates;
    void gameSetup(int n);
    void doStep(int data);
};