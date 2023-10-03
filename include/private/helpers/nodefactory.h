#pragma once

#include <QJsonValue>
#include <QObject>

#include "logics/nodetypemanager.h"
#include "logics/pintypemanager.h"
#include "qtgraph.h"
#include "logics/pin.h"

namespace qtgraph {

class TypedNode;
class Canvas;
enum class EPinDirection;

class NodeFactory : public QObject
{
    Q_OBJECT

public:
    NodeFactory();

    
    TypedNode *makeNodeOfType(int typeID, Canvas *canvas) const;
    TypedNode *makeNodeAndPinsOfType(int typeID, Canvas *canvas) const;

    WPin *makePinOfType(int typeID, WANode *node) const;

    const NodeTypeManager *getNodeTypeManager() const { return _nodeTypeManager; }
    const PinTypeManager *getPinTypeManager() const { return _pinTypeManager; }

    void setNodeTypeManager(const NodeTypeManager *manager) { _nodeTypeManager = manager; }
    void setPinTypeManager(const PinTypeManager *manager) { _pinTypeManager = manager; }

private:
    void addPinsToNodeByJsonValue(const QJsonValue &val, TypedNode *node, EPinDirection direction) const;

    const NodeTypeManager *_nodeTypeManager;
    const PinTypeManager *_pinTypeManager;
};

}