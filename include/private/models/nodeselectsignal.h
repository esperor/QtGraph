#pragma once

#include <QPointF>
#include <cstdint>

#include "qtgraph.h"

namespace qtgraph {

struct INodeSelectSignal
{
public:
    INodeSelectSignal() {}
    INodeSelectSignal(bool _selected, bool _bIsMultiSelectionModifierDown, uint32_t _nodeID)
        : selected{ _selected }
        , bIsMultiSelectionModifierDown{ _bIsMultiSelectionModifierDown }
        , nodeID{ _nodeID } {}

    bool selected;
    bool bIsMultiSelectionModifierDown;
    uint32_t nodeID;
};

}
