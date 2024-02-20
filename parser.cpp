#include "parser.h"
#include <QRegularExpression>

bool Parser::isValidVariableName(const QString &variableName)const {
    // 定义BASIC变量名的正则表达式
    QRegularExpression regex("^[a-zA-Z][a-zA-Z0-9_]{0,254}$");

    // 进行匹配
    QRegularExpressionMatch match = regex.match(variableName);

    // 返回匹配结果
    return match.hasMatch();
}

Parser::Parser()
{

}

Expression *Parser::parse(QVector<QString>& tokens)
{
    QString str;
    token lastOp;
    while(!tokens.empty()){
        // 读取一个token
        str = tokens.first();
        tokens.pop_front();

        switch (lastOp = getOp(str)) { // 获取token的类型
            case ASSIGN:
            //! 赋值符号前面必须有且只有一个变量名
                if(expStack.empty() || expStack.top()->type() != IDENTIFIER){
                    throw("Invalid Expression");
                }
                opStack.push(str);
                break;
            case ADD:
            case SUB:
            // 如果是加减号，那么前面直到开括号的所有运算可以执行
                while (!opStack.empty() && getOp(opStack.top()) >= ADD)
                {
                    createCompoundExp(opStack.pop());
                }
                opStack.push(str);
                break;
            // 如果是乘除号，那么前面高于加减号的所有运算可以执行
            case MULTI:
            case DIV:
            case MOD:
                while (!opStack.empty() && getOp(opStack.top()) >= MULTI)
                {
                    createCompoundExp(opStack.pop());
                }
                opStack.push(str);
                break;
            case EXP:
            // 如果是乘方，优先进栈
                opStack.push(str);
                break;
            case VALUE:
            // 如果是变量或者整数，那么生成一个常量表达式或者变量表达式
                createExp(str);
                break;
            case OPAREN:
            // 如果是开括号，那么直接进栈
                opStack.push(str);
                break;
            case CPAREN:
            // 开括号和闭括号之间的所有运算可以执行
                while(!opStack.empty() && getOp(opStack.top()) != OPAREN){
                    createCompoundExp(opStack.pop());
                }
                // 如果栈空了，那么说明没有开括号，那么就是不合法的表达式
                if(opStack.empty()){
                    throw("Invalid Expression");
                }
                // 如果不是空的，那么就是开括号，把开括号弹出
                opStack.pop();
                break;
            default:
                break; 
        }
    }

    // 如果操作符栈不空，那么就把栈里面的运算都执行
    while(!opStack.empty()){
        createCompoundExp(opStack.pop());
    }
    // 如过EXP栈空了，那么就是不合法的表达式
    if(expStack.empty()){
        throw("Invalid Expression");
    }

    exp = expStack.pop();   
    if(!expStack.empty()){ // 如果exp不是最后一个表达式，那么就是不合法的表达式
        throw("Invalid Expression");
    }

    return exp;
}

token Parser::getOp(QString str) const
{
    if(str == "+"){
        return ADD;
    }
    else if(str == "-"){
        return SUB;
    }
    else if(str == "*"){
        return MULTI;
    }
    else if(str == "/"){
        return DIV;
    }
    else if(str == "MOD"){
        return MOD;
    }
    else if(str == "**"){
        return EXP;
    }
    else if(str == "="){
        return ASSIGN;
    }
    else if(str == "("){
        return OPAREN;
    }
    else if(str == ")"){
        return CPAREN;
    }
    else if(!isValidVariableName(str)){
        // 不是变量，并且不是整数，就是不合法的
        bool ok;
        str.toInt(&ok);
        if(!ok) throw("Invalid Identifier");
    }
    else if(str == "LET" || str == "PRINT" || str == "INPUT" || str == "GOTO"
            || str == "IF" || str == "THEN" || str == "END" || str == "REM"){
        throw("Invalid Identifier");
    }
    // 其他情况下就是变量或者整数
    return VALUE;
}

void Parser::createExp(QString var)
{
    bool ok;
    int value = var.toInt(&ok); // 判断是否是整数
    if(ok){
        expStack.push(new ConstantExp(value));
    }
    else{
        expStack.push(new IdentifierExp(var));
    }
}

void Parser::createCompoundExp(QString op)
{
    Expression* rhs = expStack.pop();
    Expression* lhs = expStack.pop();
    expStack.push(new CompoundExp(op, lhs, rhs));
}
