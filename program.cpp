#include "program.h"

Program::Program()
{

}


void Program::handle(const QString &stmt)
{
    newStmt = nullptr;

    tokens = tokenizer.split(stmt);

    if(tokens.empty()) return;

    bool ok;
    int index = tokens[0].toInt(&ok);

    if(!ok){ //直接执行
        try
        {
            execute();
        }
        catch(const char* error)
        {
            handleError(error);
        }
    }
    else{ // 有行号，存储
        tokens.pop_front(); // 去掉行号
        try{
            parse(index);
            store(index);
        }
        catch(const char* error){
            handleError(error);
        }
    }
}

void Program::parse(int index)
{
    //只输入行号，则补充""
    if(tokens.empty()) tokens.append("");

    QString token = tokens[0]; // 获取第一个token
    // 判断Statement类型
    if(token == "REM"){
        try {
            newStmt = new RemStmt(index, tokens); // 利用语句构造Statement
        } catch (const char* error) {
            delete newStmt; // 构造失败，删除newStmt
            throw(error);
        }
    }
    else if(token == "LET"){
        try {
            newStmt = new LetStmt(index, tokens);
        } catch (const char* error) {
            delete newStmt;
            throw(error);
        }
    }
    else if(token == "PRINT"){
        try {
            newStmt = new PrintStmt(index, tokens);
            connect(newStmt, &Statement::print, this, &Program::print);
        } catch (const char* error) {
            delete newStmt;
            throw(error);
        }
    }
    else if(token == "INPUT"){
        try {
            newStmt = new InputStmt(index, tokens);
            connect(newStmt, &Statement::input, this, &Program::input);
        } catch (const char* error) {
            delete newStmt;
            throw(error);
        }
    }
    else if(token == "GOTO"){
        try {
            newStmt = new GotoStmt(index, tokens);
            connect(newStmt, &Statement::Goto, this, &Program::Goto);
        } catch (const char* error) {
            delete newStmt;
            throw(error);
        }
    }
    else if(token == "IF"){
        try {
            newStmt = new IfStmt(index, tokens);
            connect(newStmt, &Statement::Goto, this, &Program::Goto);
        } catch (const char* error) {
            delete newStmt;
            throw(error);
        }
    }
    else if(token == "END"){
        try {
            newStmt = new EndStmt(index, tokens);
            connect(newStmt, &Statement::end, this, &Program::end);
        } catch (const char* error) {
            delete newStmt;
            throw(error);
        }
    }
    else if(token == ""){ //输入空行
        newStmt = nullptr;
    }
    else{ //非法输入
        throw("Unknown Command");
    }
}

void Program::store(const int index)
{
    if(stmtList.empty()){ // 第一条语句
        if(!newStmt) return; // 空行，不存储
        stmtList.append(newStmt);
        evalstate.hash.insert(index, 0); // 存储到hash表中，便于从行号查找语句
        // 更新界面
        emit updateCode();
        emit updateTree();
        return;
    }

    int i = 0; // 记录插入位置
    for(auto stmt : stmtList){
        if(stmt->Index() > index){
            // insert the new line
            break;
        }
        else if(stmt->Index() == index){
            if(!newStmt){ // 空行，删除
                // delete the command
                evalstate.hash.remove(stmt->Index());
                // 需要更新hash表，插入位置之后的行号都要减1
                for(auto it = evalstate.hash.begin(); it != evalstate.hash.end(); ++it){
                    if(it.key() > index){
                        it.value() -= 1;
                    }
                }

                delete stmtList.at(i);
                stmtList.removeAt(i);
            }
            else{
                // replace the command
                delete stmtList.at(i);
                stmtList.replace(i, newStmt);
            }
            emit updateCode();
            emit updateTree();
            return;
        }
        ++i;
    }
    if(!newStmt) return;
    stmtList.insert(i, newStmt);
    evalstate.hash.insert(index, i);
    // 需要更新hash表，插入位置之后的行号都要加1
    for(auto it = evalstate.hash.begin(); it != evalstate.hash.end(); ++it){
        if(it.key() > index){
            it.value() += 1;
        }
    }
    emit updateCode();
    emit updateTree();
}

