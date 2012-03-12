#ifndef TABLEPACKET_H
#define TABLEPACKET_H

#include <QTableWidget>
#include <QtGui>
#include <packet.h>
#include <textpacket.h>

class TablePacket : public QTableWidget
{

public:
    TablePacket();
    void addPacket(Packet );

private:
    void initGUI();

private:
    int rCount;
    int cCount;

};

#endif // TABLEPACKET_H
