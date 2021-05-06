#ifndef STATEMENT_H
#define STATEMENT_H
#include "evalstate.h"
#include <vector>
#include "exp.h"
enum{
    REM_STATEMENT=1,INPUT_STATEMENT,INPUTS_STATEMENT,PRINT_STATEMENT,
    END_STATEMENT,LET_STATEMENT,GOTO_STATEMENT,IF_STATEMENT,PRINTF_STATEMENT
};
class Statement{

public:
    Statement();
    virtual ~Statement();
    virtual int get_statement_type()=0;
    virtual void execute(EvalState & state)=0;
};

class REM_statement:public Statement{
public:
    REM_statement();
    ~REM_statement() override;
    int get_statement_type() override;
    void execute(EvalState &state) override;
};
//输入数字
class INPUT_statement:public Statement{
public:
    explicit INPUT_statement(string &x);
    ~INPUT_statement() override;
    void execute(EvalState &state) override;
    int get_statement_type() override;
    void get_value(string &input);
    string getname();
private:
    string var_name;
    int value;
};
//输入字符串
class INPUTS_statement:public Statement{
public:
    //初始化变量名
    explicit INPUTS_statement(string &x);
    ~INPUTS_statement() override;
    void execute(EvalState &state) override;
    int get_statement_type() override;
    //初始化变量值
    void get_value(string &input);
    string getname();
private:
    string var_name;
    string value;
};

class PRINT_statement: public Statement{
public:
    explicit PRINT_statement(Expression *exp);
    ~PRINT_statement() override;
    void execute(EvalState &state) override;
    int get_statement_type() override;
    string getstring();
    ValueUnit value;
private:

    Expression *exp;


};
class PRINTF_statement: public Statement{
public:
    explicit PRINTF_statement(vector<Expression*> expGroup);
    ~PRINTF_statement() override;
    //生成最终的string
    void execute(EvalState &state) override;
    int get_statement_type() override;
    //返回最终的string
    string get_final_string();
private:

    vector<Expression*> expGroup;
    string final_string;

};

class END_statement: public Statement{
public:
    END_statement();
    ~END_statement() override;
    void execute(EvalState &state) override;
    int get_statement_type() override;
};

class LET_statement: public Statement{
public:
    explicit  LET_statement(Expression *exp);
    ~LET_statement() override;
    void execute(EvalState &state) override;
    int get_statement_type() override;
private:
    Expression *exp;
};

class GOTO_statement:public Statement{
public:
    explicit GOTO_statement(Expression *exp);
    ~GOTO_statement() override;
    void execute(EvalState& state) override;
    int get_statement_type() override;
    int get_line(EvalState& state);
private:
    Expression* exp;
};

class IF_statement:public Statement{
public:
    IF_statement(Expression *input);
    ~IF_statement() override;
    void execute(EvalState &state) override;
    int get_statement_type() override;
    int get_line(EvalState &state);
private:
    Expression* exp;
};

#endif // STATEMENT_H
