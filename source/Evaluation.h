#include <unordered_map>
#include <string>
#include <vector>
#include <random>
#include "Board.h"

class Expression {

public:
    virtual float calculate() = 0;
    virtual ~Expression() { }
};

class Number : public Expression {
    float number;
public:
    Number(float num) : number(num) { }
    float calculate()
    {
        return number;
    }
    ~Number() { }
};

class Function : public Expression {
public:
    virtual float calculate() = 0;
    ~Function() { }
};

class PrintEvery : public Function {
    int &printEvery;
    int value;

public:
    PrintEvery(int &p, int val) : printEvery(p), value(val) { }
    float calculate()
    {
        printEvery = value;
        return 0;
    }
    ~PrintEvery() { }
};

class InitialColor : public Function {
    float r, g, b;
    FieldColor &color;
public:
    InitialColor(float r, float g, float b, FieldColor& c) : r(r), g(g), b(b), color(c) { }
    float calculate()
    {
        color.r = r;
        color.g = g;
        color.b = b;
        return 0;
    }
    ~InitialColor() { }
};

class StepsLimit : public Function {
    float value;
    int &stepsLimit;

public:
    StepsLimit(float val, int& s) : value(val), stepsLimit(s) { }
    float calculate()
    {
        stepsLimit = value;
        return 0;
    }
    ~StepsLimit() { }
};

class Random : public Function {
    float left, right;
    std::mt19937& mt;
public:
    Random(float l, float r, std::mt19937& m) : left(l), right(r), mt(m) { }
    float calculate()
    {
        std::uniform_real_distribution<float> dist(0.0, 1.0);
        float val = left + (abs(left - right) * dist(mt));
        return val;
    }
    ~Random() { }
};

class SkipState : public Function {
    int index;
    float value;
    int &currentFieldId;
    std::vector<bool> &toBeSkipped;
    std::vector<float> &state;
public:
    SkipState(int ind, float val, int& curr, std::vector<bool> &t, std::vector<float> &s)
        : index(ind), value(val), toBeSkipped(t), state(s), currentFieldId(curr) { }
    float calculate()
    {
        if(state[index] == value)
        {
            toBeSkipped[currentFieldId] = true;
            return -1.0;
        }
        else return 1.0;
    }
    ~SkipState() { }
};

class Count : public Function {
    int index;
    float value;
    int &currentFieldId;
    Board &board;
public:
    Count(Board& b, int ind, float val, int& curr) : board(b), index(ind), value(val), currentFieldId(curr) { }
    float calculate()
    {
        int result = 0;
        for(auto neig: board.fields[currentFieldId].neighbours)
        {
            if(board.fields[neig].state[index] == value)
                result ++;
        }
        return result;
    }
    ~Count() { }
};

class Variable : public Expression {
    std::unordered_map<std::string, float>& variables;
    std::string name;

public:
    Variable(std::unordered_map<std::string, float>& vars, std::string n) : name(n), variables(vars) { }
    float calculate()
    {
        if(variables.find(name) != variables.end())
            return variables[name];
        else return 0.0;
    }
    ~Variable() { }
};

class ArrayVariable : public Expression {
    std::vector<float>& color;
    std::vector<float>& state;
    std::vector<float>& newState;
    int index;
    std::string name;

public:
        ArrayVariable(std::vector<float> &col, std::vector<float> &nSt, std::vector<float> &st, std::string n, int ind)
            : color(col), state(st), newState(nSt), index(ind), name(n) { }
        float calculate()
        {
            switch (name[0]) {
                case 'c':
                    return color[index];
                case 'n':
                    return newState[index];
                case 's':
                    return state[index];
                default:
                    return 0;
            }
        }
        ~ArrayVariable() { }
};

class BinaryExpression;

class BinaryExpression : public Expression {
protected:
    Expression* left;
    Expression* right;
public:
    BinaryExpression(Expression* l, Expression* r) : left(l), right(r) { }
    virtual float calculate() = 0;
    virtual ~BinaryExpression()
    {
        delete left;
        delete right;
    }
};

class Add : public BinaryExpression {
public:
    Add(Expression* l, Expression* r) : BinaryExpression(l, r) { }
    float calculate()
    {
        return left->calculate() + right->calculate();
    }
    ~Add() { }
};

class Sub : public BinaryExpression {
public:
    Sub(Expression* l, Expression* r) : BinaryExpression(l, r) { }
    float calculate()
    {
        return left->calculate() - right->calculate();
    }
    ~Sub() { }
};

class Mul : public BinaryExpression {
public:
    Mul(Expression* l, Expression* r) : BinaryExpression(l, r) { }
    float calculate()
    {
        return left->calculate() * right->calculate();
    }
    ~Mul() { }
};

