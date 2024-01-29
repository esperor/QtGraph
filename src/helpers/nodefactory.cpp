#include <QJsonArray>

#include "data/node.h"
#include "data/graph.h"
#include "data/pin.h"
#include "logics/typemanager.h"
#include "widgets/node.h"
#include "widgets/customnode.h"
#include "widgets/typednode.h"
#include "helpers/nodefactory.h"
#include "utilities/utility.h"

#include "helpers/moc_nodefactory.cpp"

namespace qtgraph {

NodeFactory::NodeFactory(QObject *parent)
    : QObject(parent)
    , _nodeTypeManager{ nullptr }
    , _pinTypeManager{ nullptr }
{}

DNode *NodeFactory::makeNodeOfType(int typeID, DGraph *graph) const
{
    DNode *node = new DNode(graph);

    node->setName(_nodeTypeManager->Types()[typeID].value("name").toString());
    node->setNodeTypeManager(_nodeTypeManager);
    node->setPinTypeManager(_pinTypeManager);
    node->setTypeID(typeID);

    return node;
}

DNode *NodeFactory::makeNodeAndPinsOfType(int typeID, DGraph *graph) const
{
    DNode *node = makeNodeOfType(typeID, graph);

    const QJsonObject &type = _nodeTypeManager->Types()[typeID];
    QJsonValue inPins = type.value("in-pins");
    QJsonValue outPins = type.value("out-pins");

    if (inPins != QJsonValue::Undefined)
        addPinsToNodeByJsonValue(inPins, node, EPinDirection::In);

    if (outPins != QJsonValue::Undefined)
        addPinsToNodeByJsonValue(outPins, node, EPinDirection::Out);

    return node;
}

WANode *NodeFactory::makeSuitableWNode(DNode *lnode, WCanvas *canvas) const
{
    WANode *node;
    if (lnode->getTypeID())
    {
        node = new WTypedNode(*lnode->getTypeID(), lnode, canvas);
        ((WTypedNode*)node)->setNodeTypeManager(_nodeTypeManager);
    }
    else
        node = new WCustomNode(lnode, canvas);

    return node;
}

DPin *NodeFactory::makePinOfType(int typeID, DNode *node) const
{
    auto type = _pinTypeManager->Types()[typeID];
    QColor color = parseToColor(type.value("color").toString());
    QString typeName = _pinTypeManager->typeNameByID(typeID);

    DPin *pin = new DPin(node);
    pin->setColor(color);
    pin->setText(typeName);
    pin->setTypeID(typeID);

    return pin;
}

void NodeFactory::addPinsToNodeByJsonValue(const QJsonValue &val, DNode *node, EPinDirection direction) const
{
    const QVector<QJsonObject> &pinTypes = _pinTypeManager->Types();
    QJsonArray pins = val.toArray();

    for (auto it = pins.begin(); it != pins.end(); it++)
    {
        QJsonObject pinObject = (*it).toObject();

        QString typeName = pinObject.value("type").toString();
        int id = _pinTypeManager->TypeNames()[typeName];

        DPin *pin = makePinOfType(id, node);
        pin->setDirection(direction);

        node->addPin(pin);
    }
}

void NodeFactory::setNodeTypeManager(NodeTypeManager *manager)
{ 
    _nodeTypeManager = manager; 
    _nodeTypeManager->setParent(this); 
}

void NodeFactory::setPinTypeManager(PinTypeManager *manager) 
{ 
    _pinTypeManager = manager; 
    _pinTypeManager->setParent(this); 
}

void NodeFactory::clear()
{
    delete _nodeTypeManager;
    _nodeTypeManager = nullptr;
    delete _pinTypeManager;
    _pinTypeManager = nullptr;

    emit cleared();
}

}

