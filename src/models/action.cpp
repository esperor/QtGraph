#include "models/action.h"
#include "logics/graph.h"

namespace qtgraph 
{

IAction::IAction(EAction _code, 
    std::string _desc, 
    ActionFn _action,
    ActionFn _reverse,
    QVector<const void*> _objects)
        : code{ _code }
        , desc{ _desc }
        , action{ std::move(_action) }
        , reverse{ std::move(_reverse) }
        , objects{ std::move(_objects) }
{}

IAction::~IAction()
{
    std::ranges::for_each(objects, [](const void* ptr){
        delete ptr;
    });
}

void IAction::executeOn(LGraph *g) 
{ 
    action(g, &objects);
}

void IAction::reverseOn(LGraph *g) 
{ 
    reverse(g, &objects);
}

}