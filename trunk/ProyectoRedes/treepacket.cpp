#include "treepacket.h"
#include <QDebug>
TreePacket::TreePacket()
{
    initGUI();
}

void TreePacket::initGUI()
{
    this->setHeaderHidden(true);

    itemFather = new QTreeWidgetItem();
    itemFather->setText(0,"Transmission Control Protocol TCP");

    itemPortSrc = new QTreeWidgetItem(itemFather);
    itemPortDst = new QTreeWidgetItem(itemFather);
    itemSeqNumber = new QTreeWidgetItem(itemFather);
    itemSeqNext = new QTreeWidgetItem(itemFather);
    itemAckNumber = new QTreeWidgetItem(itemFather);
    itemFlag = new QTreeWidgetItem(itemFather);
    itemFlag->setText(0,"Banderas: ");
    itemFlagCwr = new QTreeWidgetItem(itemFlag);
    itemFlagEce = new QTreeWidgetItem(itemFlag);
    itemFlagUrg = new QTreeWidgetItem(itemFlag);
    itemFlagAck = new QTreeWidgetItem(itemFlag);
    itemFlagPsh = new QTreeWidgetItem(itemFlag);
    itemFlagRst = new QTreeWidgetItem(itemFlag);
    itemFlagSyn = new QTreeWidgetItem(itemFlag);
    itemFlagFin = new QTreeWidgetItem(itemFlag);

    itemPacketReconocio = new QTreeWidgetItem();
    itemSegmentData = new QTreeWidgetItem();
}

void TreePacket::setListPackect(Packet packet)
{
    this->listaPacket.append(packet);
}

Packet TreePacket::getPacket(int row)
{
    return listaPacket.at(row);
}

void TreePacket::setRowPackect(int row, int col)
{

    itemFather->removeChild(itemPacketReconocio);
    itemFather->removeChild(itemSegmentData);

    if( listaPacket[row].getRestransmision() )
        itemFather->setBackgroundColor(0,Qt::yellow);
    else
        itemFather->setBackgroundColor(0,QColor(206,206,224));

    this->addTopLevelItem(itemFather);

    itemPortSrc->setText(0,"Puerto Fuente: " + QString::number(listaPacket[row].getPortFuente()));
    itemPortDst->setText(0,"Puerto Destino: " + QString::number(listaPacket[row].getPortDestino()));
    itemSeqNumber->setText(0,"Número de Secuencia: " + QString::number(listaPacket[row].getSeq()));
    itemSeqNext->setText(0,"[ Siguiente Número de Secuencia: " + QString::number(listaPacket[row].getNextSeq()) + " ]");
    itemAckNumber->setText(0,"Número de Acknowledgement: " + QString::number(listaPacket[row].getNumAck()));

    itemFlagCwr->setText(0,QString::number(listaPacket[row].getCWR()) + " = " + "CWR (Congestion Window Reduced)");
    itemFlagEce->setText(0,QString::number(listaPacket[row].getECE()) + " = " + "ECE (ECN-Echo)");
    itemFlagUrg->setText(0,QString::number(listaPacket[row].getURG()) + " = " + "URG (Urgent)");
    itemFlagAck->setText(0,QString::number(listaPacket[row].getACK()) + " = " + "ACK (Acknowledgement)");
    itemFlagPsh->setText(0,QString::number(listaPacket[row].getPUSH()) + " = " + "PSH (Push)");
    itemFlagRst->setText(0,QString::number(listaPacket[row].getRST()) + " = " + "RST (Reset)");
    itemFlagSyn->setText(0,QString::number(listaPacket[row].getSYN()) + " = " + "Syn (Syn)");
    itemFlagFin->setText(0,QString::number(listaPacket[row].getFIN()) + " = " + "Fin (Fin)");

    if( listaPacket[row].getAcusado() != -100 ) {
        itemPacketReconocio->setForeground(0,QBrush(Qt::blue));
        itemPacketReconocio->setText(0,"Este paquete reconoce recepción de fragmento: " + QString::number(listaPacket[row].getAcusado()));
        itemFather->addChild(itemPacketReconocio);
    }

    if( listaPacket[row].getSizeData() != 0 ) {
        itemSegmentData->setText(0,"TCP segmento Data: " + QString::number(listaPacket[row].getSizeData()));
        itemFather->addChild(itemSegmentData);
    }
}
