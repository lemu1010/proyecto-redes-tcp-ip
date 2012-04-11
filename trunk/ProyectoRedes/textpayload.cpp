#include "textpayload.h"
#include <QDebug>

TextPayload::TextPayload()
{
    this->setReadOnly(true);
}

void TextPayload::setTablePacket(TablePacket *tablePacket)
{
    this->tablePacket = tablePacket;
}

void TextPayload::setTextPayload(int row, int col)
{
    QString str;

    this->clear();

    if( tablePacket->getPacket(row).getSizePayload() > 0 ) {

        str.append("\n");

        for( int i = 0; i < tablePacket->getPacket(row).getPayload().size(); i++ ) {
            str.append( tablePacket->getPacket(row).getPayload().at(i).c_str() );
        }

        this->setText(str);
    }


}
