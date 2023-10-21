#include "models/pindragsignal.h"

namespace qtgraph {

IPinDragSignal::IPinDragSignal() {}
IPinDragSignal::IPinDragSignal(IPinData source, EPinDragSignalType type)
    : _source{ source }, _type{ type } {}

IPinDragMoveSignal::IPinDragMoveSignal() {}
IPinDragMoveSignal::IPinDragMoveSignal(IPinData source, EPinDragSignalType type, QPointF mousePosition)
    : IPinDragSignal(source, type), _mousePosition{ mousePosition } {}

}