void Program::clear()
{
    for(auto stmt : stmtList){
        delete stmt;
    }

    stmtList.clear();
    context.clear(); // 清空上下文
    emit updateCode();
    emit updateTree();
}

void Program::run()
{
    evalstate.init(); // 初始化evalstate
    while(true){
        try{
            evalstate.nextStmt(); // 获取下一条语句的位置
        }catch(const QString& error){
            handleError(error);
            break;
        }

        // 执行到最后一条语句或者遇到END语句
        if(evalstate.end || evalstate.curStmtPos == stmtList.size()){
            break; // 程序结束
        }

        newStmt = stmtList[evalstate.curStmtPos]; // 获取当前语句
        try{
            // 执行当前语句，返回值表示是否执行完毕
            evalstate.exeFinished = newStmt->execute(context); 
            // 如果是INPUT语句，等待用户输入
            while(!evalstate.exeFinished){
                QCoreApplication::processEvents();
            }
        }
        catch(const char* error){
            handleError(error);
            // 如果出现错误，结束程序
            break;  
        }
    }

    //在语法树窗口更新每个语句执行情况
    emit updateTree(true);

     //清空profile的count计数
    for(auto stmt : stmtList){
        stmt->resetCount();
    }
    //清空上下文中变量的使用次数
    context.resetVarCount();
}

void Program::Goto(const int n)
{
    // 设置跳转
    evalstate.nextLineNum = n;
    evalstate.jump = true;
}

void Program::end()
{
    evalstate.end = true;
}

void Program::print(const int num)
{
    emit updateResult(QString::number(num));
}

void Program::execute()
{
    QString &token = tokens[0];
    if(token == "LET"){
        try {
            newStmt = new LetStmt(tokens);
        } catch (const char* error) {
            delete newStmt;
            throw(error);
        }
    }
    else if(token == "PRINT"){
        try {
            newStmt = new PrintStmt(tokens);
            connect(newStmt, &Statement::print, this, &Program::print);
        } catch (const char* error) {
            delete newStmt;
            throw(error);
        }
    }
    else if(token == "INPUT"){
        try {
            newStmt = new InputStmt(tokens);
            connect(newStmt, &Statement::input, this, &Program::input);
            
        } catch (const char* error) {
            delete newStmt;
            throw(error);
        }
    }
    else{ //非法输入
        throw("Unknown Command");
    }

    // 构造成功，执行
    try {
        evalstate.exeFinished = newStmt->execute(context);
        while(!evalstate.exeFinished){
            QCoreApplication::processEvents();
        }
    } catch (const char* error) {
        handleError(error); // 执行失败，处理错误
    }
    // 该指令是一次性指令，执行完毕后删除
    delete newStmt;
}

void Program::handleError(const char *error)
{
    emit updateResult(error);
}

void Program::handleError(const QString& error)
{
    emit updateResult(error);
}

void Program::input(const QString& var)
{
    evalstate.inputVar = var;
    evalstate.isInput = true;
    emit inputModeChanged();
}

void Program::handleInput(const QString& cmd)
{
    QVector<QString> tokens = tokenizer.split(cmd);

    tokens.pop_front(); // 去掉问号提示符
    // 输入只能有一个变量
    if(tokens.size() != 1){
        throw("Invalid Input");
    }

    QString var = tokens[0];
    bool ok;
    int num = var.toInt(&ok);
    // 检查输入是整数
    if(ok){
        context.setValue(evalstate.inputVar, num);
    }
    else{
        throw("Invalid Input");
    }

    evalstate.isInput = false;
    evalstate.exeFinished = true;
}
