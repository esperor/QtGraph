#include "DataClasses/pindragsignal.h"

namespace qtgraph {

PinDragSignal::PinDragSignal() {}
PinDragSignal::PinDragSignal(IPinData source, PinDragSignalType type)
    : _source{ source }, _type{ type } {}

PinDragMoveSignal::PinDragMoveSignal() {}
PinDragMoveSignal::PinDragMoveSignal(IPinData source, PinDragSignalType type, QPointF mousePosition)
    : PinDragSignal(source, type), _mousePosition{ mousePosition } {}

}
