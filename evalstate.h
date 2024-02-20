#ifndef EVALSTATE_H
#define EVALSTATE_H

#include <QString>
#include <QHash>

//存储定义的变量
class EvaluationContext
{
public:
   void setValue(QString var, int value);
   int getValue(QString var);
   bool isDefined(QString var);
   void clear();
    void addVarCount(const QString& var);
    int getVarCount(const QString& var);
    void resetVarCount();
private:
   QHash<QString, int> symbolTable;
   //用来存储变量被访问的次数
    QHash<QString, int> varCount;
};

//存储程序运行时的控制相关的数据
class Evalstate
{
public:
    bool jump = false;
    bool end = false;

    int curStmtPos; // 现在执行的语句在List中的位置
    int nextLineNum; // 下一条语句的行号

    bool exeFinished = false; // 语句执行是否完毕

    bool isInput = false; // 是否是输入语句
    QString inputVar;

    QHash<int, int> hash; // hash表，用来映射行号和语句在List中的位置

    // 程序运行完毕前的初始化
    void init();

    // 执行完一条语句后，执行下一条语句
    void nextStmt();
private:
    // 该函数用来刷新jump和exeFinished
    void fresh(){
        jump = false;
        exeFinished = false;
    }
};

#endif // EVALSTATE_H
