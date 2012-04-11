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
    itemHeaderTCP = new QTreeWidgetItem(itemFather);
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

void TreePacket::setTablePacket(TablePacket * tablePacket)
{
    this->tablePacket = tablePacket;
}

void TreePacket::setRowPackect(int row, int col)
{

    itemFather->removeChild(itemPacketReconocio);
    itemFather->removeChild(itemSegmentData);

    if( tablePacket->getPacket(row).getRestransmision() )
        itemFather->setBackgroundColor(0,Qt::yellow);
    else
        itemFather->setBackgroundColor(0,QColor(206,206,224));

    this->addTopLevelItem(itemFather);

    itemPortSrc->setText(0,"Puerto Fuente: " + QString::number(tablePacket->getPacket(row).getPortFuente()));
    itemPortDst->setText(0,"Puerto Destino: " + QString::number(tablePacket->getPacket(row).getPortDestino()));
    itemSeqNumber->setText(0,"Número de Secuencia: " + QString::number(tablePacket->getPacket(row).getSeq()));
    itemSeqNext->setText(0,"[ Siguiente Número de Secuencia: " + QString::number(tablePacket->getPacket(row).getNextSeq()) + " ]");
    itemAckNumber->setText(0,"Número de Acknowledgement: " + QString::number(tablePacket->getPacket(row).getNumAck()));
    itemHeaderTCP->setText(0,"Longitud de Cabecera: " + QString::number(tablePacket->getPacket(row).getSizeTCP()) + " bytes");

    itemFlagCwr->setText(0,QString::number(tablePacket->getPacket(row).getCWR()) + " = " + "CWR (Congestion Window Reduced)");
    itemFlagEce->setText(0,QString::number(tablePacket->getPacket(row).getECE()) + " = " + "ECE (ECN-Echo)");
    itemFlagUrg->setText(0,QString::number(tablePacket->getPacket(row).getURG()) + " = " + "URG (Urgent)");
    itemFlagAck->setText(0,QString::number(tablePacket->getPacket(row).getACK()) + " = " + "ACK (Acknowledgement)");
    itemFlagPsh->setText(0,QString::number(tablePacket->getPacket(row).getPUSH()) + " = " + "PSH (Push)");
    itemFlagRst->setText(0,QString::number(tablePacket->getPacket(row).getRST()) + " = " + "RST (Reset)");
    itemFlagSyn->setText(0,QString::number(tablePacket->getPacket(row).getSYN()) + " = " + "Syn (Syn)");
    itemFlagFin->setText(0,QString::number(tablePacket->getPacket(row).getFIN()) + " = " + "Fin (Fin)");

    if( tablePacket->getPacket(row).getAcusado() != -100 ) {
        itemPacketReconocio->setForeground(0,QBrush(Qt::blue));
        itemPacketReconocio->setText(0,"Este paquete reconoce recepción de fragmento: " + QString::number(tablePacket->getPacket(row).getAcusado()));
        itemFather->addChild(itemPacketReconocio);
    }

    if( tablePacket->getPacket(row).getSizeData() != 0 ) {
        itemSegmentData->setText(0,"TCP segmento Data: " + QString::number(tablePacket->getPacket(row).getSizeData()));
        itemFather->addChild(itemSegmentData);
    }
}
