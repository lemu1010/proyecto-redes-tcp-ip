#include "tablepacket.h"

TablePacket::TablePacket()
{

    initGUI();
}

void TablePacket::initGUI()
{

    this->setColumnCount(5);

    QStringList headerRow;
    headerRow << "No." << "Tiempo" << "Fuente" << "Destino" << "Información";

    this->setHorizontalHeaderLabels(headerRow);
    this->verticalHeader()->hide();
    this->setColumnWidth(0,45);
    this->setColumnWidth(1,150);
    this->setColumnWidth(2,220);
    this->setColumnWidth(3,220);
    this->setColumnWidth(4,385);

    rCount = this->rowCount();

}

void TablePacket::addPacket(Packet packet)
{

    rCount += 1;

    this->setRowCount(rCount);

    for( int row = rCount - 1; row < rCount; row++ ) {

        QTableWidgetItem * item = new QTableWidgetItem();
        item->setBackgroundColor(QColor(112,224,255));
        item->setFlags(item->flags() & (~Qt::ItemIsEditable));
        item->setText(QString::number(packet.getNumberPacketCaptured()));
        this->setItem(row, 0, item);

        item = new QTableWidgetItem();
        item->setBackgroundColor(QColor(112,224,255));
        item->setFlags(item->flags() & (~Qt::ItemIsEditable));
        item->setText(QString::number(packet.getTimeStamp()));
        this->setItem(row, 1, item);

        item = new QTableWidgetItem();
        item->setBackgroundColor(QColor(112,224,255));
        item->setFlags(item->flags() & (~Qt::ItemIsEditable));
        item->setText(packet.getIPFuente());
        this->setItem(row, 2, item);

        item = new QTableWidgetItem();
        item->setBackgroundColor(QColor(112,224,255));
        item->setFlags(item->flags() & (~Qt::ItemIsEditable));
        item->setText(packet.getIPDestino());
        this->setItem(row, 3, item);

        item = new QTableWidgetItem();
        item->setBackgroundColor(QColor(112,224,255));
        item->setFlags(item->flags() & (~Qt::ItemIsEditable));
        QString cadInfo;

        if( packet.getSYN() && !packet.getACK() )
            cadInfo = "[SYN] no data";
        else if( packet.getSYN() && packet.getACK() )
            cadInfo = "[SYN,ACK] no data";
        else if( packet.getACK() && packet.getSizeData() == 0 )
            cadInfo = "[ACK] no data";
        else if( packet.getACK() )
            cadInfo = "[ACK] data";
        else if( packet.getFIN() && packet.getACK() )
            cadInfo = "[FIN,ACK]";

        item->setText(cadInfo);
        this->setItem(row, 4, item);



    }
}

