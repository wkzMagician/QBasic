#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QTextStream>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(&program, &Program::updateCode, this, &MainWindow::updateCode);
    connect(&program, &Program::inputModeChanged, this, &MainWindow::setInputMode);
    connect(&program, &Program::updateResult, this, &MainWindow::updateResult);
    connect(&program, &Program::updateTree, this, &MainWindow::updateTree);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_cmdLineEdit_editingFinished()
{
    QString cmd = ui->cmdLineEdit->text();
    ui->cmdLineEdit->setText("");

    if(program.inputMode()){ // 程序在运行，且要求从用户输入中获取变量值
        try{
            program.handleInput(cmd);
        }catch(...){
            ui->resultDisplay->append("Invalid Input");
        }   
    }
    else{ // 读入命令
        program.handle(cmd);
    }
}

void MainWindow::on_btnClearCode_clicked()
{
    // 清空代码以及界面
    program.clear();
    ui->resultDisplay->clear();
}

void MainWindow::on_btnLoadCode_clicked()
{
    // 打开一个窗口，选择文件
    QString filePath = QFileDialog::getOpenFileName(this, tr("选择文件"), "", tr("文本文件 (*.txt);;所有文件 (*)"));
    if (!filePath.isEmpty()) {
        // 用户选择了文件，可以在这里进行处理
        QFileInfo fileInfo(filePath);
        QString extension = fileInfo.suffix().toLower();  // 获取小写的文件后缀名
        /*
        if (extension == "txt") {
            load(filePath);
        } else {
            QMessageBox::information(this, tr("文件选择"), tr("您选择的文件不是txt格式!"));
        }*/
        load(filePath);
    }
}

void MainWindow::on_btnRunCode_clicked()
{
    program.run();
}

void MainWindow::updateCode()
{
    ui->CodeDisplay->clear();
    for(auto stmt : program.stmtList){
        ui->CodeDisplay->append(stmt->toString()); // 逐行显示代码
    }
}

void MainWindow::updateResult(const QString& result)const
{
    ui->resultDisplay->append(result);
}

void MainWindow::setInputMode()
{
    //设置输入模式
    //文本框显示"?"，并且光标输入位置在"?"后面
    ui->cmdLineEdit->setText("? ");
    ui->cmdLineEdit->setCursorPosition(2);
}

void MainWindow::load(const QString &filePath)
{
    QFile file;
    file.setFileName(filePath);
    if (file.open(QIODevice::ReadOnly)) {
        //读取文件
        QTextStream in(&file); // 新建文本流对象
        while(!in.atEnd()){
            QString line = in.readLine(); // 读取一行
            program.handle(line); // 逐行处理
        }
        file.close();
    }
}

void MainWindow::updateTree(bool show)//show表示是否显示profile
{
    ui->treeDisplay->clear();
    for(auto stmt : program.stmtList){
        ui->treeDisplay->append(stmt->toTree(show, program.getContext()));
    }
}
