#include "logics/stackhandler.h"

namespace qtgraph {

LStackHandler::LStackHandler() {}

void LStackHandler::push(IStackEntry entry)
{
    _actionStack.push(entry.action);
    _objectStack.push(entry.objects);
}

IStackEntry LStackHandler::pop()
{
    return IStackEntry(_actionStack.pop(), _objectStack.pop());
}

void LStackHandler::setLength(int l)
{
    _actionStack.setLength(l);
    _objectStack.setLength(l);
}

}