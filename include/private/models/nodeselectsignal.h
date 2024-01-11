#pragma once

#include <QPointF>
#include <cstdint>

#include "qtgraph.h"

namespace qtgraph {

struct INodeSelectSignal
{
public:
    INodeSelectSignal() {}
    INodeSelectSignal(std::optional<bool> _selected, bool _bIsMultiSelectionModifierDown, uint32_t _nodeID)
        : selected{ _selected }
        , bIsMultiSelectionModifierDown{ _bIsMultiSelectionModifierDown }
        , nodeID{ _nodeID } {}

    // nullopt means that state doesn't change, but signal must be emitted
    // existing use-case for that is when the node is selected and is being 
    // clicked on again, thus all other selected nodes must be deselected
    // but the node shouldn't be deselected by undoing this action
    std::optional<bool> selected;
    bool bIsMultiSelectionModifierDown;
    uint32_t nodeID;
};

}
