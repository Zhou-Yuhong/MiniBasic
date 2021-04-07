#include "buffer.h"
#include <sstream>
#include<iostream>
Buffer::Buffer()
{

    string head_string = "this is head";
    vector<string> head_content;
    head_content.push_back(head_string);
    head = new node(INT_MIN, head_content);
}

Buffer::~Buffer()
{
    node* tmp = head->next;
    node* p = tmp;
    while (tmp != NULL) {
        p = tmp->next;
        delete tmp;
        tmp = p;
    }
    head->next = NULL;

}

void Buffer::clear()
{
    node *T=head->next;
    node *p;
    while(T!=NULL){
        p=T->next;
        delete T;
        T=p;
    }
    head->next=NULL;

}
void Buffer::addspace(string &input) {
   static char special_char[] = { '+'   ,   '-'    ,'*' ,  '/'  ,'('  ,  ')' , '=','>','<' };
   static set<char> myset(special_char, special_char + 9);
    for (int i = 0; i < input.length(); i++) {
        if (myset.find(input[i]) != myset.end()) {
            input.insert(i, " ");
            i += 2;
            input.insert(i, " ");
        }
    }
}
int Buffer::inputstring(string &input) {
    string err;
    addspace(input);
    stringstream ss(input);
    int numget=0;  //0做标识
    string singlestring;
    //string totlestring="";
    vector<string> totlestring;
    ss >> numget;  //未读到东西numget变成0
    if(numget==0){
        //表示未读到数字，意味着是指令

        if(input=="RUN") return 1;
        if(input=="LOAD") return 2;
        if(input=="LIST") return 3;
        if(input=="CLEAR") return 4;
        if(input=="HELP")  return 5;
        if(input=="QUIT") return 6;
        else{
           err="Error:Unknown command "+input;
           throw myException(err);
        }
    }
    while (ss >> singlestring) {
        totlestring.push_back(singlestring);
    }
    node* p = new node(numget, totlestring);
    addnode(p);
    //  source_code.insert(map<int,string>::value_type(numget,totlestring));
    return 0;//如果是代码，返回0
}
void Buffer::initial() {
    node* p = head->next;
    while (p != nullptr) {
        if(p->expr==nullptr)
        p->set_status();
        p = p->next;
    }
}
void Buffer::run(EvalState& run_state) {
    node* p = head->next;
    while (p != nullptr) {
        if (p->expr == nullptr)
            p->set_status();
        string state_type = p->content[0];
        if (state_type == "REM") {
            p = p->next;
            continue;
        }
        if (state_type == "LET") {
            p->state->execute(run_state);
            p = p->next;
            continue;
        }
        if (state_type == "IF") {
            int linenum = ((IF_statement*)p->state)->get_line(run_state);
            if (linenum == 0) {
                p = p->next;
                continue;
            }
            else {
                p = gotoline(linenum);
                continue;
            }
        }
        if (state_type == "PRINT") {
            p->state->execute(run_state);
            cout << ((PRINT_statement*)p->state)->value << endl;
            p = p->next;
            continue;
        }
        if (state_type == "GOTO") {
            int linenum = ((GOTO_statement*)p->state)->get_line(run_state);
            p = gotoline(linenum);
            continue;
        }
        if (state_type == "INPUT") {
            cout << ((INPUT_statement*)p->state)->getname() << "?";
            string input;
            cin >> input;
            ((INPUT_statement*)p->state)->get_value(input);
            ((INPUT_statement*)p->state)->execute(run_state);
            p = p->next;
        }
        if (state_type == "END") {
            break;
        }
    }
}

node* Buffer::gotoline(int num) {
    node* p = head->next;
    while (p!=nullptr &&p->line_number != num) p = p->next;
    return p;
}
void Buffer::addnode(node*& T)
{
    node* tmp = head;
    while (tmp->next != NULL && tmp->next->line_number < T->line_number) {
        tmp = tmp->next;
    }
    if (tmp->next == NULL) {
        tmp->next = T;
    }
    else {
        if (tmp->next->line_number == T->line_number) {
            tmp->next->content = T->content;
            tmp->next->expr = nullptr;
            tmp->next->read_index = 1;
        }
        else {
            T->next = tmp->next;
            tmp->next = T;
        }
    }
}

TokenType node::getTokenType(string token)
{
    char ch = token[0];
    if (ch == '"' || (ch == '\'' && token.length() > 1)) return STRING;
    if (isdigit(ch)) return NUMBER;
    if (token=="+"||token=="-"||token=="*"||token=="/"||token=="<"||token==">"||token=="THEN"||token=="=") return OPERATOR;
    if(ch=='('||ch==')')
    return BRACKET;
    return WORDS;
}

Expression* node::readT()
{
    string token = content[read_index];
    read_index++;
    TokenType type = getTokenType(token);
    if (type == WORDS) return new IdentifierExp(token);
    if (type == NUMBER) return new ConstantExp(atoi(token.c_str()));
    if (token != "(") {
        //错误待处理
        return nullptr;
    }
    Expression* exp = readE();
    if (read_index < content.size() && content[read_index] != ")") {
        //错误待处理
        return nullptr;
    }
    return exp;
}

