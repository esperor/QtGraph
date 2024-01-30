#pragma once

#include <functional>
#include <string>
#include <QVector>
#include "qtgraph.h"

namespace qtgraph {

class DGraph;

#define ActionFn std::function<void(DGraph*, QVector<const void*>*)>
#define DestructorFn std::function<void(QVector<const void*>*)>

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
        , DestructorFn _destructor
        , QVector<const void*> _objects = {}
    );

    ~IAction();

    void executeOn(DGraph *g);
    void reverseOn(DGraph *g);

    ActionFn action = {};
    ActionFn reverse = {};
    DestructorFn destructor = {};
    std::string desc = "";
    EAction code = EAction::Unknown;
    QVector<const void*> objects;
};

}