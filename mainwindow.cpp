#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <sstream>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    dialog=new QDialog(this);
    dialog->setMinimumSize(400,400);
    dialog->setWindowTitle("help");
    helpinf=new QTextBrowser(dialog);
    helpinf->setMinimumSize(400,400);
    dialog->hide();
    QString help="RUN: Executes the input code\n"
                 "LOAD:Read code from an external file\n"
                 "LIST:none\n"
                 "CLEAR:deletes the program and you can start entering a new one\n"
                 "QUIT:close the window\n"
            ;
    helpinf->insertPlainText(help);



    connect(ui->Button_Run,&QPushButton::clicked,[=](){
        Run();
    });

    connect(ui->Button_Load,&QPushButton::clicked,[=](){
//        string str=(ui->lineEdit->displayText()).toStdString();
//        inputString(str);
        load();
//        ui->Code_Display->clear();
//        showcode();
//        ui->lineEdit->clear();

    });
    connect(ui->Button_Clear,&QPushButton::clicked,[=](){
    clear();
    });

}
MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::keyPressEvent(QKeyEvent *event){
    if(event->key()==KEYBOARD_ENTER){
        if(!if_run){
        string str=(ui->lineEdit->displayText()).toStdString();
        InputString(str);
        ui->Code_Display->clear();
        showcode();
        ui->lineEdit->clear();
        }
        if(if_run){           //程序开始运行
            string str=(ui->lineEdit->displayText()).toStdString();
            valinput=str;
            ui->lineEdit->clear();
            Run();
        }
    }
}

void MainWindow::showcode()
{   node* p=buffer.head->next;
    while(p!=NULL){
        string str=to_string(p->line_number)+" ";
        for(int i=0;i<p->content.size();i++){
            str+=p->content[i]+" ";
        }
        ui->Code_Display->insertPlainText(QString::fromStdString(str));
        ui->Code_Display->append("");
        p=p->next;
    }
}
//std::map<CString, double>::iterator it;
//it = map.begin();
//while (it != map.end())
//{
//	CString line = it->second.ToString();
//	// PiLiPaLa
//	it++;
//}
void MainWindow::show_current_variables(){
    //先清空上次的残留
    this->ui->Variable_Display->clear();
    std::map<string,ValueUnit>::iterator it;
    it=this->run_state.getmap().begin();
    while(it!=this->run_state.getmap().end()){
        string str=it->first+": ";
        if(it->second.type){
            //int 类型
            str+="INT = ";
            str+=it->second.tostring();
        }
        else{
            //string 类型
            str+="STR = ";
            str+=it->second.tostring();
        }
        it++;
        ui->Variable_Display->insertPlainText(QString::fromStdString(str));
        ui->Variable_Display->append("");
    }

}

void MainWindow::show_runcode(string str)
{
    ui->Result_Display->insertPlainText(QString::fromStdString(str));
    ui->Result_Display->append("");
    interaction="";   //重置
}

