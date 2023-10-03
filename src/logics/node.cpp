#include <ranges>

#include "logics/node.h"
#include "logics/graph.h"

namespace qtgraph {

LNode::LNode(LGraph *graph)
    : _ID{ graph->newID() }
    , _canvasPosition{ QPointF(0, 0) }
    , _bIsSelected{ false }
    , _name{ QString("") }
    , _pins{ QMap<uint32_t, QSharedPointer<LPin>>() }

{}

LNode::~LNode()
{}

void LNode::setPinConnection(uint32_t pinID, IPinData connectedPin)
{
    _pins[pinID]->addConnectedPin(connectedPin);
}

bool LNode::hasPinConnections() const
{
    return std::ranges::any_of(_pins, [&](const QSharedPointer<LPin> &pin){
        return pin->isConnected();
    });
}

const QSharedPointer< QMap<uint32_t, QVector<IPinData> > > &LNode::getPinConnections() const
{
    QSharedPointer<QMap<uint32_t, QVector<IPinData> >> out(new QMap<uint32_t, QVector<IPinData> >());
    std::ranges::for_each(_pins, [&](const QSharedPointer<LPin> &pin){
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
    _pins.insert(pin->ID(), QSharedPointer<LPin>(pin));
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