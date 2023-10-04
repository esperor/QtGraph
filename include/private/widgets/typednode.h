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
    WTypedNode(int typeID, WCanvas *canvas);

    void protocolize(protocol::Node *pNode) const override;

    int getTypeID() const { return _typeID; }
    void setTypeID(int newTypeID) { _typeID = newTypeID; }

    const NodeTypeManager *getNodeTypeManager() const { return _nodeTypeManager; }
    const PinTypeManager *getPinTypeManager() const { return _pinTypeManager; }

    void setNodeTypeManager(const NodeTypeManager *manager) { _nodeTypeManager = manager; }
    void setPinTypeManager(const PinTypeManager *manager) { _pinTypeManager = manager; }

private:
    void paintName(QPainter *painter, int desiredWidth, QPoint textOrigin) override;
    int calculateRowsOffset(QPainter *painter) const override;

    const NodeTypeManager *_nodeTypeManager;
    const PinTypeManager *_pinTypeManager;

protected:
    int _typeID;

};

}