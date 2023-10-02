#include <cstdint>

#include "logics/pin.h"

#include "logics/moc_pin.cpp"

namespace qtgraph {

LPin::LPin(uint32_t parentID)
{}

void LPin::protocolize(protocol::Pin *pPin) const
{
    pPin->set_type(_data.typeID);
}

}

