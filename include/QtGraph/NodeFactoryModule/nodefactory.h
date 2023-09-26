#pragma once

#include <QJsonValue>
#include <QObject>

#include "QtGraph/TypeManagers/nodetypemanager.h"
#include "QtGraph/TypeManagers/pintypemanager.h"
#include "QtGraph/GraphLib.h"
#include "QtGraph/GraphWidgets/pin.h"

namespace GraphLib {

class TypedNode;
class Canvas;
enum class PinDirection;

namespace NodeFactoryModule {

class GRAPHLIB_EXPORT NodeFactory : public QObject
{
    Q_OBJECT

public:
    NodeFactory();

    
    TypedNode *makeNodeOfType(int typeID, Canvas *canvas) const;
    TypedNode *makeNodeAndPinsOfType(int typeID, Canvas *canvas) const;

    Pin *makePinOfType(int typeID, BaseNode *node) const;

    const NodeTypeManager *getNodeTypeManager() const { return _nodeTypeManager; }
    const PinTypeManager *getPinTypeManager() const { return _pinTypeManager; }

    void setNodeTypeManager(const NodeTypeManager *manager) { _nodeTypeManager = manager; }
    void setPinTypeManager(const PinTypeManager *manager) { _pinTypeManager = manager; }

private:
    void addPinsToNodeByJsonValue(const QJsonValue &val, TypedNode *node, PinDirection direction) const;

    const NodeTypeManager *_nodeTypeManager;
    const PinTypeManager *_pinTypeManager;
};

}

}