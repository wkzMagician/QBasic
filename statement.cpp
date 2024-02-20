#include "statement.h"
#include <QTimer>
#include <QDebug>

Statement::Statement(int index, QVector<QString>& tokens) : index(index)
{
    // 把tokens中的内容拼接成str
    str += QString::number(Index()) + " ";
    for(const QString &s : tokens){
        str += s;
        if(s != tokens.last()){
            str += " ";
        }
    }
    tokens.pop_front();
    count = 0;
}

Statement::Statement(QVector<QString>& tokens)
{
    // 把tokens中的内容拼接成str
    for(const QString &s : tokens){
        str += s;
        if(s != tokens.last()){
            str += " ";
        }
    }
    tokens.pop_front();
    count = 0;
}

Statement::~Statement()
{
    
}

QString Statement::getVar() const
{
    return QString();
}

int Statement::getGotoIndex() const
{
    return 0;
}

RemStmt::RemStmt(int index, QVector<QString>& tokens) : Statement(index, tokens)
{
    // 把tokens中的内容拼接成remark
    for(const QString &s : tokens){
        remark += s;
        if(s != tokens.last()){
            remark += " ";
        }
    }
}

RemStmt::~RemStmt()
{
    
}

QString RemStmt::toTree(bool show, EvaluationContext *context)
{
    QString count = !show ? "" : QString::number(getCount());
    return QString::number(Index()) + " REM " + count + "\n    " + remark;
}

bool RemStmt::execute(EvaluationContext &context)
{
    //增加运行次数
    addCount();
    return true;
}

LetStmt::LetStmt(int index, QVector<QString>& tokens) : Statement(index, tokens)
{
    let = parser.parse(tokens);
}

LetStmt::LetStmt(QVector<QString>& tokens) : Statement(tokens)
{
    let = parser.parse(tokens);
}

LetStmt::~LetStmt()
{
    delete let;
}

QString LetStmt::toTree(bool show, EvaluationContext *context)
{
    return QString::number(Index()) + " LET " + let->toString(0, getCount(), context);
}

bool LetStmt::execute(EvaluationContext &context)
{
    let->eval(context);
    addCount();
    return true;
}

PrintStmt::PrintStmt(int index, QVector<QString>& tokens) : Statement(index, tokens)
{
    num = parser.parse(tokens);
}

PrintStmt::PrintStmt(QVector<QString>& tokens) : Statement(tokens)
{
    num = parser.parse(tokens);
}

PrintStmt::~PrintStmt()
{
    delete num;
}

QString PrintStmt::toTree(bool show, EvaluationContext *context)
{
    QString count = !show ? "" : QString::number(getCount());
    return QString::number(Index()) + " PRINT " + count + "\n"
            + "    " + num->toString(1);
}

bool PrintStmt::execute(EvaluationContext &context)
{
    emit print(num->eval(context));
    addCount();
    return true;
}

InputStmt::InputStmt(int index, QVector<QString>& tokens) : Statement(index, tokens)
{
    if(tokens.size() != 1){
        throw("Invalid Input");
    }
    var = tokens[0]; // 变量名
    //判断变量名是否合法，比如变量名必须以字母开头等等
    if(!parser.isValidVariableName(var)){
        throw("Invalid Identifier");
    }
}

InputStmt::InputStmt(QVector<QString>& tokens) : Statement(tokens)
{
    if(tokens.size() != 1){
        throw("Invalid Input");
    }
    var = tokens[0];
    //判断变量名是否合法，比如变量名必须以字母开头等等
    if(!parser.isValidVariableName(var)){
        throw("Invalid Identifier");
    }
}

InputStmt::~InputStmt()
{
    
}

QString InputStmt::toTree(bool show, EvaluationContext *context)
{
    QString count = !show ? "" : QString::number(getCount());
    return QString::number(Index()) + " INPUT " + count + "\n"
            + "    " + var;
}

bool InputStmt::execute(EvaluationContext &context)
{
    emit input(var);
    addCount();
    context.addVarCount(var);
    return false;
}

