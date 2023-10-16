#pragma once

#include <QJsonValue>
#include <QObject>

#include "logics/nodetypemanager.h"
#include "logics/pintypemanager.h"

namespace qtgraph {

class LNode;
class LGraph;
class LPin;
class WANode;
class WCanvas;
enum class EPinDirection;


/* 
    NodeFactory puts out variations of creating a node. There are two main
    functions (as in capabilities): 
    - logical: creating LNode of needed type and
    - visual: creating an object of one of the child classes of WANode
              depending on whether LNode::typeID exists or not

*/
class NodeFactory : public QObject
{
    Q_OBJECT

public:
    NodeFactory();

    
    LNode *makeNodeOfType(int typeID, LGraph *graph) const;
    LNode *makeNodeAndPinsOfType(int typeID, LGraph *graph) const;

    // This function must be kept up to date with all the
    // changes made for WANode and its children classes
    WANode *makeSuitableWNode(LNode *lnode, WCanvas *canvas) const;

    LPin *makePinOfType(int typeID, LNode *node) const;

    QSharedPointer<const NodeTypeManager> getNodeTypeManager() const { return _nodeTypeManager; }
    QSharedPointer<const PinTypeManager> getPinTypeManager() const { return _pinTypeManager; }

    void setNodeTypeManager(QSharedPointer<const NodeTypeManager> manager) { _nodeTypeManager = manager; }
    void setPinTypeManager(QSharedPointer<const PinTypeManager> manager) { _pinTypeManager = manager; }

private:
    void addPinsToNodeByJsonValue(const QJsonValue &val, LNode *node, EPinDirection direction) const;

    QSharedPointer<const NodeTypeManager> _nodeTypeManager;
    QSharedPointer<const PinTypeManager> _pinTypeManager;
};

}