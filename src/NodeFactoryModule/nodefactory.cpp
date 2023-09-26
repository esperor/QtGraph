#include <QJsonArray>

#include "GraphWidgets/typednode.h"
#include "GraphWidgets/canvas.h"
#include "GraphWidgets/pin.h"
#include "TypeManagers/nodetypemanager.h"
#include "TypeManagers/pintypemanager.h"
#include "NodeFactoryModule/nodefactory.h"
#include "utility.h"

#include "NodeFactoryModule/moc_nodefactory.cpp"

namespace GraphLib {

namespace NodeFactoryModule {


NodeFactory::NodeFactory()
{}

TypedNode *NodeFactory::makeNodeOfType(int typeID, Canvas *canvas) const
{
    TypedNode *node = new TypedNode(typeID, canvas);

    node->setName(_nodeTypeManager->Types()[typeID].value("name").toString());
    node->setNodeTypeManager(_nodeTypeManager);
    node->setPinTypeManager(_pinTypeManager);

    return node;
}

TypedNode *NodeFactory::makeNodeAndPinsOfType(int typeID, Canvas *canvas) const
{
    TypedNode *node = makeNodeOfType(typeID, canvas);

    const QJsonObject &type = _nodeTypeManager->Types()[typeID];
    QJsonValue inPins = type.value("in-pins");
    QJsonValue outPins = type.value("out-pins");

    if (inPins != QJsonValue::Undefined)
        addPinsToNodeByJsonValue(inPins, node, PinDirection::In);

    if (outPins != QJsonValue::Undefined)
        addPinsToNodeByJsonValue(outPins, node, PinDirection::Out);

    return node;
}

Pin *NodeFactory::makePinOfType(int typeID, BaseNode *node) const
{
    auto type = _pinTypeManager->Types()[typeID];
    QColor color = parseToColor(type.value("color").toString());
    QString typeName = _pinTypeManager->typeNameByID(typeID);

    Pin *pin = new Pin(node);
    pin->setColor(color);
    pin->setText(typeName);
    pin->setTypeID(typeID);

    return pin;
}

void NodeFactory::addPinsToNodeByJsonValue(const QJsonValue &val, TypedNode *node, PinDirection direction) const
{
    const QVector<QJsonObject> &pinTypes = _pinTypeManager->Types();
    QJsonArray pins = val.toArray();

    for (auto it = pins.begin(); it < pins.end(); it++)
    {
        QJsonObject pinObject = (*it).toObject();

        QString typeName = pinObject.value("type").toString();
        int id = _pinTypeManager->TypeNames()[typeName];

        Pin *pin = makePinOfType(id, node);
        pin->setDirection(direction);

        node->addPin(pin);
    }
}


}

}

