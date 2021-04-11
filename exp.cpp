#include "exp.h"
#include <iostream>
#include <cmath>
Expression::Expression() {

}
Expression::~Expression() {

}



ConstantExp::ConstantExp(int value)
{
    this->value = value;
}
int ConstantExp::eval(EvalState& state)
{
    return value;
}
string ConstantExp::toString() {
    return to_string(value);
}
ExpressionType ConstantExp::getType() {
    return CONSTANT;
}

string ConstantExp::gettoken()
{
    return to_string(value);
}

bool ConstantExp::isop()
{
    return false;
}

int ConstantExp::getValue() {
    return value;
}


IdentifierExp::IdentifierExp(string name) {
    this->name = name;
}

int IdentifierExp::eval(EvalState& state) {
    if (!state.isExist(name)) {
        string err="Error: "+name+" is undefined";
        throw myException(err);
        return 0;              //未定义的对象
    }
    return state.getValue(name);
}

string IdentifierExp::toString() {
    return name;
}

ExpressionType IdentifierExp::getType() {
    return IDENTIFIER;
}

string IdentifierExp::gettoken()
{
    return name;
}

bool IdentifierExp::isop()
{
    return false;
}

string IdentifierExp::getname() {
    return name;
}



CompoundExp::CompoundExp(string op, Expression* lhs, Expression* rhs) {
    this->op = op;
    this->lhs = lhs;
    this->rhs = rhs;
}

CompoundExp::~CompoundExp() {
    delete lhs;
    delete rhs;
}
int CompoundExp::eval(EvalState& state) {
    string err;
    if (op == "=") {
        if (lhs->getType() != IDENTIFIER) {
            //错误处理
            err="Error:Illegal variable in assignment";
            throw myException(err);
            return 0;
        }
        int val = rhs->eval(state);

        state.setValue(((IdentifierExp*)lhs)->getname(), val);
        return val;
    }
    int left = lhs->eval(state);
    int right = rhs->eval(state);
    if (op == "+") return left + right;
    if (op == "-") return left - right;
    if (op == "*") return left * right;
    if (op == "/") {
        if (right == 0) {
            //除零错误
            err="Error:division by zero";
            throw myException(err);
            return 0;
        }
        return left / right;
    }
    if (op == ">") {
        return left > right ? 1 : 0;

    }
    if (op == "<") {
        return left < right ? 1 : 0;
    }
    if (op == "THEN") {
        if (left !=0) return right;
        else return 0;
    }
    if(op=="**"){
        int num=pow(left,right);
        if(num==-2147483648) {
            err="Error: Numeric overflow";
            throw myException(err);
        }
        else
        return pow(left,right);
    }
    else{
        //非法运算符
        err="Error:Illegal operator"+op;
        throw myException(err);
    }
    return 0;
}
string CompoundExp::toString() {
    return '(' + lhs->toString() + ' ' + op + ' ' + rhs->toString() + ')';
}

string CompoundExp::gettoken()
{
    return op;
}
ExpressionType CompoundExp::getType() {
    return COMPOUND;
}

bool CompoundExp::isop()
{
    return true;
}
string CompoundExp::getop()
{
    return op;
}
Expression* CompoundExp::getlhs() {
    return lhs;
}
Expression* CompoundExp::getrhs() {
    return rhs;
}
void CompoundExp::setchild(Expression* lhs, Expression* rhs) {
    this->lhs = lhs;
    this->rhs = rhs;
}
