#include "models/selectionrectprocess.h"
#include "logics/controller.h"

namespace qtgraph {

SelectionRectProcess::SelectionRectProcess(Controller *_c) 
    : c{ _c }
    , selected( new QSet<uint32_t>() )
    , initialSelected( *_c->_selectedNodes) {}

SelectionRectProcess::~SelectionRectProcess()
{
    QVector<const void*> objects = 
    { (void*)selected };

    IAction *action = new IAction(
        EAction::Selection,
        "Node selection",
        [](DGraph *g, QVector<const void*> *o)
        {
            QSet<uint32_t>* selectedNodes = (QSet<uint32_t>*)(o->at(0));

            std::ranges::for_each(*selectedNodes, [&](uint32_t id){
                g->nodes()[id]->setSelected(true);
            });
        },
        [](DGraph *g, QVector<const void*> *o)
        {
            QSet<uint32_t>* selectedNodes = (QSet<uint32_t>*)(o->at(0));

            std::ranges::for_each(*selectedNodes, [&](uint32_t id){
                g->nodes()[id]->setSelected(false);
            });
        },
        objects
    );

    c->addAction(action, false);
}

void SelectionRectProcess::addSelected(uint32_t nodeID)
{
    if (initialSelected.contains(nodeID)) return;
    c->_graph->nodes()[nodeID]->setSelected(true);
    selected->insert(nodeID);
}

void SelectionRectProcess::removeSelected(uint32_t nodeID)
{
    if (initialSelected.contains(nodeID)) return;
    c->_graph->nodes()[nodeID]->setSelected(false);
    selected->remove(nodeID);
}

}