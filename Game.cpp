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

float Game::functionValue(const parse_tree::node& n)
{
    auto& firstArg = n.children[0];;
    std::string functionName = nodeContent(*firstArg);

    if(functionName == "neighbours")
    {
        int index = getValue(*(n.children[1]));
        return board.fields[index].neighbours.size();
    }
    if(functionName == "count")
    {
        int field = getValue(*(n.children[1]));
        int index = getValue(*(n.children[2]));
        int value = getValue(*(n.children[3]));
        int result = 0;
        for(auto neig: board.fields[field].neighbours)
        {
            if(board.fields[neig].state[index] == value)
                result ++;
        }
        return result;
    }
    if(functionName == "random")
    {
        int left = getValue(*(n.children[1]));
        int right = getValue(*(n.children[2]));

        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_real_distribution<double> dist(left, right);
        return dist(mt);
    }
    return 1;
}

float Game::operatorValue(const parse_tree::node& n)
{
    auto& firstArg = n.children[0];
    auto& secondArg = n.children[1];
    auto& thirdArg = n.children[2];

    float leftVal = getValue(*firstArg);
    float rightVal = getValue(*thirdArg);

    std::string op = nodeContent(*secondArg);

    if(op == "language::plus")
        return leftVal + rightVal;
    if(op == "language::minus")
        return leftVal + rightVal;
    if(op == "language::multiply")
        return leftVal * rightVal;
    if(op == "language::divide")
        return leftVal / rightVal;
    if(op == "language::modulo")
        return static_cast<int>(leftVal) % static_cast<int>(rightVal);
    if(op == "language::less")
        return leftVal < rightVal ? 1 : 0;
    if(op == "language::more")
        return leftVal > rightVal ? 1 : 0;
    if(op == "language::lessEqual")
        return leftVal <= rightVal ? 1 : 0;
    if(op == "language::moreEqual")
        return leftVal >= rightVal ? 1 : 0;
    if(op == "language::equal")
        return leftVal == rightVal;
    if(op == "language::andOp")
        return (leftVal != 0 && rightVal != 0) ? 1 : 0;
    if(op == "language::orOp")
        return (leftVal != 0 || rightVal != 0) ? 1 : 0;
}

float Game::getValue(const parse_tree::node& n)
{
    if(n.type == "language::variable")
    {
        std::string name = nodeContent(n);
        if(variables.find(name) == variables.end())
            return 0.0;
        else return variables[name];
    }
    if(n.type == "language::integer" || n.type == "language::floatingPoint")
    {
        return std::stof(nodeContent(n));
    }
    if(n.type == "language::arrayVariable")
    {
        auto& nameNode = n.children[0];
        auto& indexNode = n.children[1];
        std::string name = nodeContent(*nameNode);
        int index = static_cast<int>(getValue(*indexNode));
        if(name == "color")
            return color[index];
        if(name == "state")
            return state[index];
    }
    if(n.type == "language::function")
    {
        return functionValue(n);
    }
    if(n.type == "language::binaryExpression")
    {
        return operatorValue(n);
    }
}

void Game::evalProgram(const parse_tree::node& n)
{
    if(n.is_root())
    {
        for(auto& ins: n.children)
            evalProgram(*ins);
        return;
    }
    if(n.type == "language::function")
    {
        int uselesVal = functionValue(n);
    }
    if(n.type == "language::assignment")
    {
        float value = getValue(*n.children[1]);
        auto& dest = n.children[0];
        if(dest->type == "language::variable")
        {
            std::string name;
            name = nodeContent(*dest);
            variables[name] = value;
        }
        if(dest->type == "language::arrayVariable")
        {
            std::string name;
            int index;
            name = nodeContent(*(dest->children[0]));
            index = getValue(*(dest->children[1]));
            if(name == "color")
                color[index] = value;
            if(name == "state")
                state[index] = value;
        }
        return;
    }
    if(n.type == "language::ifStatement")
    {
        float cond = getValue(*(n.children[0]));
        for(auto& chld: n.children)
        {
            if(chld->type == "language::variable")
                continue;
            evalProgram(*chld);
        }
    }
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
    evalProgram(*COLORprogram);

}

void Game::evaluateTRANSITIONProgram(int field)
{
    setEnv(field);
    evalProgram(*TRANSITIONprogram);
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
    //print_node(*INITprogram, INITstring);
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
