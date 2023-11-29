#pragma once

#include <QVector>
#include "action.h"

#include "qtgraph.h"

namespace qtgraph {

struct IStackEntry 
{
public:
    IStackEntry(IAction _action, const QVector<void*> &_objects) 
        : action{ _action }, objects{ _objects } {}

    IAction action;
    QVector<void*> objects;
};

}