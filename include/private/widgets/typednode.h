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
    WTypedNode(int typeID, const DNode *lnode, WCanvas *canvas);

    void setNodeTypeManager(NodeTypeManager *ntm) { _nodeTypeManager = ntm; }

private:
    void paintName(QPainter *painter, int desiredWidth, QPoint textOrigin) override;
    int calculateRowsOffset(QPainter *painter) const override;

    NodeTypeManager *_nodeTypeManager;

protected:

};

}