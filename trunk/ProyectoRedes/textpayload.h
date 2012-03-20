#ifndef TEXTPAYLOAD_H
#define TEXTPAYLOAD_H

#include <QTextEdit>

class TextPayload : public QTextEdit
{
    Q_OBJECT
public:
    explicit TextPayload();
    
signals:
    
public slots:
     void setTextPayload(int ,int);
    
};

#endif // TEXTPAYLOAD_H
