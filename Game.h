#include "Board.h"
#include "Parsing.h"

#include <unordered_set>
#include <unordered_map>
#include <fstream>

typedef parse_tree::node nodeType;
typedef std::unique_ptr<nodeType> ptrType;

struct Game {

    ptrType INITprogram;
    ptrType COLORprogram;
    ptrType TRANSITIONprogram;

    std::string INITstring, COLORstring, TRANSITIONstring;

    std::vector<std::vector<float>> fileData;

    int currentFieldId;
    std::unordered_map<std::string, float> variables;
    std::vector<float> color;
    std::vector<float> state, newState;
    std::vector<bool> toBeSkipped;
    void loadData(std::string fileName);
    void gameSetup(std::string fileName);
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
