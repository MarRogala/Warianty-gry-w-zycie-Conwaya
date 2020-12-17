#include "Board.h"
#include "Parsing.h"

#include <unordered_set>
#include <unordered_map>

typedef parse_tree::node nodeType;
typedef std::unique_ptr<nodeType> ptrType;

struct Game {

    FieldColor red = {1, 0, 0};
    FieldColor white = {1, 1, 1};

    ptrType INITprogram;
    ptrType COLORprogram;
    ptrType TRANSITIONprogram;

    std::string INITstring, COLORstring, TRANSITIONstring;

    std::vector<std::vector<float>> fileData;

    std::unordered_map<std::string, float> variables;
    std::vector<float> color, newColor;
    std::vector<float> state, newState;

    void loadData();
    void gameSetup();
    void doStep();

    void clearEnv();
    void setEnv(int field);
    std::string nodeContent(const parse_tree::node& n);

    float functionValue(const parse_tree::node& n);
    float operatorValue(const parse_tree::node& n);
    float getValue(const parse_tree::node& n);
    void evalProgram(const parse_tree::node& n);

    void evaluateINITProgram();
    void evaluateCOLORProgram(int field);
    void evaluateTRANSITIONProgram(int field);
};
