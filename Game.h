#include "Board.h"

#include <unordered_set>

struct Game {

    std::vector<int> candidates;
    FieldColor red = {1, 0, 0};
    FieldColor white = {1, 1, 1};
    void gameSetup();
    void doStep();
};
