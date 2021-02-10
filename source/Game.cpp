#include "Game.h"

void Game::setEnv(int field)
{
    variables.clear();
    currentFieldId = field;
    color.resize(3, 0);
    color[0] = board.fields[field].color.r;
    color[1] = board.fields[field].color.g;
    color[2] = board.fields[field].color.b;
    newState.resize(board.fields[field].state.size());
    for(unsigned int i = 0; i < board.fields[field].state.size(); i ++)
    {
        state[i] = board.fields[field].state[i];
        newState[i] = board.fields[field].state[i];
    }
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

float Game::getValue(const parse_tree::node& n)
{
    if(n.type == "language::integer" || n.type == "language::floatingPoint")
    {
        return std::stof(nodeContent(n));
    }
}

Expression* Game::transcriptFunction(const parse_tree::node& n)
{
    auto& firstArg = n.children[0];
    std::string functionName = nodeContent(*firstArg);

    if(functionName == "skipState")
    {
        // index, value
        int index = getValue(*(n.children[1]));
        int value = getValue(*(n.children[2]));
        if(index >= state.size())
        {
            std::cerr << "state index out of bound\n";
            exit(1);
        }
        return new SkipState(index, value, currentFieldId, toBeSkipped, state);
    }
    else if(functionName == "count")
    {
        // id, index, value
        int index = getValue(*(n.children[1]));
        int value = getValue(*(n.children[2]));
        if(index >= state.size())
        {
            std::cerr << "state index out of bound\n";
            exit(1);
        }
        return new Count(board, index, value, currentFieldId);
    }
    else if(functionName == "random")
    {
        // random number [l, r]
        int left = getValue(*(n.children[1]));
        int right = getValue(*(n.children[2]));
        return new Random(left, right, mt);
    }
    else if(functionName == "printEvery")
    {
        int value = getValue(*(n.children[1]));
        if(printEvery == 0)
        {
            std::cerr << "printEvery can't be equal to 0\n";
            exit(1);
        }
        return new PrintEvery(printEvery, value);
    }
    else if(functionName == "initialColor")
    {
        float r = getValue(*(n.children[1]));
        float g = getValue(*(n.children[2]));
        float b = getValue(*(n.children[3]));
        return new InitialColor(r, g, b, initColor);
    }
    else if(functionName == "stepsLimit")
    {
        float value = getValue(*(n.children[1]));
        return new StepsLimit(value, stepsLimit);
    }
}

Expression* Game::transcriptBinaryExpression(const parse_tree::node& n)
{
    auto& firstArg = n.children[0];
    auto& secondArg = n.children[1];
    auto& thirdArg = n.children[2];

    Expression* leftExp = transcriptExpressionTree(*firstArg);
    Expression* rightExp = transcriptExpressionTree(*thirdArg);

    std::string op = nodeContent(*secondArg);

    if(op.size() == 1)
    {
        switch (op[0]) {
            case '+':
                return new Add(leftExp, rightExp);
            case '-':
                return new Sub(leftExp, rightExp);
            case '*':
                return new Mul(leftExp, rightExp);
            case '/':
                return new Div(leftExp, rightExp);
            case '%':
                return new Mod(leftExp, rightExp);
            case '<':
                return new Less(leftExp, rightExp);
            case '>':
                return new More(leftExp, rightExp);
        }
    }
    else
    {
        if(op == "<=")
            return new LessEqual(leftExp, rightExp);
        else if(op == ">=")
            return new MoreEqual(leftExp, rightExp);
        else if(op == "==")
            return new Equal(leftExp, rightExp);
        else if(op == "&&")
            return new And(leftExp, rightExp);
        else if(op == "||")
            return new Or(leftExp, rightExp);
    }
}

Expression* Game::transcriptExpressionTree(const parse_tree::node& n)
{
    if(n.type == "language::variable")
    {
        std::string name = nodeContent(n);
        return new Variable(variables, name);
    }
    else if(n.type == "language::integer" || n.type == "language::floatingPoint")
    {
        return new Number(std::stof(nodeContent(n)));
    }
    else if(n.type == "language::arrayVariable")
    {
        auto& nameNode = n.children[0];
        auto& indexNode = n.children[1];
        std::string name = nodeContent(*nameNode);
        int index = static_cast<int>(getValue(*indexNode));
        return new ArrayVariable(color, newState, state, name, index);
    }
    else if(n.type == "language::function")
    {
        return transcriptFunction(n);
    }
    else if(n.type == "language::binaryExpression")
    {
        return transcriptBinaryExpression(n);
    }
}

Instruction* Game::transcriptProgramTree(const parse_tree::node& n, bool root)
{
    if(n.type == "language::assignment")
    {
        Expression* expression = transcriptExpressionTree(*n.children[1]);
        auto& dest = n.children[0];
        Destination *destination;
        if(dest->type == "language::variable")
        {
            std::string name;
            name = nodeContent(*dest);
            destination = new VariableDestination(name, variables);
        }
        else if(dest->type == "language::arrayVariable")
        {
            std::string name;
            int index;
            name = nodeContent(*(dest->children[0]));
            index = getValue(*(dest->children[1]));
            destination = new ArrayDestination(name, index, color, state, newState);
        }
        return new Assignment(destination, expression);
    }
    else if(n.type == "language::function")
    {
        return new FunctionCall(transcriptExpressionTree(n));
    }
    else if(n.type == "language::ifStatement")
    {
        Expression* cond = transcriptExpressionTree(*(n.children[0]));
        std::vector<Instruction*> block;
        for(auto& chld: n.children)
        {
            if(chld->type != "language::assignment")
                continue;
            block.push_back(transcriptProgramTree(*chld, false));
        }
        return new If(cond, block);
    }
    else if(root)
    {
        std::vector<Instruction*> block;
        for(auto& ins: n.children)
            block.push_back(transcriptProgramTree(*ins, false));
        return new Block(block);
    }

}

void Game::evaluateINITProgram()
{
    INITprogram->evaluate();
}

void Game::evaluateCOLORProgram(int field)
{
    setEnv(field);
    COLORprogram->evaluate();
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
    TRANSITIONprogram->evaluate();
}

void Game::loadData(std::string fileName)
{
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

    color.resize(3, 0);
    state.resize(counter - 2);
    newState.resize(counter - 2);

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
        Field field = Field(i, {fileData[i][0], fileData[i][1]}, initColor);
        for(unsigned int j = 2; j < fileData[i].size(); j ++)
        {
            field.state.push_back(fileData[i][j]);
        }
        board.fields.push_back(field);
    }
    toBeSkipped.resize(board.fields.size(), false);
    std::cout << "File loading is done\n";
}

