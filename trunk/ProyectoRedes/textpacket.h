#ifndef TEXTPACKET_H
#define TEXTPACKET_H

#include <QTextEdit>

class TextPacket : public QTextEdit
{
    Q_OBJECT
public:
    explicit TextPacket();
    
private:
    void initGUI();

signals:
    
public slots:
    void setRowPackect(int,int);
    
};

#endif // TEXTPACKET_H
