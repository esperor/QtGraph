#include <ranges>

#include "logics/node.h"
#include "logics/graph.h"
#include "utilities/utility.h"

namespace qtgraph {

LNode::LNode(LGraph *graph)
    : _ID{ graph->newID() }
    , _canvasPosition{ QPointF(0, 0) }
    , _bIsSelected{ false }
    , _name{ QString("") }
    , _pins{ QMap<uint32_t, LPin*>() }

{}

LNode::~LNode()
{}

void LNode::protocolize(protocol::Node *pNode) const
{
    *(pNode->mutable_canvas_position()) = convertTo_protocolPointF(_canvasPosition);
    pNode->set_id(_ID);
    pNode->set_is_selected(_bIsSelected);
    pNode->set_name(_name.toStdString());
    std::ranges::for_each(_pins, [pNode](LPin *pin) {
        pin->protocolize(pNode->add_pins());
    });
}

void LNode::deprotocolize(const protocol::Node &pNode)
{
    setSelected(pNode.is_selected());
    setCanvasPosition(convertFrom_protocolPointF(pNode.canvas_position()));
    setName(QString::fromStdString(pNode.name()));

    std::ranges::for_each(pNode.pins(), [this](const protocol::Pin &pn){
        LPin *pin = new LPin(this);

        // pin automatically gets new id as it's created
        // that's what we don't need in case of deserialization
        _IDgenerator.removeTaken(pin->ID());
        pin->setID(pn.id());
        _IDgenerator.addTaken(pin->ID());

        pin->deprotocolize(pn);
        addPin(pin);
    });
}

std::optional<LPin*> LNode::operator[](uint32_t id)
{
    if (!_pins.contains(id)) return {};
    else return _pins[id];
}

void LNode::setNodeTypeManager(NodeTypeManager *manager)
{
    _nodeTypeManager = manager;
}

void LNode::setPinTypeManager(PinTypeManager *manager)
{
    _pinTypeManager = manager;
}

void LNode::setPinConnection(uint32_t pinID, IPinData connectedPin)
{
    _pins[pinID]->addConnectedPin(connectedPin);
}

bool LNode::hasPinConnections() const
{
    return std::ranges::any_of(_pins, [&](LPin *pin){
        return pin->isConnected();
    });
}

const QMap<uint32_t, QVector<IPinData> > *LNode::getPinConnections() const
{
    QMap<uint32_t, QVector<IPinData> > *out = new QMap<uint32_t, QVector<IPinData>>();
    std::ranges::for_each(_pins, [&](LPin *pin){
        out->insert(pin->ID(), pin->getConnectedPins());
    });
    return out;
}

void LNode::removePinConnection(uint32_t pinID, uint32_t connectedPinID)
{
    _pins[pinID]->removeConnectedPinByID(connectedPinID);
}


void LNode::onPinDestroyed(QObject *obj)
{
    if (obj == nullptr) qDebug() << "Pointer to destroyed pin is nullptr";

    _IDgenerator.removeTaken( ((LPin*)obj)->ID() );
}

uint32_t LNode::addPin(LPin *pin)
{
    _pins.insert(pin->ID(), pin);
    connect(pin, &LPin::destroyed, this, &LNode::onPinDestroyed);

    return pin->ID();
}

uint32_t LNode::addPin(QString text, EPinDirection direction, QColor color)
{
    LPin *newPin = new LPin(this);

    newPin->setColor(color);
    newPin->setText(text);
    newPin->setDirection(direction);
    return addPin(newPin);
}

IDGenerator LNode::_IDgenerator = IDGenerator();

}