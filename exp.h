#ifndef EXP_H
#define EXP_H
#include "evalstate.h"
#include "error.h"
enum ExpressionType { CONSTANT, IDENTIFIER, COMPOUND };
class Expression {
public:
    Expression();
    virtual ~Expression();
    virtual int eval(EvalState& state) = 0;
    virtual string toString() = 0;
    virtual string gettoken()=0;
    virtual ExpressionType getType() = 0;
    virtual bool isop()=0;

};

class ConstantExp : public Expression {
public:
    ConstantExp(int value);
    virtual int eval(EvalState& state);
    virtual string toString();
    virtual ExpressionType getType();
    virtual string gettoken();
    virtual bool isop();
    int getValue();
private:
    int value;
};


class IdentifierExp : public Expression {
public:
    IdentifierExp(string name);
    virtual int eval(EvalState& state);
    virtual string toString();
    virtual ExpressionType getType();
    virtual string gettoken();
    virtual bool isop();
    string getname();
private:
    string name;
};

class CompoundExp : public Expression {
public:
    CompoundExp(string op, Expression* lhs=nullptr, Expression* rhs=nullptr);

    virtual ~CompoundExp();
    virtual int eval(EvalState& state);
    virtual string toString();
    virtual string gettoken();
    virtual ExpressionType getType();
    virtual bool isop();
    string getop();
    Expression* getlhs();
    Expression* getrhs();
    void setchild(Expression* lhs,Expression* rhs);
    //为了方便，放到public上
    Expression* lhs, * rhs;
private:
    string op;

};

#endif // EXP_H

