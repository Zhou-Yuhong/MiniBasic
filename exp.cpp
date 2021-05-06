#include "exp.h"
#include <iostream>
#include <cmath>
Expression::Expression() {

}
Expression::~Expression() {

}



ConstantExp::ConstantExp(int value)
{
    ValueUnit tmp(value);
    this->value=tmp;
}
ConstantExp::ConstantExp(string value){
    ValueUnit tmp(value);
    this->value=tmp;
}

ValueUnit ConstantExp::eval(EvalState& state)
{
    return value;
}
string ConstantExp::toString() {
   if(value.type){
    return to_string(value.getvalInt());
   }
   else{
       return value.getvalString();
   }
}
ExpressionType ConstantExp::getType() {
    return CONSTANT;
}

string ConstantExp::gettoken()
{
    if(value.type){
     return to_string(value.getvalInt());
    }
    else{
        return value.getvalString();
    }
}

bool ConstantExp::isop()
{
    return false;
}
int ConstantExp::getValueInt(){
    return this->value.getvalInt();
}
string ConstantExp::getValueString(){
    return this->value.getvalString();
}

IdentifierExp::IdentifierExp(string name) {
    this->name = name;
}

ValueUnit IdentifierExp::eval(EvalState& state) {
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
ValueUnit CompoundExp::eval(EvalState& state) {
    string err;
    if (op == "=") {
        if (lhs->getType() != IDENTIFIER) {
            //错误处理
            err="Error:Illegal variable in assignment";
            throw myException(err);
            return 0;
        }
        ValueUnit val = rhs->eval(state);

        state.setValue(((IdentifierExp*)lhs)->getname(), val);
        return val;
    }
    ValueUnit left = lhs->eval(state);
    ValueUnit right = rhs->eval(state);
    if (op == "+") return left + right;
    if (op == "-") return left - right;
    if (op == "*") return left * right;
    if (op == ">") return left>right;
    if (op == "<") return left<right;
    if (op == "/") {
        if (right.getvalInt() == 0) {
            //除零错误
            err="Error:division by zero";
            throw myException(err);
            return 0;
        }
        return left / right;
    }
    if (op == "THEN") {
        if (left.getvalInt() !=0) return right;
        else return ValueUnit(0);
    }
    if(op=="**"){
        int num=pow(left.getvalInt(),right.getvalInt());
        if(num==-2147483648) {
            err="Error: Numeric overflow";
            throw myException(err);
        }
        else
        return ValueUnit(num);
    }
    else{
        //非法运算符
        err="Error:Illegal operator"+op;
        throw myException(err);
    }
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
