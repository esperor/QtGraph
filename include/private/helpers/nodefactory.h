#pragma once

#include <QJsonValue>
#include <QObject>

#include "logics/typemanager.h"

namespace qtgraph {

class DNode;
class DGraph;
class DPin;
class WANode;
class WCanvas;
enum class EPinDirection;


/* 
    NodeFactory puts out variations of creating a node. There are two main
    functions (as in capabilities): 
    - logical: creating DNode of needed type and
    - visual: creating an object of one of the child classes of WANode
              depending on whether DNode::typeID exists or not

*/
class NodeFactory : public QObject
{
    Q_OBJECT

public:
    NodeFactory(QObject *parent = nullptr);
    
    DNode *makeNodeOfType(int typeID, DGraph *graph) const;
    DNode *makeNodeAndPinsOfType(int typeID, DGraph *graph) const;

    // This function must be kept up to date with all the
    // changes made for WANode and its children classes
    WANode *makeSuitableWNode(DNode *lnode, WCanvas *canvas) const;

    DPin *makePinOfType(int typeID, DNode *node) const;

    NodeTypeManager *getNodeTypeManager() const { return _nodeTypeManager; }
    PinTypeManager *getPinTypeManager() const { return _pinTypeManager; }

    void setNodeTypeManager(NodeTypeManager *manager);
    void setPinTypeManager(PinTypeManager *manager);

    void clear();

signals:
    void cleared();

private:
    void addPinsToNodeByJsonValue(const QJsonValue &val, DNode *node, EPinDirection direction) const;

    NodeTypeManager *_nodeTypeManager;
    PinTypeManager *_pinTypeManager;
};

}