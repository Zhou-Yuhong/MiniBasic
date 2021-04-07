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
#define KEYBOARD_ENTER 16777220
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
    void showcode();
    void show_runcode(string str);
    void inputString(string str);
    void InputString(string str);
    void run();
    void Run();//run的外裹函数
    void load();//从文件读
    void help();

    string interaction="";
    string valinput="";
    int run_line=-1;
    bool if_throw=false;//处理莫名其妙代码乱跑的问题
    void printtree(Expression* p);
    void PrintIftree(Expression* p,bool flag=true);
    void Ins_handle(string input);
    void clear();

private:
    Ui::MainWindow *ui;
    QDialog* dialog;
    QTextBrowser* helpinf;
    void keyPressEvent(QKeyEvent *event);

};

#endif // MAINWINDOW_H
