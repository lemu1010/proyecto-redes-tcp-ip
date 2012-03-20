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

        // Column = 0
        QTableWidgetItem * item = new QTableWidgetItem();
        item->setFlags(item->flags() & (~Qt::ItemIsEditable));

        if( packet.getAckRepetido() || packet.getRestransmision() ) {
            item->setBackgroundColor(QColor(0,0,0));
            item->setForeground(QBrush(QColor(255,0,0)));
        }
        else
            item->setBackgroundColor(QColor(112,224,255));

        item->setText(QString::number(packet.getNumberPacketCaptured()));
        this->setItem(row, 0, item);

        // Column = 1
        item = new QTableWidgetItem();
        item->setFlags(item->flags() & (~Qt::ItemIsEditable));

        if( packet.getAckRepetido() || packet.getRestransmision() ) {
            item->setBackgroundColor(QColor(0,0,0));
            item->setForeground(QBrush(QColor(255,0,0)));
        }
        else
            item->setBackgroundColor(QColor(112,224,255));

        item->setText(QString::number(packet.getTimeStamp()));
        this->setItem(row, 1, item);

        // Column = 2
        item = new QTableWidgetItem();
        item->setFlags(item->flags() & (~Qt::ItemIsEditable));

        if( packet.getAckRepetido() || packet.getRestransmision() ) {
            item->setBackgroundColor(QColor(0,0,0));
            item->setForeground(QBrush(QColor(255,0,0)));
        }
        else
            item->setBackgroundColor(QColor(112,224,255));

        item->setText(packet.getIPFuente());
        this->setItem(row, 2, item);

        // Column = 3
        item = new QTableWidgetItem();
        item->setFlags(item->flags() & (~Qt::ItemIsEditable));

        if( packet.getAckRepetido() || packet.getRestransmision() ) {
            item->setBackgroundColor(QColor(0,0,0));
            item->setForeground(QBrush(QColor(255,0,0)));
        }
        else
            item->setBackgroundColor(QColor(112,224,255));

        item->setText(packet.getIPDestino());
        this->setItem(row, 3, item);

        // Column = 4
        item = new QTableWidgetItem();
        item->setFlags(item->flags() & (~Qt::ItemIsEditable));

        if( packet.getAckRepetido() || packet.getRestransmision() ) {
            item->setBackgroundColor(QColor(0,0,0));
            item->setForeground(QBrush(QColor(255,0,0)));
        }
        else
            item->setBackgroundColor(QColor(112,224,255));

        QString cadInfo;
        QString cadInfoAddicional = QString(" Seq = " + QString::number(packet.getSeq()) + //
                                            QString(" Ack = ") + QString::number(packet.getNumAck()) + //
                                            QString(" Len = ") + QString::number(packet.getSizeData()));

        if( packet.getSYN() && !packet.getACK() )
            cadInfo = "[SYN]" +  cadInfoAddicional;
        else if( packet.getSYN() && packet.getACK() )
            cadInfo = "[SYN,ACK]" + cadInfoAddicional;
        else if( packet.getACK() && packet.getSizeData() == 0 && not (packet.getFIN()))
            cadInfo = "[ACK]" + cadInfoAddicional;
        else if( packet.getACK() &&not (packet.getFIN()) && packet.getSizeData()>0 && !packet.getRestransmision())
            cadInfo = "[TCP Segemento Data]";
        else if( packet.getACK() &&not (packet.getFIN()) && packet.getSizeData()>0 && packet.getRestransmision())
            cadInfo = "[TCP Retransmisión]";
        else if( packet.getFIN() && packet.getACK() )
            cadInfo = "[FIN,ACK]" + cadInfoAddicional;

        item->setText(cadInfo);
        this->setItem(row, 4, item);

    }

}


