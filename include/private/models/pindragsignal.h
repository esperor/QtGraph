#pragma once

#include <QPointF>

#include "qtgraph.h"
#include "QtGraph/DataClasses/pindata.h"

namespace qtgraph {

enum class PinDragSignalType
{
    Enter,
    Leave,
    Start,
    End
};

struct PinDragSignal
{
public:
    PinDragSignal();
    PinDragSignal(IPinData source, PinDragSignalType type);

    IPinData &source() { return _source; }
    PinDragSignalType &type() { return _type; }

private:
    IPinData _source;
    PinDragSignalType _type;
};

struct PinDragMoveSignal : public PinDragSignal
{
public:
    PinDragMoveSignal();
    PinDragMoveSignal(IPinData source, PinDragSignalType type, QPointF mousePosition);

    QPointF &mousePosition() { return _mousePosition; }

private:
    QPointF _mousePosition;
};

}
