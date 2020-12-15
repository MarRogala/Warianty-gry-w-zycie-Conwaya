#include "Board.h"
#include "Parsing.h"

#include <unordered_set>
#include <memory>

typedef tao::pegtl::parse_tree::node nodeType;
typedef std::unique_ptr<nodeType> nodePtr;

struct Game {

    std::vector<int> candidates;
    FieldColor red = {1, 0, 0};
    FieldColor white = {1, 1, 1};

    nodePtr TRANSITIONprogram;

    std::vector<std::vector<float>> fileData;

    void loadData();
    void gameSetup();
    void doStep();

    void evaluateINITfunction(nodePtr);
    void evaluateINITprogram();
};
