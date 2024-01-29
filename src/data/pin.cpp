#include <cstdint>

#include "data/pin.h"
#include "utilities/utility.h"
#include "data/node.h"
#include "logics/controller.h"

#include "data/moc_pin.cpp"

namespace qtgraph
{

DPin::DPin(DNode *parent, std::optional<uint32_t> id)
    : Object(parent)
    , _text{ QString("") }
    , _color{ QColor(0, 0, 0) }
    , _connectedPins{ QMap<uint32_t, IPinData>() }
{
    uint32_t _id;
    if (!id.has_value())
        _id = ID::generate<DPin>();
    else
    {
        _id = *id;
        ID::addTaken<DPin>(_id);
    }  

    _data = IPinData(EPinDirection::In, parent->ID(), _id);
}

DPin::~DPin()
{
    ID::removeTaken<DPin>(ID());
}

void DPin::protocolize(protocol::Pin *pPin) const
{
    *(pPin->mutable_color()) = convertTo_protocolColor(_color);
    pPin->set_text(_text.toStdString());
    pPin->set_direction((protocol::EPinDirection)_data.pinDirection);
    pPin->set_id(_data.pinID);

    // TODO: wrap IPinData.typeID with std::optional
    pPin->set_type(_data.typeID);
}

void DPin::deprotocolize(const protocol::Pin &pPin)
{
    setColor(convertFrom_protocolColor(pPin.color()));
    setText(QString::fromStdString(pPin.text()));
    setDirection((EPinDirection)pPin.direction());
}

void DPin::addConnectedPin(IPinData pin)
{
    if (pin.pinDirection == _data.pinDirection)
        throw std::invalid_argument("DPin::addConnectedPin - pin with the same direction passed as the argument.");
    _connectedPins.insert(pin.pinID, pin);
}

void DPin::removeConnectedPinByID(uint32_t ID)
{
    if (_connectedPins.contains(ID))
        _connectedPins.remove(ID);
}

Controller *DPin::controller()
{ 
    return ((DNode*)parent())->controller(); 
}

}
