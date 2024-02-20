#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <cmath>

#include "evalstate.h"

enum ExpressionType { CONSTANT, IDENTIFIER, COMPOUND };

//存储表达式值的类
class Expression {

public:

   Expression();
   virtual ~Expression();
   virtual int eval(EvaluationContext & context) = 0;

   //下面的函数用于显示表达式树,level表示缩进层数，letStmtCount表示let语句执行的次数，context表示上下文，用于统计变量被use的次数
   //letStmtCount >=0, 表示let语句执行了x次
   //letStmtCount = -1, 表示不是let语句，正常输出
   //letStmtCount = -2, 表示位于let表达式树的第二行，要打印出变量被使用的次数
   virtual QString toString(int level = 0, int letStmtCount = -1, EvaluationContext* context = nullptr) = 0;
   virtual ExpressionType type() = 0;

/* Getter methods for convenience */

   virtual int getConstantValue();
   virtual QString getIdentifierName();
   virtual QString getOperator();
   virtual Expression *getLHS();
   virtual Expression *getRHS();

   void error(const char* e){
       throw e;
   }

};


class ConstantExp: public Expression {

public:

   ConstantExp(int val);
   virtual ~ConstantExp();

   virtual int eval(EvaluationContext & context);
   virtual QString toString(int level = 0, int letStmtCount = -1, EvaluationContext* context = nullptr);
   virtual ExpressionType type();

   virtual int getConstantValue();

private:

   int value;

};


class IdentifierExp: public Expression {

public:

   IdentifierExp(QString name);
   virtual ~IdentifierExp();

   virtual int eval(EvaluationContext & context);
   virtual QString toString(int level = 0, int letStmtCount = -1, EvaluationContext* context = nullptr);
   virtual ExpressionType type();

   virtual QString getIdentifierName();

private:

   QString name;

};


class CompoundExp: public Expression {

public:

   CompoundExp(QString op, Expression *lhs, Expression *rhs);
   virtual ~CompoundExp();

   virtual int eval(EvaluationContext & context);
   virtual QString toString(const int level = 0, const int letStmtCount = -1, EvaluationContext* context = nullptr);
   virtual ExpressionType type();

   virtual QString getOperator();
   virtual Expression *getLHS();
   virtual Expression *getRHS();

private:

   QString op;
   Expression *lhs, *rhs;

};



#endif // EXPRESSION_H
