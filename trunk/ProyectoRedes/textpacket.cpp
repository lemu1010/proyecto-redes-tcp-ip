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
