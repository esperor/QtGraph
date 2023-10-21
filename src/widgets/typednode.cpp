#include "logics/nodetypemanager.h"
#include "widgets/typednode.h"
#include "utilities/constants.h"
#include "widgets/canvas.h"
#include "utilities/utility.h"

#include "widgets/moc_typednode.cpp"

namespace qtgraph {

WTypedNode::WTypedNode(int typeID, LNode *lnode, WCanvas *canvas)
    : WANode(lnode, canvas)
{}

int WTypedNode::calculateRowsOffset(QPainter *painter) const
{
    QFont font = standardFont(c_nodeNameSize * _zoom);
    painter->setFont(font);
    QSize nameBounding = painter->fontMetrics().size(Qt::TextSingleLine, getLogical()->getName());
    return nameBounding.height() * 2.25f + c_normalPinD * _zoom;
}

void WTypedNode::paintName(QPainter *painter, int desiredWidth, QPoint textOrigin)
{
    QSize nameBounding = painter->fontMetrics().size(Qt::TextSingleLine, getLogical()->getName());

    painter->setFont(standardFont(c_nodeNameSize * _zoom));

    painter->drawText(QRect(textOrigin.x(), textOrigin.y(), desiredWidth, nameBounding.height() * 2),
                     (Qt::AlignVCenter | Qt::AlignHCenter), getLogical()->getName());

    QPen pen = painter->pen();
    QColor temp = pen.color();
    pen.setColor(temp.darker(120));
    painter->setPen(pen);
    painter->setFont(standardFont(c_nodeNameSize * 0.75f * _zoom));

    painter->drawText(QRect(textOrigin.x(), textOrigin.y() + nameBounding.height(), desiredWidth, nameBounding.height() * 2),
                      (Qt::AlignVCenter | Qt::AlignHCenter), _nodeTypeManager->typeNameByID(*getLogical()->getTypeID()));

    pen.setColor(temp);
    painter->setPen(pen);
}

}


