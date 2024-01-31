#include "models/pindata.h"
#include "utilities/constants.h"
#include "data/pin.h"

namespace qtgraph {

IPinData::IPinData(const DPin *pin)
    : pinDirection{ pin->getData().pinDirection }
    , nodeID{ pin->getData().nodeID }
    , pinID{ pin->getData().pinID }
{}

IPinData::IPinData(const IPinData &other)
{ *this = other; }

IPinData::IPinData(EPinDirection _direction, uint32_t _nodeID, uint32_t _pinID, uint32_t _typeID)
    : pinDirection{ _direction }, nodeID{ _nodeID }, pinID{ _pinID }, typeID{ _typeID }
{}

IPinData::~IPinData() {}

void IPinData::operator=(const IPinData &other)
{
    pinDirection = other.pinDirection;
    nodeID = other.nodeID;
    pinID = other.pinID;
    typeID = other.typeID;
}

QByteArray IPinData::toByteArray() const
{
    QByteArray output;
    output.append(QByteArray::number(static_cast<int>(pinDirection == EPinDirection::In)));
    output.append(c_dataSeparator);

    output.append(QByteArray::number(nodeID))           .append(c_dataSeparator);
    output.append(QByteArray::number(pinID))            .append(c_dataSeparator);
    output.append(QByteArray::number(typeID));
    return output;
}

IPinData IPinData::fromByteArray(const QByteArray &byteArray)
{
    unsigned short i = 0;

    IPinData data;
    QList<QByteArray> arrays = byteArray.split(c_dataSeparator);
    data.pinDirection = static_cast<bool>(arrays[i++].toInt()) ?
                            EPinDirection::In : EPinDirection::Out;

    data.nodeID = arrays[i++].toInt();
    data.pinID = arrays[i++].toInt();
    data.typeID = arrays[i++].toInt();

    return data;
}

QDebug &operator<<(QDebug &debug, const IPinData &obj)
{
    bool temp = debug.autoInsertSpaces();
    debug.setAutoInsertSpaces(false);

    debug << (obj.pinDirection == EPinDirection::In ? "In-" : "Out-")
          << "Pin(NodeID: " << obj.nodeID <<  ", pinID: " << obj.pinID << ")";

    debug.setAutoInsertSpaces(temp);
    return debug;
}

QDataStream &operator<<(QDataStream &out, const IPinData &obj)
{
    out << (obj.pinDirection == EPinDirection::In ? "In-" : "Out-")
        << "Pin(NodeID: " << obj.nodeID <<  ", pinID: " << obj.pinID << ")";

    return out;
}

bool operator<(const IPinData &first, const IPinData &second)
{
    return first.pinID < second.pinID;
}

bool operator>(const IPinData &first, const IPinData &second)
{
    return first.pinID > second.pinID;
}

bool operator==(const IPinData &first, const IPinData &second)
{
    return first.nodeID == second.nodeID
           && first.pinID == second.pinID
           && first.pinDirection == second.pinDirection;
}

}
