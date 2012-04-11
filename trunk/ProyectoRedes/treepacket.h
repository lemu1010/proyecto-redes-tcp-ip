#ifndef TEXTPACKET_H
#define TEXTPACKET_H

#include <QTreeWidget>
#include <tablepacket.h>
#include <packet.h>

class TreePacket : public QTreeWidget
{
    Q_OBJECT

    TablePacket *tablePacket;

    QTreeWidgetItem *itemFather;
    QTreeWidgetItem *itemPortSrc;
    QTreeWidgetItem *itemPortDst;
    QTreeWidgetItem *itemSeqNumber;
    QTreeWidgetItem *itemSeqNext;
    QTreeWidgetItem *itemAckNumber;
    QTreeWidgetItem *itemHeaderTCP;
    QTreeWidgetItem *itemFlag;
    QTreeWidgetItem *itemFlagCwr;
    QTreeWidgetItem *itemFlagEce;
    QTreeWidgetItem *itemFlagUrg;
    QTreeWidgetItem *itemFlagAck;
    QTreeWidgetItem *itemFlagPsh;
    QTreeWidgetItem *itemFlagRst;
    QTreeWidgetItem *itemFlagSyn;
    QTreeWidgetItem *itemFlagFin;
    QTreeWidgetItem *itemPacketReconocio;
    QTreeWidgetItem *itemSegmentData;

public:
    explicit TreePacket();
    void setTablePacket(TablePacket *);
    
private:
    void initGUI();

signals:
    
public slots:
    void setRowPackect(int,int);
    
};

#endif // TEXTPACKET_H