void MainWindow::inputString(string str)
{
    //如果是input或者inputs的后续输入
    if(node_input_flag){
        if(ins_node->state->get_statement_type()==INPUT_STATEMENT){
                 ((INPUT_statement*)ins_node->state)->get_value(str);
                 ((INPUT_statement*)ins_node->state)->execute(run_state);
                    node_input_flag=false;
                    return;
        }
        if(ins_node->state->get_statement_type()==INPUTS_STATEMENT){
                 ((INPUTS_statement*)ins_node->state)->get_value(str);
                 ((INPUTS_statement*)ins_node->state)->execute(run_state);
                    node_input_flag=false;
                    return;
        }
    }
    //针对下面的case7、8、9进行提前处理
    string copy=str;
    buffer.addspace(copy);
    stringstream handle(copy);
    vector<string> inf_vec;
    string singlestring;
    string err;
    while(handle>>singlestring){
        inf_vec.push_back(singlestring);
    }

    int ins_type=this->buffer.inputstring(str);
    switch(ins_type){
    //0为输入代码，1为Run,2为LOAD，3为LIST，4为CLEAR，5为HELP，6为QUIT,7为INPUT，8为LET，9为PRINT,10为INPUTS,11为PRINTF
      case 0:
        return;
      case 1:
        Run();
        return;
      case 2:
        load();
        return;
      case 3:
        return;
      case 4:
        clear();
        return;
      case 5:
        help();
        return;
      case 6:
        this->window()->close();
      case 7:
         delete ins_node;
        ins_node=new node(0,inf_vec);
        ins_node->set_status();
        interaction= ((INPUT_statement*)ins_node->state)->getname()+"?";
        show_runcode(interaction);
        node_input_flag=true;
        return;
      case 8:
        delete ins_node;
        ins_node=new node(0,inf_vec);
        ins_node->set_status();
        if(ins_node->expr==nullptr){
            err="Error: Wrong Expression";
            throw myException(err);
        }
        ins_node->state->execute(run_state);
        return;
      case 9:
        delete ins_node;
        ins_node=new node(0,inf_vec);
        ins_node->set_status();
        if(ins_node->expr==nullptr){
            err="Error: Wrong Expression";
            throw myException(err);
        }
        ins_node->state->execute(run_state);
        interaction=((PRINT_statement*)ins_node->state)->getstring();
        show_runcode(interaction);
        return;
       case 10:
       delete ins_node;
       ins_node=new node(0,inf_vec);
       ins_node->set_status();
       interaction= ((INPUTS_statement*)ins_node->state)->getname()+"?";
       show_runcode(interaction);
       node_input_flag=true;
       return;
       case 11:
       delete ins_node;
       ins_node=new node(0,inf_vec);
       ins_node->set_status();
       ins_node->state->execute(run_state);
       interaction=((PRINTF_statement*)ins_node->state)->get_final_string();
       show_runcode(interaction);
       return;
    }
}

void MainWindow::InputString(string str)
{
    try{
        inputString(str);
    }
    catch(myException e){

        ui->Result_Display->insertPlainText(QString::fromStdString(e.report()));
        ui->Statement_Display->append("");
    }

}

