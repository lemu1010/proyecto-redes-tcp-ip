#ifndef TEXTPAYLOAD_H
#define TEXTPAYLOAD_H

#include <QTextEdit>
#include <tablepacket.h>

class TextPayload : public QTextEdit
{
    Q_OBJECT

    TablePacket *tablePacket;

public:
    explicit TextPayload();
    void setTablePacket(TablePacket *);

public slots:
     void setTextPayload(int, int);
    
};

#endif // TEXTPAYLOAD_H
