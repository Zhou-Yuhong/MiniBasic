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

    wrong_dialog=new QDialog(this);
    wrong_dialog->setMinimumSize(400,400);
    wrong_dialog->setWindowTitle("ERROR");
    wronginf=new QTextBrowser(wrong_dialog);
    wronginf->setMinimumSize(400,400);
    wrong_dialog->hide();
    QString wrong="There is an error in this statement";
    wronginf->insertPlainText(wrong);

    normal_dialog=new QDialog(this);
    normal_dialog->setMinimumSize(400,400);
    normal_dialog->setWindowTitle("NORMOL EXIT");
    normalinf=new QTextBrowser(normal_dialog);
    normalinf->setMinimumSize(400,400);
    normal_dialog->hide();
    QString normal="The debugged program ends normally";
    normalinf->insertPlainText(normal);

    connect(ui->Button_Run,&QPushButton::clicked,[=](){
        Run();
    });

    connect(ui->Button_Load,&QPushButton::clicked,[=](){
        load();
    });
    connect(ui->Button_Clear,&QPushButton::clicked,[=](){
    clear();
    });
    connect(ui->debug_button,&QPushButton::clicked,[=](){
        //进入单步模式
        if(!debug_button_flag){
            //设置load和Clear禁用
            this->ui->Button_Load->setEnabled(false);
            this->ui->Button_Clear->setEnabled(false);
            if_run=true;
            debug_button_flag=true;
            //设置要开始运行的那行代码变绿色
            if(this->buffer.head->next!=nullptr){
               this->Green_Line=this->transfer_linenum(this->buffer.head->next->line_number);
            }
            //过一遍语法树，标记所有出错的代码为红色
            syntax_all();
            this->highLight();
            this->ui->Statement_Display->clear();
        }
        else{
            this->Step_Run();
        }
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
            if(!this->debug_button_flag)Run();
            else{
                Step_Run();
            }
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
//void MainWindow::show_current_variables(){
//    //先清空上次的残留
//    this->ui->Variable_Display->clear();

//   map<string,ValueUnit> valmap=this->run_state.getmap();
//   for(int i=0;i<valmap.size();i++){
//       string str=valmap[i].first+": ";
//       if(valmap[i].second.type){
//           str+="INT = ";
//           str+=valmap[i].second.tostring();
//       }
//       else{
//           str+="STR = ";
//           str+=valmap[i].second.tostring();
//       }
//        ui->Variable_Display->insertPlainText(QString::fromStdString(str));
//        ui->Variable_Display->append("");
//   }
//}

void MainWindow::show_runcode(string str)
{
    ui->Result_Display->insertPlainText(QString::fromStdString(str));
    ui->Result_Display->append("");
    interaction="";   //重置
}

void MainWindow::show_current_variables()
{
    this->ui->Variable_Display->clear();
      vector<string> tmp=this->run_state.get_all_val();
      for(int i=0;i<tmp.size();i++){

          ui->Variable_Display->insertPlainText(QString::fromStdString(tmp[i]));
          ui->Variable_Display->append("");
      }
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
//单步调试的外裹函数
void MainWindow::Step_Run(){
    if(!if_run) return;
    int judge;
    try{
        judge=this->step_run();
        //返回值为-1代表结束
        if(judge==-1) {
            if_run=false;
            this->debug_button_flag=false;
            this->ui->Button_Clear->setEnabled(true);
            this->ui->Button_Load->setEnabled(true);
            this->normal_dialog->show();
            return;
        }
        else{
            //显示当前变量以及值
            this->show_current_variables();
            //高亮下一次要运行的代码
            this->Green_Line=this->transfer_linenum(run_line);
            this->highLight();
            return;
        }
    }
    catch(myException e){
        if(!if_throw){
            string err="Line "+to_string(run_line)+" "+e.report();
            ui->Result_Display->insertPlainText(QString::fromStdString(err));
            ui->Statement_Display->append("");
            if_throw=true;
            this->ui->Button_Clear->setEnabled(true);
            this->ui->Button_Load->setEnabled(true);
            //出错显示窗口
            this->wrong_dialog->show();
            return;
        }
    }

}
//单步调式的语法树只有跑的那行，所以要先clear
int MainWindow::step_run(){
    static bool flag=true;//控制input只进行一次
    node *p;
    string err;
    if(run_line==-1){
        p=buffer.head->next;
    }
    else{
        p=buffer.gotoline(run_line);
    }
    if(p==nullptr) return -1;
    //跳过空行
    while(p!=nullptr&&p->content.empty()){
        p=p->next;

    }
     run_line=p->line_number;
    if(p==nullptr) return -1;
    string state_type = p->content[0];
    if(state_type!="REM"&&state_type!="LET"&&state_type!="IF"&&state_type!="PRINT"&&state_type!="END"&&state_type!="INPUT"
            &&state_type!="GOTO"&&state_type!="INPUTS"&&state_type!="PRINTF"){
        err="Error: Unknown state "+state_type;
        throw myException(err);
    }
        p->set_status();
    if (state_type == "REM") {
        //语法树，先clear
        ui->Statement_Display->clear();
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
        if(p==nullptr) return -1;
        else{
            run_line=p->line_number;
            return 0;
        }
    }
    if (state_type == "LET"){
        if(p->expr==nullptr){
            err="Error: Wrong Expression";
            throw myException(err);
        }
        p->state->execute(run_state);
        //语法树,先clear
        ui->Statement_Display->clear();
        ui->Statement_Display->insertPlainText(QString::number(p->line_number));
        ui->Statement_Display->insertPlainText(QString::fromStdString(" "+state_type+" "));
        printtree(p->expr);
        //语法树结束
        p = p->next;
        if(p==nullptr){
            return -1;
        }
        else{
            run_line=p->line_number;
            return 0;
        }
    }
    if (state_type == "IF") {
        int linenum = ((IF_statement*)p->state)->get_line(run_state);
        if(linenum!=0&&buffer.gotoline(linenum)==nullptr){
            err="Syntax error: Line number "+std::to_string(linenum)+" does not exist";
            throw myException(err);
        }
        //语法树,先clear
        ui->Statement_Display->clear();
        ui->Statement_Display->insertPlainText(QString::number(p->line_number));
        ui->Statement_Display->insertPlainText(QString::fromStdString(" "+state_type+" "));
        PrintIftree(p->expr);
        //语法树结束

        if (linenum == 0) {
            p = p->next;
            if(p==nullptr){
                return -1;
            }
            else{
               run_line=p->line_number;
               return 0;
            }
        }
        else {
            run_line=linenum;
            return 0;
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
        //语法树,先clear
        ui->Statement_Display->clear();
        ui->Statement_Display->insertPlainText(QString::number(p->line_number));
        ui->Statement_Display->insertPlainText(QString::fromStdString(" "+state_type+" "));
        printtree(p->expr);
        //语法树结束
        p = p->next;
        if(p==nullptr){
            return -1;
        }
        else{
            run_line=p->line_number;
            return 0;
        }
    }
    if(state_type=="PRINTF"){
        p->state->execute(run_state);
        interaction=((PRINTF_statement*)p->state)->get_final_string();
        show_runcode(interaction);
        //语法树,先clear
        ui->Statement_Display->clear();
        ui->Statement_Display->insertPlainText(QString::number(p->line_number));
        ui->Statement_Display->insertPlainText(QString::fromStdString(" "+state_type+" "));
        ui->Statement_Display->insertPlainText(QString::fromStdString(interaction));
          //
        p=p->next;
        if(p==nullptr){
            return -1;
        }
        else{
            run_line=p->line_number;
            return 0;
        }
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
        //语法树,先clear
        ui->Statement_Display->clear();
        ui->Statement_Display->insertPlainText(QString::number(q->line_number));
        ui->Statement_Display->insertPlainText(QString::fromStdString(" "+state_type+" "));
        printtree(q->expr);
        //语法树结束
        run_line=linenum;
        return 0;
    }
    if (state_type == "INPUT") {
        if(valinput==""){               //未得到输入，退出该函数
            if(!flag) return 0;
            if(p->expr->getType()!=IDENTIFIER){
                err="Error: Input should be followed by a variable";
                throw myException(err);
            }
            interaction= ((INPUT_statement*)p->state)->getname()+"?";
            show_runcode(interaction);
            //语法树,先清空
            ui->Statement_Display->clear();
            ui->Statement_Display->insertPlainText(QString::number(p->line_number));
            ui->Statement_Display->insertPlainText(QString::fromStdString(" "+state_type+" "));
            ui->Statement_Display->insertPlainText(QString::fromStdString(((INPUT_statement*)p->state)->getname()));
//            //语法树结束
            flag=false;
            return 0;
        }
        ((INPUT_statement*)p->state)->get_value(valinput);
        //execute把变量和值加入到run_state中
        ((INPUT_statement*)p->state)->execute(run_state);
        valinput="";
        flag=true;
        p = p->next;
        if(p==nullptr){
            return -1;
        }
        else{
            run_line=p->line_number;
            return 0;
        }
    }
    if(state_type=="INPUTS"){
        if(valinput==""){
            //未得到输入，退出该函数
            if(!flag) return 0;    //flag防止跑两次
            if(p->expr->getType()!=IDENTIFIER){
                err="Error: Input should be followed by a variable";
                throw myException(err);
            }
            interaction=((INPUTS_statement *)p->state)->getname()+"?";
            show_runcode(interaction);
//            //语法树,先清空
            ui->Statement_Display->clear();
            ui->Statement_Display->insertPlainText(QString::number(p->line_number));
            ui->Statement_Display->insertPlainText(QString::fromStdString(" "+state_type+" "));
            ui->Statement_Display->insertPlainText(QString::fromStdString(((INPUT_statement*)p->state)->getname()));
//            //语法树结束
            flag=false;
            return 0;
        }
        //把得到的输入加入到run_state中
        ((INPUTS_statement*)p->state)->get_value(valinput);
        ((INPUTS_statement*)p->state)->execute(run_state);
        //重置，为下一次INPUTS做准备
        valinput="";
        flag=false;
        p=p->next;
        if(p==nullptr) return -1;
        else{
            run_line=p->line_number;
            return 0;
        }
    }

    if (state_type == "END") {
        return -1;
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
               p->set_status();          
           if (state_type == "REM") {     
               p = p->next;
               continue;
           }
           if (state_type == "LET") {
               if(p->expr==nullptr){
                   err="Error: Wrong Expression";
                   throw myException(err);
               }
               p->state->execute(run_state);
               p = p->next;      
               continue;
           }
           if (state_type == "IF") {
               int linenum = ((IF_statement*)p->state)->get_line(run_state);
               if(linenum!=0&&buffer.gotoline(linenum)==nullptr){
                   err="Syntax error: Line number "+std::to_string(linenum)+" does not exist";
                   throw myException(err);
               }
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
               p = p->next;

               continue;
           }
           if(state_type=="PRINTF"){
               p->state->execute(run_state);
               interaction=((PRINTF_statement*)p->state)->get_final_string();
               show_runcode(interaction);
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
               flag=true;
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
{
    //解除限制
    this->ui->Button_Clear->setEnabled(true);
    this->ui->Button_Load->setEnabled(true);
    //先生成语法树,仅一次
    this->syntax_all();
    //根据语法树进行高亮
    this->Green_Line=-1;
    this->highLight();
    //没错才进行下一步
    if(!this->error_line.empty()) return;
    this->if_run=true;
    try{
        run();
    }
    catch(myException e){
        if(!if_throw){
        string err="Line "+to_string(run_line)+" "+e.report();
        //HightLight(run_line,QColor(255,100,100));
        ui->Result_Display->insertPlainText(QString::fromStdString(err));
        ui->Result_Display->append("");
        if_throw=true;
        return;
        }

    }
}
//载入文件并显示
void MainWindow::load()
{
    this->clear();
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
    ui->Statement_Display->clear();
    ui->Result_Display->clear();
    ui->lineEdit->clear();
}

void MainWindow::help()
{
    this->dialog->show();
}
void MainWindow::syntax_all(){
    if(this->print_tree_once==false) return;
    node *p=this->buffer.head->next;
    while(p!=nullptr){
        try{
            this->syntaxtree(p);
            p=p->next;
            continue;
        }
        catch(myException e){
            this->error_line.push_back(this->transfer_linenum(p->line_number));
            p=p->next;
            continue;
        }
    }
    this->print_tree_once=false;
}

//由一个node*打印语法树
void MainWindow::syntaxtree(node *p){
    string state_type=p->content[0];
    if(state_type!="REM"&&state_type!="LET"&&state_type!="IF"&&state_type!="PRINT"&&state_type!="END"&&state_type!="INPUT"
            &&state_type!="GOTO"&&state_type!="INPUTS"&&state_type!="PRINTF"){
        string err="Error: Unknown state "+state_type;
        throw myException(err);
    }
    if(state_type=="REM"){
        //语法树
        p->initial_exp();
        ui->Statement_Display->insertPlainText(QString::number(p->line_number));
        ui->Statement_Display->insertPlainText(QString::fromStdString(" "+state_type+" "));
        ui->Statement_Display->append("");
        string str;
        for(int i=1;i<p->content.size();i++){
            str+=p->content[i]+" ";
        }
        ui->Statement_Display->insertPlainText(QString::fromStdString(str));
        ui->Statement_Display->append("");
        return;
        //语法树
    }
    if(state_type=="LET"){
        p->initial_exp();
        //语法树
        ui->Statement_Display->insertPlainText(QString::number(p->line_number));
        ui->Statement_Display->insertPlainText(QString::fromStdString(" "+state_type+" "));
        printtree(p->expr);
        //语法树结束
        return;
    }
    if(state_type=="IF"){
        p->initial_exp();
        //语法树
        ui->Statement_Display->insertPlainText(QString::number(p->line_number));
        ui->Statement_Display->insertPlainText(QString::fromStdString(" "+state_type+" "));
        PrintIftree(p->expr);
        //语法树结束
        return;
    }
    if(state_type=="PRINT"){
        p->initial_exp();
        //语法树
        ui->Statement_Display->insertPlainText(QString::number(p->line_number));
        ui->Statement_Display->insertPlainText(QString::fromStdString(" "+state_type+" "));
        printtree(p->expr);
        //语法树结束
        return;
    }
    if(state_type=="PRINTF"){
        //不需要初始化exp
        ui->Statement_Display->insertPlainText(QString::number(p->line_number));
        ui->Statement_Display->insertPlainText(QString::fromStdString(" "+state_type+" "));
        string tmp="";
        for(int i=1;i<p->content.size();i++){
           tmp+=p->content[i]+" ";
        }
         ui->Statement_Display->insertPlainText(QString::fromStdString(tmp));
         ui->Statement_Display->append("");
        return;
    }
    if(state_type=="GOTO"){
        p->initial_exp();
        //语法树
        ui->Statement_Display->insertPlainText(QString::number(p->line_number));
        ui->Statement_Display->insertPlainText(QString::fromStdString(" "+state_type+" "));
        printtree(p->expr);
        //语法树结束
        return;
    }
    if(state_type=="INPUT"){
        p->initial_exp();
        //语法树,直接原样打印
        ui->Statement_Display->insertPlainText(QString::number(p->line_number));
        ui->Statement_Display->insertPlainText(QString::fromStdString(" "+state_type+" "));
        string tmp="";
        for(int i=1;i<p->content.size();i++){
            tmp+=p->content[i]+" ";
        }
        ui->Statement_Display->insertPlainText(QString::fromStdString(tmp));
        ui->Statement_Display->append("");
        //语法树结束
        return;
    }
    if(state_type=="INPUTS"){
        p->initial_exp();
        //语法树，直接原样打印
        ui->Statement_Display->insertPlainText(QString::number(p->line_number));
        ui->Statement_Display->insertPlainText(QString::fromStdString(" "+state_type+" "));
        string tmp="";
        for(int i=1;i<p->content.size();i++){
            tmp+=p->content[i]+" ";
        }
        ui->Statement_Display->insertPlainText(QString::fromStdString(tmp));
        ui->Statement_Display->append("");
        //语法树结束
    }
    if(state_type=="END"){
        return;
     }
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
    ui->Variable_Display->clear();
    this->error_line.clear();
    this->Green_Line=-1;
    buffer.clear();
    run_state.clear();
    node_input_flag=false;
    if_run=false;
    if_throw=false;
    debug_button_flag=false;
    this->print_tree_once=true;
    interaction="";
    valinput="";
    run_line=-1;
}
int MainWindow::transfer_linenum(int linenum){
    int line=0;
    node *p=buffer.head->next;
    while(p!=nullptr&&p->line_number!=linenum){
        p=p->next;
        line++;
    }
    if(p==nullptr) return -1;
    else return line;
}

//void MainWindow::HightLight(int line_num, QColor color){
//    //先把line_num转成在Display中的行号
//    int line=transfer_linenum(line_num);
//    if(line==-1) return;
//    highLight(line,color);
//}
//这里的line是第几行，从0开始
//void MainWindow::highLight(int Line, QColor color)
//{       //如果Line是-1直接返回
//        if(Line==-1) return;
//        QList<QTextEdit::ExtraSelection> extras;
//        QTextEdit::ExtraSelection h;
//        QTextBlock line = ui->Code_Display->document()->findBlockByLineNumber(Line);
//        QTextCursor cursor(line);
//        h.cursor = cursor;
//        h.format.setProperty(QTextFormat::FullWidthSelection, true);
//        h.format.setBackground(color);
//        extras.append(h);
//        ui->Code_Display->setExtraSelections(extras);
//    QTextCursor cursor(line);
//    h.cursor = cursor;
//    h.format.setProperty(QTextFormat::FullWidthSelection, true);
//    h.format.setBackground(color);
//    extras.append(h);
//    ui->Code_Display->setExtraSelections(extras);
//    QList<QTextEdit::ExtraSelection> extraSelections;//提供一种方式显示选择的文本
//    extraSelections = ui->plainTextEdit->extraSelections();//获取之前高亮的设置
//    QTextEdit::ExtraSelection selection;
//    selection.format.setBackground(color);
//    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
//    selection.cursor = ui->plainTextEdit->textCursor();
//    selection.cursor.movePosition(QTextCursor::Up);//光标移动到某一行。此处移动到上一行，上一行将高亮。
//    extraSelections.append(selection);
//    ui->plainTextEdit->setExtraSelections(extraSelections);//设置高亮

//    ui->plainTextEdit->setExtraSelections(extraSelections);//设置高亮
//     QList<QTextEdit::extraSelections>
//}
void MainWindow::highLight(){
    QList<QTextEdit::ExtraSelection> extras;
    for(int i=0;i<this->error_line.size();i++){
        QTextEdit::ExtraSelection h;
        QTextBlock line = ui->Code_Display->document()->findBlockByLineNumber(this->error_line[i]);
        QTextCursor cursor(line);
        h.cursor = cursor;
        h.format.setProperty(QTextFormat::FullWidthSelection, true);
        h.format.setBackground(QColor(255,100,100));
        extras.append(h);
    }
    if(Green_Line>=0){
    QTextEdit::ExtraSelection h;
    QTextBlock line = ui->Code_Display->document()->findBlockByLineNumber(this->Green_Line);
    QTextCursor cursor(line);
    h.cursor = cursor;
    h.format.setProperty(QTextFormat::FullWidthSelection, true);
    h.format.setBackground(QColor(100,255,100));
    extras.append(h);}
    ui->Code_Display->setExtraSelections(extras);
}


