void MainWindow::run()
{   if(!if_run) return;
    static bool flag=true;//控制input只进行一次
    node *p;
    string err;
    if(run_line==-1){
    p = buffer.head->next;}
    else{
        p=buffer.gotoline(run_line);
    }
       while (p != nullptr) {
           if(p->content.empty()){
               p=p->next;
               continue;
           }
           //更新正在跑的行
           run_line=p->line_number;
           //错误处理
           string state_type = p->content[0];
           if(state_type!="REM"&&state_type!="LET"&&state_type!="IF"&&state_type!="PRINT"&&state_type!="END"&&state_type!="INPUT"
                   &&state_type!="GOTO"&&state_type!="INPUTS"&&state_type!="PRINTF"){
               err="Error: Unknown state "+state_type;
               throw myException(err);
           }
           if (p->expr == nullptr&&state_type!="END")
               p->set_status();          
           if (state_type == "REM") {
               //语法树
               ui->Statement_Display->insertPlainText(QString::number(p->line_number));
               ui->Statement_Display->insertPlainText(QString::fromStdString(" "+state_type+" "));
               ui->Statement_Display->append("");
               string str;
               for(int i=1;i<p->content.size();i++){
                   str+=p->content[i]+" ";
               }
               ui->Statement_Display->insertPlainText(QString::fromStdString(str));
               ui->Statement_Display->append("");
               //语法树结束
               p = p->next;
               continue;
           }
           if (state_type == "LET") {
               if(p->expr==nullptr){
                   err="Error: Wrong Expression";
                   throw myException(err);
               }
               p->state->execute(run_state);
               //语法树
               ui->Statement_Display->insertPlainText(QString::number(p->line_number));
               ui->Statement_Display->insertPlainText(QString::fromStdString(" "+state_type+" "));
               printtree(p->expr);
               //语法树结束
               p = p->next;      
               continue;
           }
           if (state_type == "IF") {
               int linenum = ((IF_statement*)p->state)->get_line(run_state);
               if(linenum!=0&&buffer.gotoline(linenum)==nullptr){
                   err="Syntax error: Line number "+std::to_string(linenum)+" does not exist";
                   throw myException(err);
               }
               //语法树
               ui->Statement_Display->insertPlainText(QString::number(p->line_number));
               ui->Statement_Display->insertPlainText(QString::fromStdString(" "+state_type+" "));
               PrintIftree(p->expr);
               //语法树结束

               if (linenum == 0) {
                   p = p->next;
                   continue;
               }
               else {
                   p = buffer.gotoline(linenum);
                   continue;
               }
           }
           if (state_type == "PRINT") {
               if(p->expr==nullptr){
                   err="Error: Wrong Expression";
                   throw myException(err);
               }
               p->state->execute(run_state);
               interaction=((PRINT_statement*)p->state)->getstring();
               show_runcode(interaction);
               //语法树
               ui->Statement_Display->insertPlainText(QString::number(p->line_number));
               ui->Statement_Display->insertPlainText(QString::fromStdString(" "+state_type+" "));
               printtree(p->expr);
               //语法树结束
               p = p->next;

               continue;
           }
           if(state_type=="PRINTF"){
               p->state->execute(run_state);
               interaction=((PRINTF_statement*)p->state)->get_final_string();
               show_runcode(interaction);
               //语法树
               ui->Statement_Display->insertPlainText(QString::number(p->line_number));
               ui->Statement_Display->insertPlainText(QString::fromStdString(" "+state_type+" "));
               ui->Statement_Display->insertPlainText(QString::fromStdString(interaction));
                 //
               p=p->next;
               continue;
           }
           if (state_type == "GOTO") {

               int linenum = ((GOTO_statement*)p->state)->get_line(run_state);
               node* q=p;
               p = buffer.gotoline(linenum);
               //错误处理
               if(p==nullptr){
                   err="Syntax error: Line number "+std::to_string(linenum)+" does not exist";
                   throw myException(err);
               }
               //语法树
               ui->Statement_Display->insertPlainText(QString::number(q->line_number));
               ui->Statement_Display->insertPlainText(QString::fromStdString(" "+state_type+" "));
               printtree(q->expr);
               //语法树结束
               //
               continue;
           }
           if (state_type == "INPUT") {
               if(valinput==""){               //未得到输入，退出该函数
                   if(!flag) return;
                   if(p->expr->getType()!=IDENTIFIER){
                       err="Error: Input should be followed by a variable";
                       throw myException(err);
                   }
                   interaction= ((INPUT_statement*)p->state)->getname()+"?";
                   show_runcode(interaction);
                   flag=false;
                   return;
               }
               ((INPUT_statement*)p->state)->get_value(valinput);
               //execute把变量和值加入到run_state中
               ((INPUT_statement*)p->state)->execute(run_state);
               valinput="";
               flag=true;
               p = p->next;
               continue;
           }
           if(state_type=="INPUTS"){
               if(valinput==""){
                   //未得到输入，退出该函数
                   if(!flag) return;    //flag防止跑两次
                   if(p->expr->getType()!=IDENTIFIER){
                       err="Error: Input should be followed by a variable";
                       throw myException(err);
                   }
                   interaction=((INPUTS_statement *)p->state)->getname()+"?";
                   show_runcode(interaction);
                   flag=false;
                   return;
               }
               //把得到的输入加入到run_state中
               ((INPUTS_statement*)p->state)->get_value(valinput);
               ((INPUTS_statement*)p->state)->execute(run_state);
               //重置，为下一次INPUTS做准备
               valinput="";
               flag=false;
               p=p->next;
               continue;
           }

           if (state_type == "END") {
               break;
           }
       }
       if(p==nullptr) if_run=false;
}

