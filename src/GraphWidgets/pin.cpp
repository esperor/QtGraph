#include "GraphWidgets/pin.h"

#include "GraphWidgets/moc_pin.cpp"

namespace GraphLib {

Pin::Pin(BaseNode *parent)
    : AbstractPin{ parent }
{}

void Pin::protocolize(protocol::Pin *pPin) const
{
    AbstractPin::protocolize(pPin);
    pPin->set_type(_data.typeID);
}

}

