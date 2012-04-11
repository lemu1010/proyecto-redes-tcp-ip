#ifndef TABLEPACKET_H
#define TABLEPACKET_H

#include <QtGui>
#include <packet.h>

class TablePacket : public QTableWidget
{

    QList<Packet> listaPacket;

public:
    TablePacket();
    void addPacket(Packet );
    Packet getPacket(int row);

private:
    void initGUI();

private:
    int rCount;
    int cCount;

};

#endif // TABLEPACKET_H
