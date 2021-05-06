#ifndef BUFFER_H
#define BUFFER_H
//#define INT_MAX 2147483647;
//#define INT_MIN (-INT_MAX-1);
using namespace std;
#include <string>
#include <vector>
#include <map>
#include <set>
#include "statement.h"
#include "exp.h"
#include <stack>
#include <error.h>
enum TokenType { WORDS, NUMBER, STRING, OPERATOR ,BRACKET};
struct node {
    int line_number;
    int read_index = 1;
    vector<string> content;
    node* next;
    Statement* state;
    Expression* expr = nullptr;
    node(int numget, vector<string>& input, node* T = NULL) {
        line_number = numget;
        content = input;
        next = T;
       // state = P;
    }
    string present_token() {
        return content[read_index];
    }
    TokenType getTokenType(string token);
    int precedence(string token); //判断字符（运算符）优先级
    void initial_exp();  //分离出生成exp的环节
    void initial_state();//分离生成state的环节
    void set_status();
    void transform();
    void LL(Expression**t);
    void handle_negative();
    string handle_string();
    vector<Expression*> printf_prework(); //处理类型为PRINTF的state的提前工作
    bool isnum(string str);
};
class Buffer
{
private:

public:
    node* head;
    //    map<int,string> source_code;
    Buffer();
    ~Buffer();
    void clear();
    //在特殊字符前后添加空格
    void addspace(string &input);

    int inputstring(string& input);
    void addnode(node*& T);
    void initial();
    void run(EvalState &run_state);
    node* gotoline(int num);
};

#endif // BUFFER_H