Expression* node::readE(int prec)
{
    Expression* exp = readT();
    string token;
    while (true) {
        if (read_index >= content.size()) break;
        token = content[read_index];
        read_index++;
        int newprec = precedence(token);
        if (newprec > prec) {
            Expression* rhs = readE(newprec);
            exp = new CompoundExp(token, exp, rhs);
        }

    }
    return exp;

}

int node::precedence(string token)
{
    if (token == "="||token=="THEN") return 1;
    if (token==">"||token=="<") return 2;
    if(token == "+" || token == "-") return 3;
    if (token == "*" || token == "/") return 4;
    return 0;

}

void node::set_status()
{
    //if (content.size() == 1) return;
    //this->expr = readE();
    //if (read_index < content.size()) {
    //    //有错，需要处理
    //    return;
    //}
    //初始化exp指针
    string err;
    stack<Expression*> oper;
    stack<Expression*> iden;
    string token;
    if(read_index>=content.size()) return;
    while (read_index < content.size()) {
        token = content[ read_index];
        read_index++;
        TokenType type = getTokenType(token);
        if (type == WORDS ) {
            Expression* exp = new IdentifierExp(token);
            iden.push(exp);
            continue;
        }
        if (type == NUMBER) {
            Expression* exp = new ConstantExp(atoi(token.c_str()));
            iden.push(exp);
            continue;
        }
        if (type == OPERATOR) {
            if (oper.empty()) {
                Expression* exp = new CompoundExp(token);
                oper.push(exp);
                continue;
            }
            int new_pre = precedence(token);
            int old_pre = precedence(((CompoundExp*)oper.top())->getop());
            while (old_pre >= new_pre&&(!oper.empty())) {
               Expression* tmp = oper.top();
               oper.pop();
               //错误处理
               if(iden.empty()){
                   err="Error:Wrong Math Expression";
                   throw myException(err);
               }
               Expression* rhs = iden.top();
               iden.pop();
               //错误处理
               if(iden.empty()){
                   err="Error:Wrong Math Expression";
                   throw myException(err);
               }
               Expression* lhs = iden.top();
               iden.pop();
               ((CompoundExp*)tmp)->setchild(lhs, rhs);
               iden.push(tmp);
               if (!oper.empty()) {
                   old_pre = precedence(((CompoundExp*)oper.top())->getop());
               }
            }
            Expression* exp = new CompoundExp(token);
            oper.push(exp);
            continue;

        }
        if (token == "(") {
            Expression* exp = new CompoundExp(token);   //用运算符类型的来处理下括号
            oper.push(exp);
            continue;
        }
        if (token == ")") {
            string tmp = ((CompoundExp*)oper.top())->getop();
            while (tmp != "(") {

                Expression* exp = oper.top();
                oper.pop();
                //错误处理
                if(iden.empty()){
                    err="Error:Wrong Math Expression";
                    throw myException(err);
                }
                //
                Expression* rhs = iden.top();
                iden.pop();
                //错误处理
                if(iden.empty()){
                    err="Error:Wrong Math Expression";
                    throw myException(err);
                }
                //
                Expression* lhs = iden.top();
                iden.pop();
                ((CompoundExp*)exp)->setchild(lhs, rhs);
                iden.push(exp);
                //缺少前括号,错误处理
                if(oper.empty()){
                     err="Expression error missing '('";
                    throw myException(err);
                }
                //
            }
            oper.pop();
            continue;
        }
    }
    while (!oper.empty()) {
        Expression* expget = oper.top();
        if(((CompoundExp*)expget)->getop()=="("){
            err="Expression error missing ')'";
            throw myException(err);
        }
        oper.pop();
        //错误处理
        if(iden.empty()){
            err="Error:Wrong Math Expression";
            throw myException(err);
        }
        //
        Expression* rhs = iden.top();
        iden.pop();
        //错误处理
        if(iden.empty()){
            err="Error:Wrong Math Expression";
            throw myException(err);
        }
        //
        Expression* lhs = iden.top();
        iden.pop();
        ((CompoundExp*)expget)->setchild(lhs, rhs);
        iden.push(expget);
    }
    this->expr = iden.top();
    //exp指针初始化完成

    //初始化state指针
    string state_type = content[0];
    if (state_type == "REM") {
        state = new REM_statement();
        return;
    }
    if (state_type == "LET") {
        state = new LET_statement(this->expr);
        return;
    }
    if (state_type == "IF") {
        state = new IF_statement(this->expr);
        return;
    }
    if (state_type == "PRINT") {
        state = new PRINT_statement(this->expr);
        return;
   }
    if (state_type == "GOTO"){
        state = new GOTO_statement(this->expr);
        return;
    }
    if (state_type == "END") {
        state = new END_statement();
        return;
    }
    if (state_type == "INPUT") {
        string name = ((IdentifierExp*)this->expr)->getname();
        state = new INPUT_statement(name);
        return;
    }
    //错误处理
    else{
        err="unkown operation "+state_type;
        throw myException(err);
    }
    //
}








