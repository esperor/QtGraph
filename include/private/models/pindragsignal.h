#pragma once

#include <QPointF>

#include "qtgraph.h"
#include "models/pindata.h"

namespace qtgraph {

enum class EPinDragSignalType
{
    Enter,
    Leave,
    Start,
    End
};

struct IPinDragSignal
{
public:
    IPinDragSignal();
    IPinDragSignal(IPinData source, EPinDragSignalType type);

    IPinData &source() { return _source; }
    EPinDragSignalType &type() { return _type; }

private:
    IPinData _source;
    EPinDragSignalType _type;
};

struct IPinDragMoveSignal : public IPinDragSignal
{
public:
    IPinDragMoveSignal();
    IPinDragMoveSignal(IPinData source, EPinDragSignalType type, QPointF mousePosition);

    QPointF &mousePosition() { return _mousePosition; }

private:
    QPointF _mousePosition;
};

}
