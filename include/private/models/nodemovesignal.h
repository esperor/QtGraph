#pragma once

#include <QPointF>
#include <cstdint>

#include "qtgraph.h"

namespace qtgraph {

struct INodeMoveSignal
{
public:
    INodeMoveSignal() {}
    INodeMoveSignal(QPointF _newPosition, uint32_t _nodeID)
        : newPosition{ _newPosition }
        , nodeID{ _nodeID } {}

    QPointF newPosition;
    uint32_t nodeID;
};


}