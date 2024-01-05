#pragma once

#include <functional>
#include <string>
#include <QVector>
#include "qtgraph.h"

namespace qtgraph {

class LGraph;

using ActionFn = std::function<void(LGraph*, QVector<void*>*)>;

enum class EAction
{
    None,
    Deletion,
    Addition,
    Connection,
    Disconnection,
    Renaming,
    Moving,
    Conversion,
    Unknown,
};

struct IAction
{
public:
    IAction(EAction _code, 
            std::string _desc, 
            ActionFn _action, 
            ActionFn _reverse, 
            QVector<void*> &&_objects = {});

    void executeOn(LGraph *g);
    void reverseOn(LGraph *g);

    ActionFn action = {};
    ActionFn reverse = {};
    std::string desc = "";
    EAction code = EAction::Unknown;
    QVector<void*> objects;
};

}