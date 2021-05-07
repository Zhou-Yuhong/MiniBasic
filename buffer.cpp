#include "buffer.h"
#include <sstream>
#include<iostream>
#include <stack>
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
   static char special_char[] = { '+'   ,   '-'    ,'*' ,  '/'  ,'('  ,  ')' , '=','>','<','\"','\'',',' };
   static set<char> myset(special_char, special_char + 12);
    for (int i = 0; i < input.length(); i++) {
        if (myset.find(input[i]) != myset.end()) {
            if(input[i]!='*'){
            input.insert(i, " ");
            i += 2;
            input.insert(i, " ");
            continue;
            }
            else{
                if(i!=input.length()-1&&input[i+1]=='*'){
                    input.insert(i," ");
                    i+=3;
                    input.insert(i," ");
                    continue;
                }
                else{
                    input.insert(i, " ");
                    i += 2;
                    input.insert(i, " ");
                    continue;
                }
            }
        }
    }
    return;
}
int Buffer::inputstring(string &input) {
    string err;
    if(input=="") {
        err="Error: No Input";
        throw myException(err);
    }
    addspace(input);
    stringstream ss(input);
    int numget=0;  //0做标识
    string singlestring;
    //string totlestring="";
    vector<string> totlestring;
    ss >> numget;  //未读到东西numget变成0
    if(numget==0){
        //表示未读到数字，意味着是指令
        stringstream tt(input);
        tt>>singlestring;
        if(singlestring=="RUN") return 1;
        if(singlestring=="LOAD") return 2;
        if(singlestring=="LIST") return 3;
        if(singlestring=="CLEAR") return 4;
        if(singlestring=="HELP")  return 5;
        if(singlestring=="QUIT") return 6;
        if(singlestring=="INPUT") return 7;
        if(singlestring=="LET") return 8;
        if(singlestring=="PRINT") return 9;
        if(singlestring=="INPUTS") return 10;
        if(singlestring=="PRINTF") return 11;
        else{
           err="Error:Unknown command "+input;
           throw myException(err);
        }


    }
    //读到了数字
    while (ss >> singlestring) {
        totlestring.push_back(singlestring);
    }
    node* p = new node(numget, totlestring);
    addnode(p);
    //提前处理负号
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
            //cout << ((PRINT_statement*)p->state)->value << endl;
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
    if (token=="\""||token=="\'") return STRING;
    if (isnum(token)||(token.size()>1&&token[0]=='-'&&isnum(token.substr(1)))) return NUMBER;
    if (token=="+"||token=="-"||token=="*"||token=="/"||token=="<"||token==">"||token=="THEN"||token=="="||token=="**") return OPERATOR;
    if(ch=='('||ch==')')
    return BRACKET;
    return WORDS;
}


int node::precedence(string token)
{
    if (token == "="||token=="THEN") return 1;
    if (token==">"||token=="<") return 2;
    if(token == "+" || token == "-") return 3;
    if (token == "*" || token == "/") return 4;
    if(token=="**") return 5;
    return 0;

}
void node::initial_state(){
    //根据state类型进行初始化
    //初始化state
    string state_type = content[0];
    string err;
    if (state_type == "REM") {
        state = new REM_statement();
        return;
    }
    if (state_type == "LET") {
        //初始化exp
        if(expr==nullptr){
        this->initial_exp();
        }
        state = new LET_statement(this->expr);
        return;
    }
    if (state_type == "IF") {
        this->initial_exp();
        state = new IF_statement(this->expr);
        return;
    }
    if (state_type == "PRINT") {
        if(expr==nullptr){
        this->initial_exp();
        }
        state = new PRINT_statement(this->expr);
        return;
   }
    if (state_type == "GOTO"){
        if(expr==nullptr){
        this->initial_exp();
        }
        state = new GOTO_statement(this->expr);
        return;
    }
    if (state_type == "END") {
        state = new END_statement();
        return;
    }
    if (state_type == "INPUT") {
        if(expr==nullptr){
        this->initial_exp();
        }
        if(this->expr->getType()!=IDENTIFIER){
            err="Error: INPUT should follow a variable";
            throw myException(err);
        }
        string name = ((IdentifierExp*)this->expr)->getname();
        state = new INPUT_statement(name);
        return;
    }
    if(state_type=="INPUTS"){
        if(expr==nullptr){
        this->initial_exp();
        }
        if(this->expr->getType()!=IDENTIFIER){
            err="Error: INPUTS should follow a variable";
            throw myException(err);
        }
        string name = ((IdentifierExp*)this->expr)->getname();
        state=new INPUTS_statement(name);
        return;
    }
    if(state_type=="PRINTF"){
        //只有printf不需要初始化exp
        this->state=new PRINTF_statement(this->printf_prework());
        return;
    }
    //错误处理
    else{
        err="unkown operation "+state_type;
        throw myException(err);
    }
}

