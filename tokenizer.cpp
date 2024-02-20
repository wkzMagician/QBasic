#include "tokenizer.h"

Tokenizer::Tokenizer()
{

}

QVector<QString> Tokenizer::split(const QString& cmd)
{
    QVector<QString> tokens;
    QString str = "";
    for(auto &c : cmd){
        if(c != ' '){
            str.append(c);
        }
        else{
            if(str != ""){
                tokens.append(str);
                str = "";
            }
        }
    }
    // reach EOF, append the last word
    if(str != "")
        tokens.append(str);

    return tokens;
}


