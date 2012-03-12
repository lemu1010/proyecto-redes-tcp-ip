#include "textpacket.h"

TextPacket::TextPacket()
{
    initGUI();
}

void TextPacket::initGUI()
{
    this->setReadOnly(true);
    this->setCursor(QCursor(Qt::ArrowCursor));
}

void TextPacket::setRowPackect(int row, int col)
{
    this->setText("Clicked Row: " + QString::number(row) + " Column: " + QString::number(col));
}