void node::initial_exp(){
    //防止PRINTF进入这里
    if(this->content[0]=="PRINTF") return;
    //初始化exp指针
    handle_negative();//处理负数（加0）
    string err;
    if(content.size()==1){
        err="Error:Line "+to_string(this->line_number)+" is empty";
        throw myException(err);
    }
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
        //如果是STRING类型
        if(type==STRING){
            //提取该string,去掉引号
            string get=this->handle_string();
            //生成string类型的Constant
            Expression* exp=new ConstantExp(get);
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
            if(oper.empty()){
                err="Error:Wrong Math Expression";
                throw myException(err);
            }
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
                tmp= ((CompoundExp*)oper.top())->getop();
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
    this->transform();
    if(expr==nullptr){
        err="Error: Wrong Expression";
        throw myException(err);
    }
    //exp指针初始化完成
}

void node::set_status()
{  
 this->initial_state();
}

void node::transform()
{
    Expression **p=&(this->expr);
    Expression **q=nullptr;
    while((*p)!=nullptr&&(*p)->isop()){
         q=&(((CompoundExp*)*p)->lhs);
        if((((CompoundExp*)*p)->getop()=="**")&&((*q)->isop())&&(((CompoundExp*)*q)->getop()=="**")){
         LL(p);

          }
        p=q;
    }
}

void node::LL(Expression **t)
{    Expression *t1=((CompoundExp*)*t)->lhs;
     ((CompoundExp*)*t)->lhs=((CompoundExp*)t1)->rhs;
     ((CompoundExp*)t1)->rhs=*t;
     *t=t1;

}
string node::handle_string(){
    //读出"  或者 '
    string mark=this->content[read_index-1];
    string result="";
    bool flag=false;//只有第一次不加空格，其他要加
    while(read_index<this->content.size()&&this->content[read_index]!=mark){
        if(!flag){
            result+=this->content[read_index];
            flag=true;
            read_index++;
            continue;
        }
        else{
            result+=" "+this->content[read_index];
            read_index++;
            continue;
        }
    }
    //退出的时候read_index停留在" 或'处，所以要把read_index加1，跳过这里
    this->read_index++;
    return result;
}
vector<Expression*> node::printf_prework(){
    string token;
    vector<Expression*> result;
    while(read_index<this->content.size()){
        token=this->content[read_index];
        read_index++;
        if(token==",") continue;
        TokenType type=getTokenType(token);
        if(type==WORDS){
            Expression* exp=new IdentifierExp(token);
            result.push_back(exp);
            continue;
        }
        if(type==NUMBER){
            Expression * exp=new ConstantExp(atoi(token.c_str()));
            result.push_back(exp);
            continue;
        }
        if(type==STRING){
            string str=this->handle_string();
            Expression *exp=new ConstantExp(str);
            result.push_back(exp);
            continue;
        }
    }
    return result;
}

void node::handle_negative()
{   stack<int> mark;
    string err;
    if(content.empty()) return;
    if(content.size()==1) return;
    for(int i=1;i<content.size();i++){
        if(i==1&&content[1]=="-"){
           mark.push(1);
           if(i+1>=content.size()){
               err="Error:Error in mathematical expression";
               throw myException(err);
           }

        }
        if(i>1){
            if(content[i]=="-"&&(content[i-1]=="("||content[i-1]=="=")){
                mark.push(i);
                if(i+1>=content.size()){
                    err="Error:Error in mathematical expression";
                    throw myException(err);
                }
            }
            if(content[i]=="-"){
                if(i+1>=content.size()){
                    err="Error:Error in mathematical expression";
                    throw myException(err);
                }
                if(content[i+1]=="("){
                    mark.push(i);
                }
            }
        }
    }
    while(!mark.empty()){
        int num=mark.top();
        mark.pop();
        //vec.erase(vec.begin()+2);
        content.insert(content.begin()+num,"0");
    }
}

bool node::isnum(string str)
{
    for (int i = 0; i<str.size(); i++)
       {
           if (!isdigit(str[i]))
           {

               return false;
           }
       }
       return true;


}








