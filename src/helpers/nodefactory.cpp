#include <QJsonArray>

#include "logics/node.h"
#include "logics/graph.h"
#include "logics/pin.h"
#include "logics/nodetypemanager.h"
#include "logics/pintypemanager.h"
#include "widgets/node.h"
#include "widgets/customnode.h"
#include "widgets/typednode.h"
#include "helpers/nodefactory.h"
#include "utilities/utility.h"

#include "helpers/moc_nodefactory.cpp"

namespace qtgraph {

NodeFactory::NodeFactory()
    : _nodeTypeManager{ new NodeTypeManager() }
    , _pinTypeManager{ new PinTypeManager() }
{}

LNode *NodeFactory::makeNodeOfType(int typeID, LGraph *graph) const
{
    LNode *node = new LNode(graph);

    node->setName(_nodeTypeManager->Types()[typeID].value("name").toString());
    node->setNodeTypeManager(QSharedPointer<const NodeTypeManager>(_nodeTypeManager));
    node->setPinTypeManager(QSharedPointer<const PinTypeManager>(_pinTypeManager));
    node->setTypeID(typeID);

    return node;
}

LNode *NodeFactory::makeNodeAndPinsOfType(int typeID, LGraph *graph) const
{
    LNode *node = makeNodeOfType(typeID, graph);

    const QJsonObject &type = _nodeTypeManager->Types()[typeID];
    QJsonValue inPins = type.value("in-pins");
    QJsonValue outPins = type.value("out-pins");

    if (inPins != QJsonValue::Undefined)
        addPinsToNodeByJsonValue(inPins, node, EPinDirection::In);

    if (outPins != QJsonValue::Undefined)
        addPinsToNodeByJsonValue(outPins, node, EPinDirection::Out);

    return node;
}

WANode *NodeFactory::makeSuitableWNode(LNode *lnode, WCanvas *canvas) const
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

LPin *NodeFactory::makePinOfType(int typeID, LNode *node) const
{
    auto type = _pinTypeManager->Types()[typeID];
    QColor color = parseToColor(type.value("color").toString());
    QString typeName = _pinTypeManager->typeNameByID(typeID);

    LPin *pin = new LPin(node);
    pin->setColor(color);
    pin->setText(typeName);
    pin->setTypeID(typeID);

    return pin;
}

void NodeFactory::addPinsToNodeByJsonValue(const QJsonValue &val, LNode *node, EPinDirection direction) const
{
    const QVector<QJsonObject> &pinTypes = _pinTypeManager->Types();
    QJsonArray pins = val.toArray();

    for (auto it = pins.begin(); it != pins.end(); it++)
    {
        QJsonObject pinObject = (*it).toObject();

        QString typeName = pinObject.value("type").toString();
        int id = _pinTypeManager->TypeNames()[typeName];

        LPin *pin = makePinOfType(id, node);
        pin->setDirection(direction);

        node->addPin(pin);
    }
}

}

