#pragma once

#include <QObject>

#include "models/pindata.h"
#include "logics/node.h"

#include "pin.pb.h"

namespace qtgraph {

class LPin : public QObject
{
    Q_OBJECT

public:
    LPin(uint32_t parentID);

    void protocolize(protocol::Pin *pPin) const;

    IPinData getData() const { return _data; }

    void setParentID(uint32_t newParentID) { _data.nodeID = newParentID; }
    void setTypeID(uint32_t id) { _data.typeID = id; }
    void setID(uint32_t id) { _data.pinID = id; }
    void setDirection(EPinDirection dir) { _data.pinDirection = dir; }

private:
    IPinData _data;

};

}