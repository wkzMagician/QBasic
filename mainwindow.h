#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "program.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

//窗口显示类
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_cmdLineEdit_editingFinished();
    void on_btnClearCode_clicked();
    void on_btnLoadCode_clicked();
    void on_btnRunCode_clicked();
private:
    Ui::MainWindow *ui;
    // include:
    // CodeDisplay
    // textBrowser
    // TreeDisplay

    Program program;

    void load(const QString& filePath);
public slots:
    void updateTree(const bool show = false); // 更新语法树显示，默认不显示profile
    void updateCode(); // 更新代码显示
    void updateResult(const QString &result)const; // 更新打印结果
    void setInputMode(); // 设置输入模式ui(Input)
};


#endif // MAINWINDOW_H
