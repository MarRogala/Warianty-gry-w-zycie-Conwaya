#include "Parsing.h"
#include "Evaluation.h"

#include <unordered_set>
#include <unordered_map>
#include <fstream>

typedef parse_tree::node nodeType;
typedef std::unique_ptr<nodeType> ptrType;

struct Game {

    Game() : mt(std::random_device()()) { }

    Board board;

    ptrType INITtree;
    ptrType COLORtree;
    ptrType TRANSITIONtree;

    std::string INITstring, COLORstring, TRANSITIONstring;

    std::vector<std::vector<float>> fileData;

    std::mt19937 mt;
    float rndValue(float l, float r);

    FieldColor initColor;
    int currentFieldId, stepsLimit = -1, stepsCounter = 0, printEvery = 1;
    std::unordered_map<std::string, float> variables;

    std::vector<std::vector<float>> newStates;
    std::vector<float> state, newState, color;
    std::vector<bool> toBeSkipped;

    void loadData(std::string fileName);
    void gameSetup(std::string fileName);
    void doStep();
    void doParsing();
    std::string nodeContent(const parse_tree::node& n);

    float getValue(const parse_tree::node& n);
    void setEnv(int field);
    void evaluateINITProgram();
    void evaluateCOLORProgram(int field);
    void evaluateTRANSITIONProgram(int field);

    Instruction* transcriptProgramTree(const parse_tree::node& n, bool root = false);
    Expression* transcriptExpressionTree(const parse_tree::node& n);
    Expression* transcriptFunction(const parse_tree::node& n);
    Expression* transcriptBinaryExpression(const parse_tree::node& n);
    Instruction* INITprogram;
    Instruction* COLORprogram;
    Instruction* TRANSITIONprogram;
};
