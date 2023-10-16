#include <cstdint>

#include "logics/pin.h"
#include "utilities/utility.h"
#include "logics/node.h"

#include "logics/moc_pin.cpp"

namespace qtgraph
{

LPin::LPin(LNode *parent)
    : _data{ IPinData(EPinDirection::In, parent->newID(), parent->ID()) }
    , _text{ QString("") }
    , _color{ QColor(0, 0, 0) }
    , _connectedPins{ QMap<uint32_t, IPinData>() }
{
}

void LPin::protocolize(protocol::Pin *pPin) const
{
    *(pPin->mutable_color()) = convertTo_protocolColor(_color);
    pPin->set_text(_text.toStdString());
    pPin->set_direction((protocol::EPinDirection)_data.pinDirection);
    pPin->set_id(_data.pinID);

    // TODO: wrap IPinData.typeID with std::optional
    pPin->set_type(_data.typeID);
}

void LPin::deprotocolize(const protocol::Pin &pPin)
{
    setColor(convertFrom_protocolColor(pPin.color()));
    setText(QString::fromStdString(pPin.text()));
    setDirection((EPinDirection)pPin.direction());
}

void LPin::addConnectedPin(IPinData pin)
{
    if (pin.pinDirection == _data.pinDirection)
        throw std::invalid_argument("LPin::addConnectedPin - pin with the same direction passed as the argument.");
    _connectedPins.insert(pin.pinID, pin);
}

void LPin::removeConnectedPinByID(uint32_t ID)
{
    if (_connectedPins.contains(ID))
        _connectedPins.remove(ID);
}

}
