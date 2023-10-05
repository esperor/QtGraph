#pragma once

#include <QObject>
#include <QtWidgets/QWidget>

#include "widgets/node.h"

namespace qtgraph {

class NodeTypeManager;
class PinTypeManager;

class WTypedNode : public WANode
{
    Q_OBJECT

public:
    WTypedNode(int typeID, LNode *lnode, WCanvas *canvas);

private:
    void paintName(QPainter *painter, int desiredWidth, QPoint textOrigin) override;
    int calculateRowsOffset(QPainter *painter) const override;

    const NodeTypeManager *_nodeTypeManager;
    const PinTypeManager *_pinTypeManager;

protected:

};

}