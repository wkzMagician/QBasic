#include "expression.h"

Expression::Expression()
{

}

Expression::~Expression()
{

}

ConstantExp::ConstantExp(int val) : value(val)
{
}

IdentifierExp::IdentifierExp(QString name) : name(name)
{
}

CompoundExp::CompoundExp(QString op, Expression *lhs, Expression *rhs) : op(op), lhs(lhs), rhs(rhs)
{
}

ConstantExp::~ConstantExp()
{
}

IdentifierExp::~IdentifierExp()
{
}

CompoundExp::~CompoundExp()
{
    delete lhs;
    delete rhs;
}

int ConstantExp::eval(EvaluationContext &context)
{
    return value;
}

QString ConstantExp::toString(int level, int letStmtCount, EvaluationContext *context)
{
    return QString::number(this->value);
}

int IdentifierExp::eval(EvaluationContext &context)
{
    if (!context.isDefined(name)) // 未定义的变量
        error(name.toLatin1() + " is undefined");
    // 变量被访问的次数加一
    context.addVarCount(name);
    return context.getValue(name);
}

QString IdentifierExp::toString(int level, int letStmtCount, EvaluationContext *context)
{
    if(letStmtCount == -2){//打印变量被使用的次数
        return name + " " + QString::number(context->getVarCount(name));
    } 
    return this->name;
}

int CompoundExp::eval(EvaluationContext &context)
{
    int right = rhs->eval(context);
    if (op == "=") // 赋值语句
    {
        context.setValue(lhs->getIdentifierName(), right);
        return right;
    }
    int left = lhs->eval(context);
    if (op == "+")
        return left + right;
    if (op == "-")
        return left - right;
    if (op == "*")
        return left * right;
    if (op == "/")
    {
        if (right == 0) // 除0错误
            error("Division by 0");
        return left / right;
    }
    if (op == "MOD")
    {
        if (right == 0) // 模0错误
            error("MOD by 0");
        // 特殊情况
        if (right == 1 || right == -1)
            return 0;
        if (left == 0)
            return 0;

        // 除数取绝对值
        int divisor = abs(right);
        int dividend = left;
        // 计算余数
        if (left > 0)
        {
            if (right > 0)
            {
                while (dividend >= divisor)
                {
                    dividend -= divisor;
               }
           }
           else{
               while(dividend > 0){
                    dividend -= divisor;
               }
           }
       }
       else{
           if(right > 0){
               while(dividend < 0){
                    dividend += divisor;
               }
           }
           else{
               while(dividend >= divisor){
                    dividend += divisor;
               }
           }
       }
       return dividend;

   }
   if (op == "**"){ // 幂运算
       if(right == 0) return 1;
       if(left == 0) return 0;

       int p = right;
       int base = left;
       while(p > 1){
           base *= left;
           --p;
       }
       return base;
   }
   error("Illegal operator in expression");
   return 0;
}

QString CompoundExp::toString(const int level, const int letStmtCount, EvaluationContext *context)
{
    QString space = "    ";
    int l = level;
    // 根据level计算缩进空格
    for(;l > 0; --l) space += "    ";
    if (op == "=" && letStmtCount > -1) // 赋值语句
    {
        if(letStmtCount == 0){
            return "= \n"
                + space + lhs->toString(level + 1) + "\n"
                + space + rhs->toString(level + 1);
        }
        else{
            return "= " + QString::number(letStmtCount) + "\n"
                + space + lhs->toString(level + 1, -2, context) + "\n"
                + space + rhs->toString(level + 1);
        }
    }
    else{
        return op + "\n"
            + space + lhs->toString(level + 1) + "\n"
            + space + rhs->toString(level + 1);
    }
}

ExpressionType ConstantExp::type()
{
    return CONSTANT;
}

ExpressionType IdentifierExp::type()
{
    return IDENTIFIER;
}

ExpressionType CompoundExp::type()
{
    return COMPOUND;
}

int ConstantExp::getConstantValue()
{
    return value;
}

QString IdentifierExp::getIdentifierName()
{
    return name;
}

QString CompoundExp::getOperator()
{
    return op;
}

Expression* CompoundExp::getLHS()
{
    return lhs;
}

Expression* CompoundExp::getRHS()
{
    return rhs;
}

int Expression::getConstantValue()
{
    return 0;
}

QString Expression::getIdentifierName()
{
    return QString();
}

QString Expression::getOperator()
{
    return QString();
}

Expression *Expression::getLHS()
{
    return nullptr;
}

Expression *Expression::getRHS()
{
    return nullptr;
}
