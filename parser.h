#ifndef PARSER_H
#define PARSER_H

#include <QVector>
#include <QString>
#include <QStack>

#include <expression.h>

//定义符号的类型
enum token{OPAREN, ASSIGN, ADD, SUB, MULTI, DIV, MOD, EXP, CPAREN, VALUE};

//解析器类
class Parser
{
public:
    Parser();
    //parse tokens, 返回构造好的Expression
    Expression* parse(QVector<QString>& tokens);
    //判断变量名是否合法，比如变量名必须以字母开头等等
    bool isValidVariableName(const QString &variableName)const;
private:
    //辅助用的操作符栈，表达式栈
    QStack<QString> opStack;
    QStack<Expression*> expStack;
    Expression* exp;

    token getOp(QString str)const; // 判断string是哪一种操作符/数
    void createExp(QString var); // 根据var是否是整数创建Constant/Indentifier exp
    void createCompoundExp(QString op);// 根据op类型创建CompoundExp
};

#endif // PARSER_H
