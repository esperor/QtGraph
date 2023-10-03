#include <cstdint>

#include "logics/pin.h"
#include "utilities/utility.h"

#include "logics/moc_pin.cpp"

namespace qtgraph {

LPin::LPin(uint32_t parentID)
{}

void LPin::protocolize(protocol::Pin *pPin) const
{
    *(pPin->mutable_color()) = convertTo_protocolColor(_color);
    pPin->set_text(_text.toStdString());
    pPin->set_direction((protocol::PinDirection)_data.pinDirection);
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


IPinData LPin::getData() const
{
    return _data;
}

void LPin::addConnectedPin(IPinData pin)
{
    if (pin.pinDirection == _data.pinDirection)
        throw std::invalid_argument("LPin::addConnectedPin - pin with the same direction passed as the argument.");
    _connectedPins.insert(pin.pinID, pin);
    _bIsConnected = true;
}

void LPin::removeConnectedPinByID(uint32_t ID)
{
    if (_connectedPins.contains(ID))
    {
        _connectedPins.remove(ID);
        _bIsConnected = !(_connectedPins.empty());
    }
}

}

