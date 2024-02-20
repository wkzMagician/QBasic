#include "evalstate.h"
#include <QString>

void EvaluationContext::setValue(QString var, int value)
{
    symbolTable.insert(var, value);
    if(!varCount.contains(var)){
        varCount.insert(var, 1);
    }
    else{
        varCount[var] += 1;
    }
}

int EvaluationContext::getValue(QString var)
{
    return symbolTable.value(var);
}

bool EvaluationContext::isDefined(QString var)
{
    return symbolTable.contains(var);
}

void EvaluationContext::clear()
{
    symbolTable.clear();
    varCount.clear();
}

void EvaluationContext::addVarCount(const QString &var)
{
    //增加变量被访问的次数
    varCount[var] += 1;
}

int EvaluationContext::getVarCount(const QString &var)
{
    return varCount[var];
}

void EvaluationContext::resetVarCount()
{
    //将次数全部清零
    for(auto it = varCount.begin(); it != varCount.end(); ++it){
        it.value() = 0;
    }
}

void Evalstate::init()
{
    fresh();
    curStmtPos = -1; // 从-1开始，因为第一次执行时会自动加1
    end = false;
}

void Evalstate::nextStmt()
{
    if(jump){
        //从行号查找对应的坐标
        curStmtPos = hash.value(nextLineNum, -1);
        if(curStmtPos == -1){//没找到
            throw("Line " + QString::number(nextLineNum) + " does not exist");
        }
    }
    else{
        ++curStmtPos;
    }
    //更新状态
    fresh();
}
