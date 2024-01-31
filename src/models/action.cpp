#include "models/action.h"
#include "data/graph.h"

namespace qtgraph 
{

IAction::IAction(EAction _code, 
    std::string _desc, 
    ActionFn _action,
    ActionFn _reverse,
    DestructorFn _destructor,
    QVector<const void*> _objects)
        : code{ _code }
        , desc{ _desc }
        , action{ std::move(_action) }
        , reverse{ std::move(_reverse) }
        , destructor{ std::move(_destructor) }
        , objects{ std::move(_objects) }
{}

IAction::~IAction()
{
    destructor(&objects);
}

void IAction::executeOn(DGraph *g) 
{ 
    action(g, &objects);
    g->registerAction(code);
}

void IAction::reverseOn(DGraph *g) 
{ 
    reverse(g, &objects);
    g->registerAction(code);
}

}