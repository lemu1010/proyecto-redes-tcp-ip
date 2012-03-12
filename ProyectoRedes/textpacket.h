#ifndef TEXTPACKET_H
#define TEXTPACKET_H

#include <QTextEdit>
#include <QTextStream>
#include <packet.h>

class TextPacket : public QTextEdit
{
    Q_OBJECT

    QList<Packet> listaPacket;

public:
    explicit TextPacket();
    void setListPackect(Packet );
    
private:
    void initGUI();

signals:
    
public slots:
    void setRowPackect(int,int);
    
};

#endif // TEXTPACKET_H
