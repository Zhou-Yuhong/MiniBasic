#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "buffer.h"
#include <QMainWindow>
#include <QString>
#include <QPushButton>
#include <QKeyEvent>
#include <iostream>
#include <queue>
#include <QFile>
#include <QfileDialog>
#include <QtWidgets/QDialog>
#include <QTextBrowser>
#include <QColor>
#include <QTextBlock>
#define KEYBOARD_ENTER 16777220
//#define COLOR_GREEN(QColor(100,255,100));
//#define COLOR_RED(QColor(255,100,100));
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    Buffer buffer;
    EvalState run_state;
    bool if_run=false;//是否开始运行程序
    bool debug_button_flag=false;//区分进入调试模式以及单步执行
    void showcode();
    void show_runcode(string str);
    //显示当前变量
    void show_current_variables();
    void inputString(string str);
    void InputString(string str);
    int  step_run();//单步执行,返回0表示正常进行，返回-1表示结束
    void Step_Run();//单步执行的外裹函数
    void run();
    void Run();//run的外裹函数
    void load();//从文件读
    void HightLight(int line_num,QColor color);//高亮的外部函数
    void highLight(int Line, QColor color);//高亮的内部函数
    void help();
    string interaction="";
    string valinput="";
    //正在跑的行号
    int run_line=-1;
    bool if_throw=false;//处理莫名其妙代码乱跑的问题
    void printtree(Expression* p);
    void PrintIftree(Expression* p,bool flag=true);
    void Ins_handle(string input);
    void clear();
    node * ins_node=nullptr;   //用于处理INPUT，LET，PRINT，INPUTS,PRINTF指令的node*
    bool node_input_flag=false;  //上述node是否处于input等待输入的状态
private:
    Ui::MainWindow *ui;
    QDialog* dialog;//帮助的dialog
    QDialog* normal_dialog; //正常退出的弹框
    QDialog* wrong_dialog;   //错误退出的弹框
    QTextBrowser* helpinf;
    QTextBrowser* normalinf;
    QTextBrowser* wronginf;
    void keyPressEvent(QKeyEvent *event);

};

#endif // MAINWINDOW_H
