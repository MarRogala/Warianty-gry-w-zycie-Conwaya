#include "Game.h"

Board board;

void Game::clearEnv()
{
    state.clear();
    newState.clear();
    color.clear();
    variables.clear();
}

void Game::setEnv(int field)
{
    clearEnv();
    currentFieldId = field;
    color.resize(3, 0);
    color[0] = board.fields[field].color.r;
    color[1] = board.fields[field].color.g;
    color[2] = board.fields[field].color.b;
    newState.resize(board.fields[field].state.size());
    for(unsigned int i = 0; i < board.fields[field].state.size(); i ++)
    {
        state.push_back(board.fields[field].state[i]);
    }
    newState = state;
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

float Game::rndValue(float l, float r)
{
    std::uniform_real_distribution<float> dist(0.0, 1.0);
    return l + (abs(l - r) * dist(mt));
}

float Game::functionValue(const parse_tree::node& n)
{
    auto& firstArg = n.children[0];;
    std::string functionName = nodeContent(*firstArg);

    if(functionName == "initialColor")
    {
        initColor.r = getValue(*(n.children[1]));
        initColor.g = getValue(*(n.children[2]));
        initColor.b = getValue(*(n.children[3]));
        return 1.0;
    }
    if(functionName == "stepsLimit")
    {
        float value = getValue(*(n.children[1]));
        stepsLimit = value;
    }
    if(functionName == "skipCondition")
    {
        float value = getValue(*(n.children[1]));
        if(value != 0.0)
        {
            toBeSkipped[currentFieldId] = true;
            return -1.0;
        }
        else return 1.0;
    }
    if(functionName == "skipState")
    {
        // index, value
        int index = getValue(*(n.children[1]));
        int value = getValue(*(n.children[2]));
        if(state[index] == value)
        {
            toBeSkipped[currentFieldId] = true;
            return -1.0;
        }
        else return 1.0;
    }
    if(functionName == "neighbours")
    {
        // id
        int index = getValue(*(n.children[1]));
        return board.fields[index].neighbours.size();
    }
    if(functionName == "count")
    {
        // id, index, value
        int index = getValue(*(n.children[1]));
        int value = getValue(*(n.children[2]));
        int result = 0;
        for(auto neig: board.fields[currentFieldId].neighbours)
        {
            if(board.fields[neig].state[index] == value)
                result ++;
        }
        return result;
    }
    if(functionName == "random")
    {
        // random number [l, r]
        int left = getValue(*(n.children[1]));
        int right = getValue(*(n.children[2]));

        return rndValue(left, right);
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

    if(op == "+")
        return leftVal + rightVal;
    if(op == "-")
        return leftVal + rightVal;
    if(op == "*")
        return leftVal * rightVal;
    if(op == "/")
        return leftVal / rightVal;
    if(op == "%")
        return static_cast<int>(leftVal) % static_cast<int>(rightVal);
    if(op == "<")
        return leftVal < rightVal ? 1 : 0;
    if(op == ">")
        return leftVal > rightVal ? 1 : 0;
    if(op == "<=")
        return leftVal <= rightVal ? 1 : 0;
    if(op == ">=")
        return leftVal >= rightVal ? 1 : 0;
    if(op == "==")
        return leftVal == rightVal;
    if(op == "&&")
        return (leftVal != 0 && rightVal != 0) ? 1 : 0;
    if(op == "||")
        return (leftVal != 0 || rightVal != 0) ? 1 : 0;
}

float Game::getValue(const parse_tree::node& n)
{
    if(n.type == "language::variable")
    {
        std::string name = nodeContent(n);
        if(variables.find(name) == variables.end())
            return 0.0;
        else
            return variables[name];
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
        int value = functionValue(n);
        if(value == -1.0)
            return;
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
            if(name == "newState")
                newState[index] = value;
        }
        return;
    }
    if(n.type == "language::ifStatement")
    {
        float cond = getValue(*(n.children[0]));
        if(cond != 0.0)
        {
            for(auto& chld: n.children)
            {
                if(chld->type == "language::variable")
                    continue;
                evalProgram(*chld);
            }
        }
    }
}

void Game::evaluateINITProgram()
{
    evalProgram(*INITprogram);
}

void Game::evaluateCOLORProgram(int field)
{
    setEnv(field);
    evalProgram(*COLORprogram);
    if(board.fields[field].color.r == color[0] && board.fields[field].color.g == color[1] && board.fields[field].color.b == color[2])
    {
        board.fields[field].changeColor = false;
        return;
    }
    board.fields[field].changeColor = true;
    board.fields[field].color.r = color[0];
    board.fields[field].color.g = color[1];
    board.fields[field].color.b = color[2];

}

void Game::evaluateTRANSITIONProgram(int field)
{
    setEnv(field);
    evalProgram(*TRANSITIONprogram);
}

void Game::loadData(std::string fileName)
{
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(-0.0001, 0.0001);

    std::ifstream inputFile("./input/" + fileName);
    if(!inputFile.is_open() || !inputFile.good())
    {
        std::cerr << "Input file is not open\n";
        exit(1);
    }

    std::string line, columnName;
    std::getline(inputFile, line);
    std::stringstream lineStream(line);

    std::cout << "Reading columns: ";
    int counter = 0;
    while(std::getline(lineStream, columnName, ','))
    {
        std::string trimedName;
        std::copy_if(
            columnName.begin(), columnName.end(),
            std::back_inserter(trimedName),
            [](char c){
                return !isspace(c);
            }
        );
        counter ++;
        std::cout << trimedName << ", ";
    }
    std::cout << "from file " << fileName << "\n";
    if(counter < 3)
    {
        std::cerr << "To short lines in input file\n";
        exit(1);
    }

    std::string coma = ",";
    while(std::getline(inputFile, line))
    {
        std::vector<float> lineData;
        std::string value;

        int pos = 0;
        while ((pos = line.find(coma)) != std::string::npos)
        {
            value = line.substr(0, pos);
            line.erase(0, pos + coma.length());
            lineData.push_back(std::stof(value));
        }
        lineData.push_back(std::stof(line));
        if(lineData.size() != counter)
        {
            std::cerr << "Wrong size of line in input file\n";
            exit(1);
        }
        fileData.push_back(lineData);
    }
    inputFile.close();

    if(fileData.empty())
    {
        std::cerr << "Input file is empty!";
        exit(1);
    }

    int lineSize = fileData[0].size();
    for(auto line: fileData)
    {
        if(line.size() != lineSize)
        {
            std::cerr << "Lines with wrong size in input file!";
            exit(1);
        }
    }
    for(unsigned int i = 0; i < fileData.size(); i ++)
    {
        Field field = Field(i, {fileData[i][0], fileData[i][1] + dist(mt)}, initColor);
        //std::cout << "field: " << field.fieldCenter.first << " " << field.fieldCenter.second << "\n";
        for(unsigned int j = 2; j < fileData[i].size(); j ++)
        {
            field.state.push_back(fileData[i][j]);
        }
        board.fields.push_back(field);
    }
    std::cout << "File loading is done\n";
}

void Game::gameSetup(std::string fileName)
{
    INITstring = language::readFile("programs/INIT.c");
    COLORstring = language::readFile("programs/COLOR.c");
    TRANSITIONstring = language::readFile("programs/TRANSITION.c");

    INITprogram = language::parseProgram(INITstring, "INIT");
    COLORprogram = language::parseProgram(COLORstring, "COLOR");
    TRANSITIONprogram = language::parseProgram(TRANSITIONstring, "TRANS");

    evaluateINITProgram();

    loadData(fileName);
    board.generateVoronoi();


    toBeSkipped.resize(board.fields.size(), false);
    for(unsigned int i = 0; i < board.fields.size(); i ++)
    {
        evaluateCOLORProgram(i);
        board.changeFieldColor(board.fields[i].fieldId, board.fields[i].color);
    }
    std::cout << "Setup is done\n";
}

void Game::doStep()
{
    //int x; std::cin >> x;
    if(stepsLimit != -1 && stepsCounter > stepsLimit)
        return;
    stepsCounter ++;

    std::vector<std::vector<float>> newStates;
    toBeSkipped.resize(board.fields.size(), false);
    for(int i = 0; i < board.fields.size(); i ++)
    {
        evaluateTRANSITIONProgram(i);
        newStates.push_back(newState);
    }
    for(int i = 0; i < board.fields.size(); i ++)
    {
        if(toBeSkipped[i])
            continue;
        board.fields[i].state = newStates[i];
        evaluateCOLORProgram(i);
        board.changeFieldColor(board.fields[i].fieldId, board.fields[i].color);
    }
}