void Game::doParsing()
{
    INITstring = language::readFile("programs/INIT.c");
    COLORstring = language::readFile("programs/COLOR.c");
    TRANSITIONstring = language::readFile("programs/TRANSITION.c");

    INITtree = language::parseProgram(INITstring, "INIT");
    COLORtree = language::parseProgram(COLORstring, "COLOR");
    TRANSITIONtree = language::parseProgram(TRANSITIONstring, "TRANS");

    INITprogram = transcriptProgramTree(*INITtree, true);
    COLORprogram = transcriptProgramTree(*COLORtree, true);
    TRANSITIONprogram = transcriptProgramTree(*TRANSITIONtree, true);
}

void Game::gameSetup(std::string fileName)
{
    evaluateINITProgram();

    for(unsigned int i = 0; i < board.fields.size(); i ++)
    {
        evaluateCOLORProgram(i);
        board.changeFieldColor(board.fields[i].fieldId, board.fields[i].color);
    }
    std::cout << "Setup is done\n";
}

void Game::doStep()
{
    if(stepsLimit != -1 && stepsCounter > stepsLimit)
        return;
    stepsCounter ++;

    newStates.clear();
    toBeSkipped.resize(board.fields.size(), false);
    for(int i = 0; i < board.fields.size(); i ++)
    {
        evaluateTRANSITIONProgram(i);
        newStates.push_back(newState);
    }
    for(int i = 0; i < board.fields.size(); i ++)
    {
        if(toBeSkipped[i] && printEvery == 1)
            continue;
        board.fields[i].state = newStates[i];
        if((stepsCounter % printEvery) == (printEvery - 1))
        {
            evaluateCOLORProgram(i);
            board.changeFieldColor(board.fields[i].fieldId, board.fields[i].color);
        }
    }
}
