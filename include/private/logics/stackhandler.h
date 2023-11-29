#pragma once

#include <QObject>

#include "logics/stack.h"
#include "models/action.h"
#include "models/stackentry.h"

#include "qtgraph.h"

namespace qtgraph {

class LStackHandler : public QObject
{
public: 
    LStackHandler();
    
    void push(IStackEntry entry);
    IStackEntry pop();

    void setLength(int l);
    int getLength() const { return _actionStack.getLength(); }

    void clear() { _actionStack.clear(); _objectStack.clear(); }

private:
    LStack<IAction> _actionStack;
    LStack<QVector<void*>> _objectStack;

};

}