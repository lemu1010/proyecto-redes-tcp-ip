#include "textpacket.h"
#include <QDebug>

TextPacket::TextPacket()
{
    initGUI();
}

void TextPacket::initGUI()
{
    this->setReadOnly(true);
    this->setCursor(QCursor(Qt::ArrowCursor));
}

void TextPacket::setListPackect(Packet packet)
{
    this->listaPacket.append(packet);
}

void TextPacket::setRowPackect(int row, int col)
{
    QString infomationPacket;
    QString title = "Transmission Control Protocol TCP <br>";
    QString puertos = "Puerto Fuente: " + QString::number(listaPacket[row].getPortFuente()) +//
            "  Puerto Destino: " + QString::number(listaPacket[row].getPortDestino()) + "<br>";
    QString seqNumber = "Número de Secuencia: " + QString::number(listaPacket[row].getSeq()) + "<br>";
    QString seqNextNumber = "[ Siguiente Número de Secuencia: " + QString::number(listaPacket[row].getNextSeq()) + " ]<br>";
    QString ackNumber = "Número de Acknowledgement: " + QString::number(listaPacket[row].getNumAck()) + "<br>";
    QString flags = "Banderas: <br>";
    QString cwr = "      "  + QString::number(listaPacket[row].getCWR()) + " = " + "Reducir Ventana Congestion <br>";
    QString ece = "      "  + QString::number(listaPacket[row].getECE()) + " = " + "ECN-Echo <br>";
    QString urg = "      "  + QString::number(listaPacket[row].getURG()) + " = " + "Urgente <br>";
    QString ack = "      "  + QString::number(listaPacket[row].getACK()) + " = " + "Acknowledgement <br>";
    QString psh = "      "  + QString::number(listaPacket[row].getPUSH()) + " = " + "Empuje <br>";
    QString rst = "      "  + QString::number(listaPacket[row].getRST()) + " = " + "Reajustar <br>";
    QString syn = "      "  + QString::number(listaPacket[row].getSYN()) + " = " + "Syn <br>";
    QString fin = "      "  + QString::number(listaPacket[row].getFIN()) + " = " + "Fin <br>";

    QString packetReconocio = "";
    qDebug() << "ACUSADO: " << listaPacket[row].getAcusado();
    if( listaPacket[row].getAcusado() != -100 )
        packetReconocio = "Este paquete reconoce: " + QString::number(listaPacket[row].getAcusado()) + "<br>";

    QString segmentData = "";
    if( listaPacket[row].getSizeData() != 0 )
        segmentData = "TCP segmento Data: " + QString::number(listaPacket[row].getSizeData()) + "<br>";


    infomationPacket = title + puertos + seqNumber + seqNextNumber + ackNumber + //
                       flags + cwr + ece + urg + ack + psh + rst + syn + fin +   //
                       "<font color = #0000FF>" + packetReconocio + "</font>" + segmentData;

    this->setText(infomationPacket);
}
