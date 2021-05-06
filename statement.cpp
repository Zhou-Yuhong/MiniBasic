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
int REM_statement::get_statement_type(){
    return REM_STATEMENT;
}

void REM_statement::execute(EvalState &state){

}



INPUT_statement::INPUT_statement(string &x){
    var_name=x;
}
int INPUT_statement::get_statement_type(){
    return INPUT_STATEMENT;
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

INPUTS_statement::INPUTS_statement(string &x){
    var_name=x;
}
INPUTS_statement::~INPUTS_statement(){

}
void INPUTS_statement::execute(EvalState &state){
    state.setValue(this->var_name,this->value);
}
int INPUTS_statement::get_statement_type(){
    return INPUTS_STATEMENT;
}

void INPUTS_statement::get_value(string &input){
    this->value=input;
}
string INPUTS_statement::getname(){
    return this->var_name;
}

PRINT_statement::PRINT_statement(Expression *exp)
{
    this->exp=exp;
}


PRINT_statement::~PRINT_statement()
{
    delete exp;
}
string PRINT_statement::getstring(){
    if(this->value.type){
        return to_string(this->value.getvalInt());
    }
    else{
        return this->value.getvalString();
    }
}
int PRINT_statement::get_statement_type(){
    return PRINT_STATEMENT;
}

void PRINT_statement::execute(EvalState &state)
{   if(exp==nullptr) {
    string err="Error: Wrong Expression";
    throw myException(err);
    }
    this->value=exp->eval(state);
}
PRINTF_statement::PRINTF_statement(vector<Expression *> expGroup){
    this->expGroup=expGroup;
}
PRINTF_statement::~PRINTF_statement(){

}
void PRINTF_statement::execute(EvalState &state){
    //得到初始字符串
    Expression *first=this->expGroup[0];
    string origin=first->toString();
    //把要替换的字符全部先存起来
    vector<string> strGroup;
    ValueUnit value;
    string err;
    for(int i=1;i<this->expGroup.size();i++){
        Expression *exp=expGroup[i];
        switch(exp->getType()){
        case CONSTANT:
            strGroup.push_back(exp->toString());
            continue;
        case IDENTIFIER:
           value=((IdentifierExp *)exp)->eval(state);
            strGroup.push_back(value.tostring());
            continue;
        case COMPOUND:
            value=((CompoundExp*)exp)->eval(state);
            strGroup.push_back(value.tostring());
            continue;
        }
    }
    //把初始字符中的{}全部换了
    string::size_type pos(0);
    int j=0;
    while((pos=origin.find("{}"))!=string::npos){
        if(j>=strGroup.size()){
            err="Error: The PRINTF command lack parameters";
            throw myException(err);
        }
        origin.replace(pos,2,strGroup[j++]);
    }
    if(j!=strGroup.size()){
        err="Error: too many parameters for PRINTF command";
        throw myException(err);
    }
    this->final_string=origin;
}
int PRINTF_statement::get_statement_type(){
    return PRINTF_STATEMENT;
}
string PRINTF_statement::get_final_string(){
    return this->final_string;
}

END_statement::END_statement()
{    //空

}

END_statement::~END_statement()
{    //空

}
int END_statement::get_statement_type(){
    return END_STATEMENT;
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
int LET_statement::get_statement_type(){
    return LET_STATEMENT;
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
int GOTO_statement::get_statement_type(){
    return GOTO_STATEMENT;
}

int GOTO_statement::get_line(EvalState&state) {
    return this->exp->eval(state).getvalInt();
}
void GOTO_statement::execute(EvalState& state) {

}
GOTO_statement::~GOTO_statement() {
    //空
}

IF_statement::IF_statement(Expression *input) {
    this->exp = input;
}
int IF_statement::get_statement_type(){
    return IF_STATEMENT;
}

IF_statement::~IF_statement() {
    delete exp;
}
int IF_statement::get_line(EvalState& state) {
    return exp->eval(state).getvalInt();   //IF条件成立，返回目标行数，否则返回0
}
void IF_statement::execute(EvalState &state) {

}