GotoStmt::GotoStmt(int index, QVector<QString>& tokens) : Statement(index, tokens)
{
    if (tokens.size() != 1)
    {
        throw("Invalid Input");
    }
    bool ok;
    int num = tokens[0].toInt(&ok);
    if(!ok){
        throw("Invalid Input");
    }
    gotoIndex = num; // 跳转的行号
}

GotoStmt::~GotoStmt()
{
    
}

QString GotoStmt::toTree(bool show, EvaluationContext *context)
{
    QString count = !show ? "" : QString::number(getCount());
    return QString::number(Index()) + " GOTO " + count + "\n"
            + "    " + QString::number(gotoIndex);
}

bool GotoStmt::execute(EvaluationContext &context)
{
    emit Goto(gotoIndex);
    addCount();
    return true;
}

IfStmt::IfStmt(int index, QVector<QString>& tokens) : Statement(index, tokens)
{
    //找<>=
    int opIndex = -1;
    for(opIndex = 0; opIndex < tokens.size(); opIndex++){
        if(tokens[opIndex] == "<" || tokens[opIndex] == "=" || tokens[opIndex] == ">"){
            op = tokens[opIndex][0].toLatin1();
            break;
        }
    }
    if(opIndex == tokens.size()){ // 没有找到
        throw("Invalid Input");
    }
    //找THEN
    int thenIndex = -1;
    for(thenIndex = 0; thenIndex < tokens.size(); thenIndex++){
        if(tokens[thenIndex] == "THEN"){
            break;
        }
    }
    if(thenIndex == tokens.size()){ // 没有找到
        throw("Invalid Input");
    }
    //必须有行号
    if(thenIndex != tokens.size() - 2){
        throw("Invalid Input");
    }
    //解析表达式
    //取子串：[0, opIndex) [opIndex + 1, thenIndex)
    QVector<QString> lhsTokens;
    for(int i = 0; i < opIndex; i++){
        lhsTokens.push_back(tokens[i]);
    }
    QVector<QString> rhsTokens;
    for(int i = opIndex + 1; i < thenIndex; i++){
        rhsTokens.push_back(tokens[i]);
    }
    //解析
    lhs = parser.parse(lhsTokens);
    rhs = parser.parse(rhsTokens);
    //解析行号
    bool ok;
    int num = tokens[thenIndex + 1].toInt(&ok);
    if(!ok){
        throw("Invalid Input");
    }
    gotoIndex = num;
}

IfStmt::~IfStmt()
{
    delete lhs;
    delete rhs;
}

QString IfStmt::toTree(bool show, EvaluationContext *context)
{
    QString numTrue = !show ? "" : QString::number(countTrue);
    QString numFalse = !show ? "" : QString::number(countFalse);
    return QString::number(Index()) + " IF THEN " + numTrue + ' ' + numFalse + "\n"
            + "    " + lhs->toString(1) + "\n"
            + "    " + op + "\n"
            + "    " + rhs->toString(1) + "\n"
            + "    " + QString::number(gotoIndex);
}

bool IfStmt::execute(EvaluationContext &context)
{
    int result = lhs->eval(context) - rhs->eval(context); // 计算表达式的值
    // 根据op判断是否跳转
    switch (op) {
    case '<':
        if(result < 0){
            emit Goto(gotoIndex);
            countTrue++;
        }
        else countFalse++;
        break;
    case '=':
        if(result == 0){
            emit Goto(gotoIndex);
            countTrue++;
        }
        else countFalse++;
        break;
    case '>':
        if(result > 0){
            emit Goto(gotoIndex);
            countTrue++;
        }
        else countFalse++;
        break;
    default:
        break;
    }
    return true;
}

EndStmt::EndStmt(int index, QVector<QString>& tokens) : Statement(index, tokens)
{
    if(!tokens.empty()){
        throw("Invalid Input");
    }
}

EndStmt::~EndStmt()
{
    
}

QString EndStmt::toTree(bool show, EvaluationContext *context)
{
    QString count = !show ? "" : QString::number(getCount());
    return QString::number(Index()) + " END " + count;
}

bool EndStmt::execute(EvaluationContext &context)
{
    emit end();
    addCount();
    return true;
}
