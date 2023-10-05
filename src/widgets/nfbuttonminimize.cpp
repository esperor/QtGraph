#include <QPaintEvent>

#include "widgets/nfbuttonminimize.h"
#include "utilities/constants.h"

#include "widgets/moc_nfbuttonminimize.cpp"

namespace qtgraph {

NFButtonMinimize::NFButtonMinimize(QWidget *parent)
    : QWidget{ parent }
    , _painter{ new QPainter() }
{}

void NFButtonMinimize::paintEvent(QPaintEvent *event)
{
    _painter->begin(this);
    _painter->setRenderHint(QPainter::Antialiasing, true);
    setUpdatesEnabled(false);

    QPen pen(color);
    _painter->setPen(pen);

    _painter->drawText(event->rect(), Qt::AlignCenter, text);

    pen.setColor(backgroundColor);
    _painter->setBrush(backgroundColor);
    _painter->setPen(pen);

    _painter->drawRoundedRect(event->rect(), c_nodeRoundingRadius / 2, c_nodeRoundingRadius / 2);

    setUpdatesEnabled(true);
    _painter->end();
}

}