void MainWindow::Run()
{   this->if_run=true;
    try{
        run();
    }
    catch(myException e){
        if(!if_throw){
        string err="Line "+to_string(run_line)+" "+e.report();
        ui->Result_Display->insertPlainText(QString::fromStdString(err));
        ui->Statement_Display->append("");
        if_throw=true;
        return;
        }

    }
}
//载入文件并显示
void MainWindow::load()
{
    QString path=QFileDialog::getOpenFileName(this,"打开文件","C:\\Users\\周昱宏\\Desktop");
    QFile file(path);
    file.open(QIODevice::ReadOnly);
    while(file.atEnd() == false)//文件没到末尾就执行循环体内容
    {
        QByteArray buf = file.readLine();//读取一行的数据
        QString str(buf);
        string input=str.toStdString();
        InputString(input);
    }
    file.close();
    ui->Code_Display->clear();
    showcode();
    ui->lineEdit->clear();
}

void MainWindow::help()
{
    this->dialog->show();
}

void MainWindow::printtree(Expression *p)
{
   queue<Expression*> q1;
   queue<Expression*> q2;
   Expression* tmp;
   string sget;
   string stab;
   int tabnum=0;
   if(p==nullptr) return;
   q1.push(p);
   while(!q1.empty()||!q2.empty()){
       stab="";
       for(int i=0;i<tabnum;i++){
           stab+="\t";
       }
       while(!q1.empty()){
           tmp=q1.front();
           sget=tmp->gettoken();
           q1.pop();
//           ui->Code_Display->insertPlainText(QString::fromStdString(str));
//           ui->Code_Display->append("");
           if(tmp->isop()){
               if(((CompoundExp*)tmp)->getlhs()){
                   q2.push(((CompoundExp*)tmp)->getlhs());
               }
               if(((CompoundExp*)tmp)->getrhs()){
                   q2.push(((CompoundExp*)tmp)->getrhs());
               }
           }
           sget=stab+sget;
           ui->Statement_Display->insertPlainText(QString::fromStdString(sget));
           ui->Statement_Display->append("");
       }
       stab="";
       tabnum++;
       for(int i=0;i<tabnum;i++){
           stab+="\t";
       }
       while(!q2.empty()){
           tmp=q2.front();
           sget=tmp->gettoken();
           q2.pop();
           if(tmp->isop()){
               if(((CompoundExp*)tmp)->getlhs()){
                   q1.push(((CompoundExp*)tmp)->getlhs());
               }
               if(((CompoundExp*)tmp)->getrhs()){
                   q1.push(((CompoundExp*)tmp)->getrhs());
               }
           }
           sget=stab+sget;
           ui->Statement_Display->insertPlainText(QString::fromStdString(sget));
           ui->Statement_Display->append("");
       }
      tabnum++;
   }
}

void MainWindow::PrintIftree(Expression *p,bool flag)
{
    if(p==nullptr) return;
    string sget;
    if(flag){
        sget=p->gettoken();
        ui->Statement_Display->insertPlainText(QString::fromStdString(sget));
        ui->Statement_Display->append("");
        flag=false;
    }
    else{
        sget="\t"+p->gettoken();
        ui->Statement_Display->insertPlainText(QString::fromStdString(sget));
        ui->Statement_Display->append("");
    }
    if(p->isop()){
    PrintIftree(((CompoundExp*)p)->getlhs(),flag);
    PrintIftree(((CompoundExp*)p)->getrhs(),flag);
    }
}

void MainWindow::Ins_handle(string input)
{
    if(input=="RUN"){
        Run();
        return;
    }
    if(input=="CLEAR"){
        clear();
    }

}

void MainWindow::clear()
{
    ui->Code_Display->clear();
    ui->Result_Display->clear();
    ui->Statement_Display->clear();
    ui->lineEdit->clear();
    buffer.clear();
    run_state.clear();
    node_input_flag=false;
    if_run=false;
    if_throw=false;
    interaction="";
    valinput="";
    run_line=-1;
}





































