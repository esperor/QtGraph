#pragma once

#include <functional>
#include <string>
#include <QVector>
#include "qtgraph.h"

namespace qtgraph {

class LGraph;

#define ActionFn std::function<void(LGraph*, QVector<const void*>*)>

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
    Selection,
    Unknown,
};

struct IAction
{
public:
    IAction() {}
    IAction(EAction _code
        , std::string _desc
        , ActionFn _action
        , ActionFn _reverse
        , QVector<const void*> _objects = {}
    );

    ~IAction();

    void executeOn(LGraph *g);
    void reverseOn(LGraph *g);

    ActionFn action = {};
    ActionFn reverse = {};
    std::string desc = "";
    EAction code = EAction::Unknown;
    QVector<const void*> objects;
};

}