class Div : public BinaryExpression {
public:
    Div(Expression* l, Expression* r) : BinaryExpression(l, r) { }
    float calculate()
    {
        return left->calculate() / right->calculate();
    }
    ~Div() { }
};

class Mod : public BinaryExpression {
public:
    Mod(Expression* l, Expression* r) : BinaryExpression(l, r) { }
    float calculate()
    {
        return static_cast<int>(left->calculate()) % static_cast<int>(right->calculate());
    }
    ~Mod() { }
};

class LessEqual : public BinaryExpression {
public:
    LessEqual(Expression* l, Expression* r) : BinaryExpression(l, r) { }
    float calculate()
    {
        return left->calculate() <= right->calculate() ? 1 : 0;
    }
    ~LessEqual() { }
};

class MoreEqual : public BinaryExpression {
public:
    MoreEqual(Expression* l, Expression* r) : BinaryExpression(l, r) { }
    float calculate()
    {
        return left->calculate() >= right->calculate() ? 1 : 0;
    }
    ~MoreEqual() { }
};

class More : public BinaryExpression {
public:
    More(Expression* l, Expression* r) : BinaryExpression(l, r) { }
    float calculate()
    {
        return left->calculate() > right->calculate() ? 1 : 0;
    }
    ~More() { }
};

class Less : public BinaryExpression {
public:
    Less(Expression* l, Expression* r) : BinaryExpression(l, r) { }
    float calculate()
    {
        return left->calculate() < right->calculate() ? 1 : 0;
    }
    ~Less() { }
};

class Equal : public BinaryExpression {
public:
    Equal(Expression* l, Expression* r) : BinaryExpression(l, r) { }
    float calculate()
    {
        return left->calculate() == right->calculate() ? 1 : 0;
    }
    ~Equal() { }
};

class And : public BinaryExpression {
public:
    And(Expression* l, Expression* r) : BinaryExpression(l, r) { }
    float calculate()
    {
        return left->calculate() != 0.0 && right->calculate() != 0.0 ? 1 : 0;
    }
    ~And() { }
};

class Or : public BinaryExpression {
public:
    Or(Expression* l, Expression* r) : BinaryExpression(l, r) { }
    float calculate()
    {
        return left->calculate() != 0.0 || right->calculate() != 0.0 ? 1 : 0;
    }
    ~Or() { }
};

class Instruction {
public:
    virtual void evaluate() = 0;
    virtual ~Instruction() { }
};

class FunctionCall : public Instruction {
    Expression* func;
public:
    FunctionCall(Expression* f) : func(f) { }
    void evaluate()
    {
        float v = func->calculate();
    }
    ~FunctionCall()
    {
        delete func;
    }
};

class Destination {
public:
    virtual void assign(float value) = 0;
    virtual ~Destination() { }
};

class VariableDestination : public Destination {
    std::unordered_map<std::string, float>& variables;
    std::string name;
public:
    VariableDestination(std::string name, std::unordered_map<std::string, float>& var)
        : name(name), variables(var) { }
    void assign(float value)
    {
        variables[name] = value;
    }
    ~VariableDestination() { }
};

class ArrayDestination : public Destination
{
    std::vector<float>& color;
    std::vector<float>& state;
    std::vector<float>& newState;
    std::string name;
    int index;
public:
    ArrayDestination(std::string name, int index, std::vector<float>& c, std::vector<float>& s, std::vector<float>& nS)
        : name(name), index(index), color(c), state(s), newState(nS) { }
    void assign(float value)
    {
        switch (name[0]) {
            case 'c':
                color[index] = value;
                break;
            case 's':
                state[index] = value;
                break;
            case 'n':
                newState[index] = value;
                break;
        }
    }
    ~ArrayDestination() { }
};

class Assignment : public Instruction {
    Destination *destination;
    Expression* expression;
public:
    Assignment(Destination *dest, Expression* exp) : destination(dest), expression(exp) { }
    void evaluate()
    {
        destination->assign(expression->calculate());
    }
    ~Assignment()
    {
        delete destination;
        delete expression;
    }
};

class Block : public Instruction {
    std::vector<Instruction*> block;
public:
    Block(std::vector<Instruction*> b) : block(b) { }
    void evaluate()
    {
        for(auto ins: block)
            ins->evaluate();
    }
    ~Block() { }
};

class If : public Instruction {
    Expression* exp;
    Block block;
public:
    If(Expression* e, Block b) : exp(e), block(b) { }
    void evaluate()
    {
        if(exp->calculate() != 0.0)
        {
            block.evaluate();
        }
    }
    ~If()
    {
        delete exp;
    }
};
