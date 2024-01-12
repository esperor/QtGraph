#include <ranges>

#include "data/node.h"
#include "data/graph.h"
#include "utilities/utility.h"
#include "logics/controller.h"

namespace qtgraph {

DNode::DNode(DGraph *graph, std::optional<uint32_t> id)
    : Object(graph)
    , _canvasPosition{ QPointF(0, 0) }
    , _bIsSelected{ false }
    , _name{ QString("") }
    , _pins{ QMap<uint32_t, DPin*>() }
{
    _idGen = controller()->getIDGenerator();
    if (!id.has_value())
        _ID = _idGen->generate<DNode>();
    else
    {
        _ID = *id;
        _idGen->addTaken<DNode>(_ID);
    }  
}

DNode::~DNode() 
{
    _idGen->removeTaken<DNode>(ID());
}

void DNode::protocolize(protocol::Node *pNode) const
{
    *(pNode->mutable_canvas_position()) = convertTo_protocolPointF(_canvasPosition);
    pNode->set_id(_ID);
    pNode->set_is_selected(_bIsSelected);
    pNode->set_name(_name.toStdString());
    if (_typeID) pNode->set_type(*_typeID);
    std::ranges::for_each(_pins, [pNode](DPin *pin) {
        pin->protocolize(pNode->add_pins());
    });
}

void DNode::deprotocolize(const protocol::Node &pNode)
{
    setSelected(pNode.is_selected());
    setCanvasPosition(convertFrom_protocolPointF(pNode.canvas_position()));
    setName(QString::fromStdString(pNode.name()));

    std::ranges::for_each(pNode.pins(), [this](const protocol::Pin &pn){
        DPin *pin = new DPin(this);

        pin->deprotocolize(pn);
        addPin(pin);
    });
}

Controller *DNode::controller()
{
    return ((DGraph*)parent())->controller();
}

std::optional<DPin*> DNode::operator[](uint32_t id)
{
    if (!_pins.contains(id)) return {};
    else return _pins[id];
}

void DNode::setNodeTypeManager(NodeTypeManager *manager)
{
    _nodeTypeManager = manager;
}

void DNode::setPinTypeManager(PinTypeManager *manager)
{
    _pinTypeManager = manager;
}

void DNode::setPinConnection(uint32_t pinID, IPinData connectedPin)
{
    _pins[pinID]->addConnectedPin(connectedPin);
}

bool DNode::hasPinConnections() const
{
    return std::ranges::any_of(_pins, [&](DPin *pin){
        return pin->isConnected();
    });
}

const QMap<uint32_t, QVector<IPinData> > *DNode::getPinConnections() const
{
    QMap<uint32_t, QVector<IPinData> > *out = new QMap<uint32_t, QVector<IPinData>>();
    std::ranges::for_each(_pins, [&](DPin *pin){
        out->insert(pin->ID(), pin->getConnectedPins());
    });
    return out;
}

void DNode::removePinConnection(uint32_t pinID, uint32_t connectedPinID)
{
    _pins[pinID]->removeConnectedPinByID(connectedPinID);
}

void DNode::setSelected(bool b)
{
    _bIsSelected = b;
    emit isSelectedChanged(b, ID());
}

void DNode::onPinDestroyed(QObject *obj)
{
    emit pinRemoved(((DPin*)obj)->ID());
}

uint32_t DNode::addPin(DPin *pin)
{
    pin->setParent(this);
    _pins.insert(pin->ID(), pin);
    connect(pin, &DPin::destroyed, this, &DNode::onPinDestroyed);

    return pin->ID();
}

uint32_t DNode::addPin(QString text, EPinDirection direction, QColor color)
{
    DPin *newPin = new DPin(this);

    newPin->setColor(color);
    newPin->setText(text);
    newPin->setDirection(direction);
    return addPin(newPin);
}

}