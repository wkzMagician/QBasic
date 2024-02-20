#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <QString>
#include <QVector>

//拆开输入语句，返回一个单词数组
class Tokenizer
{
public:
    Tokenizer();
    QVector<QString> split(const QString& cmd);
};

#endif // TOKENIZER_H
