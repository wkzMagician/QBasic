#ifndef PROGRAM_H
#define PROGRAM_H

#include <QDebug>
#include <QString>
#include <QVector>
#include <QObject>
#include <QCoreApplication>
#include <QHash>
#include <QEventLoop>
#include "tokenizer.h"
#include "parser.h"
#include "statement.h"
#include "evalstate.h"

//程序主体类，存储命令以及运行程序
class Program : public QObject
{
    Q_OBJECT
public:
    QVector<Statement*> stmtList; // 存储命令

    Program();

    bool inputMode()const{return evalstate.isInput;} // 返回程序是否需要用户读取输入
    EvaluationContext* getContext(){return &context;}

    //handle函数对输入进行初步判断，如果不是以数字开头就直接执行，否则存储
    void handle(const QString& cmd);
    //清空所有存储的命令
    void clear();
    //运行程序
    void run();

private:
    Statement* newStmt; //构造Statement过程中的辅助变量

    EvaluationContext context; //程序上下文，存储变量键值对
    Evalstate evalstate; //程序运行时数据

    Tokenizer tokenizer; //将原本的一行文本拆成
    QVector<QString> tokens;

    void store(const int index); //存储命令，index是行号
    void parse(int index); //解析函数，根据command调用相应的Statement构造函数
    void execute();//直接执行不带行号的命令

    void handleError(const char* error);//将错误信息上报窗口显示
    void handleError(const QString& error);
signals:
    void updateCode();
    void updateResult(const QString& result);
    void updateTree(const bool show = false);
    void inputModeChanged();//通知窗口改变输入模式显示
public slots:
    void input(const QString& var);//初步处理Input命令，通知窗口
    void handleInput(const QString& cmd);//处理Input命令从用户读取的输入
    void print(const int num);//处理print语句
    void Goto(const int n);//处理goto语句
    void end();//处理end语句
};

#endif // PROGRAM_H
