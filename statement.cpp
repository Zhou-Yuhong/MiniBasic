#include "statement.h"
#include "sstream"
#include "error.h"
Statement::Statement(){

}
Statement::~Statement(){

}

REM_statement::REM_statement(){

}
REM_statement::~REM_statement(){

}

void REM_statement::execute(EvalState &state){

}



INPUT_statement::INPUT_statement(string &x){
    var_name=x;
}

INPUT_statement::~INPUT_statement()
{

}
string INPUT_statement::getname() {
    return this->var_name;
}
void INPUT_statement::get_value(string &input)
{
//    stringstream ss(input);
//    int num=0;
//    ss>>num;
    string err;
    if(input=="0") {
        this->value=0;
        return;}
    int num =atoi(input.c_str());
    if(num==0){
        err="Error: Input is not a number";
        throw myException(err);
    }
    this->value=num;
}
void INPUT_statement::execute(EvalState &state)
{
    state.setValue(this->var_name,this->value);
}



PRINT_statement::PRINT_statement(Expression *exp)
{
    this->exp=exp;
}

PRINT_statement::~PRINT_statement()
{
    delete exp;
}

void PRINT_statement::execute(EvalState &state)
{   if(exp==nullptr) {
    string err="Error: Wrong Expression";
    throw myException(err);
    }
    this->value=exp->eval(state);
}





END_statement::END_statement()
{    //空

}

END_statement::~END_statement()
{    //空

}

void END_statement::execute(EvalState &state)
{
     //返回结束信号
}






LET_statement::LET_statement(Expression *exp)
{
    this->exp=exp;
}

LET_statement::~LET_statement()
{
    delete exp;
}

void LET_statement::execute(EvalState &state)
{
    //Expression* p=((CompoundExp*)exp)->getlhs();
   // int value=((CompoundExp*)exp)->eval(state);
    //state.setValue(((IdentifierExp*)p)->getname(),value);
    exp->eval(state);   //=对应的exp的eval函数有处理功能
}

GOTO_statement::GOTO_statement(Expression *exp) {
    this->exp=exp;
}
int GOTO_statement::get_line(EvalState&state) {
    return this->exp->eval(state);
}
void GOTO_statement::execute(EvalState& state) {

}
GOTO_statement::~GOTO_statement() {
    //空
}

IF_statement::IF_statement(Expression *input) {
    this->exp = input;
}
IF_statement::~IF_statement() {
    delete exp;
}
int IF_statement::get_line(EvalState& state) {
    return exp->eval(state);   //IF条件成立，返回目标行数，否则返回0
}
void IF_statement::execute(EvalState &state) {

}
