#ifndef STATEMENT_H
#define STATEMENT_H

#include <QObject>
#include <QVector>
#include "expression.h"
#include "parser.h"
#include "evalstate.h"

//命令语句类
class Statement : public QObject
{
    Q_OBJECT
public:
    Parser parser;

    Statement(int index, QVector<QString>& tokens);//有行号
    Statement(QVector<QString>& tokens);//无行号
    virtual ~Statement() = 0;

    QString toString()const{return str;};//返回语句的字符形式，用来显示code
    //show:是否显示语句执行了几次；context:上下文，用于标记变量被use的次数；
    virtual QString toTree(bool show = false, EvaluationContext *context = nullptr) = 0;//返回语句的树形式，用来显示表达式树
    virtual bool execute(EvaluationContext &context) = 0;//语句的执行

    virtual QString getVar()const;//返回变量名，特别用于input
    virtual int getGotoIndex()const;//返回goto的行号

    int getCount()const{return count;}//返回运行了几次
    void addCount(){count++;}//运行次数加一
    virtual void resetCount(){count = 0;}//运行次数清零
    int Index()const{return index;}//返回行号

private:
    int index;
    QString str;
    int count = 0;

Q_SIGNALS:
signals:
    void print(const int);
    void input(const QString&);
    void Goto(const int);
    void end();
};


class RemStmt : public Statement
{
public:
    RemStmt(int index, QVector<QString>& tokens);
    ~RemStmt();

    virtual QString toTree(bool show = false, EvaluationContext *context = nullptr) override;
    virtual bool execute(EvaluationContext &context) override;
private:
    QString remark;
};

class LetStmt : public Statement
{
public:
    LetStmt(int index, QVector<QString>& tokens);
    LetStmt(QVector<QString>& tokens);
    ~LetStmt();

    virtual QString toTree(bool show = false, EvaluationContext *context = nullptr) override;
    virtual bool execute(EvaluationContext &context) override;
private:
    Expression* let;
};

class PrintStmt : public Statement
{
public:
    PrintStmt(int index, QVector<QString>& tokens);
    PrintStmt(QVector<QString>& tokens);
    ~PrintStmt();    
    
    virtual QString toTree(bool show = false, EvaluationContext *context = nullptr) override;
    virtual bool execute(EvaluationContext &context) override;
private:
    Expression* num;
};

class InputStmt : public Statement
{
public:
    InputStmt(int index, QVector<QString>& tokens);
    InputStmt(QVector<QString>& tokens);
    ~InputStmt();

    virtual QString toTree(bool show = false, EvaluationContext *context = nullptr) override;
    virtual bool execute(EvaluationContext &context) override;

    virtual QString getVar()const override{return var;}
private:
    QString var;
};

class GotoStmt : public Statement
{
public:
    GotoStmt(int index, QVector<QString>& tokens);
    ~GotoStmt();

    virtual QString toTree(bool show = false, EvaluationContext *context = nullptr) override;
    virtual bool execute(EvaluationContext &context) override;
    virtual int getGotoIndex() const override{return gotoIndex;};
private:
    int gotoIndex;
};

class IfStmt : public Statement
{
public:
    IfStmt(int index, QVector<QString>& tokens);
    ~IfStmt();

    virtual QString toTree(bool show = false, EvaluationContext *context = nullptr) override;
    virtual bool execute(EvaluationContext &context) override;
    virtual int getGotoIndex() const override{return gotoIndex;};

    virtual void resetCount() override{countTrue = 0; countFalse = 0;}
private:
    int gotoIndex;
    Expression* lhs;
    Expression* rhs;
    char op;

    //if语句与其它语句不同，存储两个分支各运行了几次
    int countTrue = 0;
    int countFalse = 0;
};

class EndStmt : public Statement
{
public:
    EndStmt(int index, QVector<QString>& tokens);
    ~EndStmt();

    virtual QString toTree(bool show = false, EvaluationContext *context = nullptr) override;
    virtual bool execute(EvaluationContext &context) override;
};

#endif // STATEMENT_H
