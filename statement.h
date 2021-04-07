#ifndef STATEMENT_H
#define STATEMENT_H
#include "evalstate.h"
#include "exp.h"

class Statement{

public:
    Statement();
    virtual ~Statement();
    virtual void execute(EvalState & state)=0;
};

class REM_statement:public Statement{
public:
    REM_statement();
    ~REM_statement() override;
    void execute(EvalState &state) override;
};

class INPUT_statement:public Statement{
public:
    explicit INPUT_statement(string &x);
    ~INPUT_statement() override;
    void execute(EvalState &state) override;
    void get_value(string &input);
    string getname();
private:
    string var_name;
    int value;
};

class PRINT_statement: public Statement{
public:
    explicit PRINT_statement(Expression *exp);
    ~PRINT_statement() override;
    void execute(EvalState &state) override;
     int value;
private:

    Expression *exp;


};

class END_statement: public Statement{
public:
    END_statement();
    ~END_statement() override;
    void execute(EvalState &state) override;
};

class LET_statement: public Statement{
public:
    explicit  LET_statement(Expression *exp);
    ~LET_statement() override;
    void execute(EvalState &state) override;
private:
    Expression *exp;
};

class GOTO_statement:public Statement{
public:
    explicit GOTO_statement(Expression *exp);
    ~GOTO_statement() override;
    void execute(EvalState& state) override;
    int get_line(EvalState& state);
private:
    Expression* exp;
};

class IF_statement:public Statement{
public:
    IF_statement(Expression *input);
    ~IF_statement() override;
   void execute(EvalState &state) override;
    int get_line(EvalState &state);
private:
    Expression* exp;
};

#endif // STATEMENT_H
