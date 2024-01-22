#include "logics/controller.h"
#include "utilities/constants.h"
#include "utilities/utility.h"
#include <ranges>
#include <algorithm>

namespace qtgraph {

Controller::Controller(QObject *parent)
    : QObject{ parent }
    , _IDgenerator{ new IDGenerator() }
    , _graph{ new DGraph(this) }
    , _factory{ new NodeFactory(this) }
    , _stack{ Stack<IAction*>() }
{
    connect(this, &Controller::nodeRemoved, this, &Controller::onNodeRemoved);
}

Controller::~Controller()
{
    delete _graph;
    delete _IDgenerator;
}


// ------------ SERIALIZATION -------------



bool Controller::serialize(std::fstream *output) const
{
    protocol::Graph pGraph;
    protocol::State *state = pGraph.mutable_state();

    auto ntm = _factory->getNodeTypeManager();
    auto ptm = _factory->getPinTypeManager();
    if (ntm) *(state->mutable_node_type_manager()) = ntm->toProtocolTypeManager();
    if (ptm) *(state->mutable_pin_type_manager()) = ptm->toProtocolTypeManager();

    _graph->protocolize(pGraph);

    if (_canvas)
    {
        *(pGraph.mutable_offset()) = convertTo_protocolPointF(_canvas->getOffset());
        pGraph.set_zoom(_canvas->getZoomMultiplier());
    }

    pGraph.SerializeToOstream(output);
    return true;
}

bool Controller::deserialize(std::fstream *input)
{
    protocol::Graph pGraph;
    pGraph.ParseFromIstream(input);

    protocol::State state = pGraph.state();
    if (state.has_node_type_manager())
        setNodeTypeManager(NodeTypeManager::fromProtocolTypeManager(state.node_type_manager()));
    if (state.has_pin_type_manager())
        setPinTypeManager(PinTypeManager::fromProtocolTypeManager(state.pin_type_manager()));

    clear();
    _graph->deprotocolize(pGraph);

    if (_canvas)
    {
        _canvas->setUpdatesEnabled(false);
        _canvas->clear();
        _canvas->setZoom(pGraph.zoom());
        _canvas->setOffset(convertFrom_protocolPointF(pGraph.offset()));
        _canvas->visualize();
        _canvas->setUpdatesEnabled(true);
    }
    
    return true;
}



// ------------- GENERAL -------------



WCanvas *Controller::createCanvas(QWidget *parent)
{
    _canvas = new WCanvas(parent);
    _canvas->_controller = this;
    _canvas->_graph = _graph;
    _canvas->visualize();
    _canvas->_bIsTypeBrowserBound = true;

    _wNodes = &(_canvas->_nodes);
    _selectedNodes = &(_canvas->_selectedNodes);

    _typeBrowser = new WTypeBrowser(_canvas);
    _typeBrowser->show();

    connect(_canvas, &Controller::destroyed, this, [this](QObject *obj){
        delete _typeBrowser;
    });
    connect(_typeBrowser, &WTypeBrowser::onMove, _canvas, &WCanvas::onTypeBrowserMove);

    return _canvas;
}

WTypeBrowser *Controller::exportTypeBrowser()
{
    _typeBrowser->hide();
    disconnect(_typeBrowser, &WTypeBrowser::onMove, _canvas, &WCanvas::onTypeBrowserMove);
    _canvas->_bIsTypeBrowserBound = false;
    return _typeBrowser;
}


void Controller::connectPins(IPinData in, IPinData out)
{   
    if (_graph->_connectedPins.contains(in, out)) return;

    QVector<const void*> objects = 
    { (void*)new IPinData(in) 
    , (void*)new IPinData(out)
    };

    IAction *action = new IAction(
        EAction::Connection,
        "Pin connection",
        [](DGraph *g, QVector<const void*> *o)
        {
            IPinData in = *(IPinData*)o->at(0);
            IPinData out = *(IPinData*)o->at(1);

            g->_connectedPins.insert(in, out);
            g->_nodes[out.nodeID]->setPinConnection(out.pinID, in);
            g->_nodes[in.nodeID]->setPinConnection(in.pinID, out);
        },
        [](DGraph *g, QVector<const void*> *o)
        {
            IPinData in = *(IPinData*)o->at(0);
            IPinData out = *(IPinData*)o->at(1);

            g->controller()->disconnectPins(in, out);
        },
        objects
    );

    processAction(action);   
}

void Controller::disconnectPins(IPinData in, IPinData out)
{
    if (_graph->_connectedPins.find(in, out) == _graph->_connectedPins.end()) 
        return;

    QVector<const void*> objects = 
    { (void*)new IPinData(in) 
    , (void*)new IPinData(out)
    };

    IAction *action = new IAction(
        EAction::Connection,
        "Pin connection",
        [](DGraph *g, QVector<const void*> *o)
        {
            IPinData in = *(IPinData*)o->at(0);
            IPinData out = *(IPinData*)o->at(1);

            g->_connectedPins.remove(in);

            g->_nodes[out.nodeID]->removePinConnection(out.pinID, in.pinID);
            g->_nodes[in.nodeID]->removePinConnection(in.pinID, out.pinID);
        },
        [](DGraph *g, QVector<const void*> *o)
        {
            IPinData in = *(IPinData*)o->at(0);
            IPinData out = *(IPinData*)o->at(1);

            g->controller()->connectPins(in, out);
        },
        objects
    );

    processAction(action);  
}

void Controller::addAction(IAction *action, bool execute)
{
    if (execute) executeAction(action);
    _stack.push(action);
}

void Controller::executeAction(IAction *action)
{
    if (_bIsRecording)
    {
        _bIsRecording = false;
        action->executeOn(_graph);
        _bIsRecording = true;
    }
    else action->executeOn(_graph);
}

void Controller::processAction(IAction *action)
{
    if (_bIsRecording)
        addAction(action);
    else
    {
        executeAction(action);
        delete action;
    }  
}

void Controller::undo(int num)
{
    if (num <= 0) return;
    num = std::min(num, _stack.size());

    _bIsRecording = false;

    while (num > 0)
    {
        IAction *action = _stack.pop();
        action->reverseOn(_graph);
        delete action;
        num--;
    }

    _bIsRecording = true;
}

void Controller::removeNode(uint32_t id)
{
    removeNodes({ id });
}

void Controller::removeNodes(QSet<uint32_t> &&ids)
{
    using node_id = uint32_t;
    using pin_id = uint32_t;


    auto *nodeIds = new QSet<node_id>(std::move(ids));

    auto values = nodeIds->values();
    for (auto it = values.begin(); it != values.end(); it++)
        if (!_graph->containsNode(*it)) nodeIds->remove(*it);

    if (nodeIds->empty()) return;

    // that is the map of nodes with their pin connections
    auto *map = new QMap< node_id, const QMap<pin_id, QVector<IPinData>>* >();
    auto *nodes = new QVector<DNode*>();
    for (auto nodeId : *nodeIds)
    {
        DNode *node = _graph->nodes()[nodeId];
        nodes->append(node);
        map->insert(nodeId, node->getPinConnections());
    }

    QVector<const void*> objects = 
    { (void*)nodes
    , (void*)map
    };

    IAction *action = new IAction(
        EAction::Deletion,
        "Node deletion",
        [](DGraph *g, QVector<const void*> *o)
        {
            auto nodes = (QVector<DNode*>*)o->at(0);
            auto map = (QMap< node_id, const QMap<pin_id, QVector<IPinData>>* >*)o->at(1);

            bool nodesEmpty = nodes->empty();

            for (auto pair : map->asKeyValueRange())
            {   
                node_id nodeId = pair.first;
                const QMap<pin_id, QVector<IPinData> >* connections = pair.second;

                DNode *node = g->nodes()[nodeId];

                if (nodesEmpty) nodes->append(node);
                
                if (!connections->empty())
                {
                    std::ranges::for_each(connections->asKeyValueRange(), [&](std::pair<const pin_id&, const QVector<IPinData>&> pair){
                        pin_id id = pair.first;
                        IPinData firstPin = node->pin(id).value();

                        std::ranges::for_each(pair.second, [&](IPinData secondPin){
                            if (firstPin.pinDirection == EPinDirection::In)
                                g->controller()->disconnectPins(firstPin, secondPin);
                            else
                                g->controller()->disconnectPins(secondPin, firstPin);
                        });
                    });
                }
                g->nodes().remove(nodeId);
            }
        },
        [](DGraph *g, QVector<const void*> *o)
        {
            auto nodes = (QVector<DNode*>*)o->at(0);
            auto map = (QMap< node_id, const QMap<pin_id, QVector<IPinData>>* >*)o->at(1);

            for (auto it_node = nodes->begin(); it_node != nodes->end(); )
            {   
                DNode *node = *it_node;
                node_id nodeId = node->ID();
                const QMap<pin_id, QVector<IPinData> >* connections = map->value(nodeId);

                g->nodes().insert(nodeId, node);
                it_node = nodes->erase(it_node);
                
                if (!connections->empty())
                {
                    std::ranges::for_each(connections->asKeyValueRange(), [&](std::pair<const pin_id&, const QVector<IPinData>&> pair){
                        pin_id id = pair.first;
                        IPinData firstPin = node->pin(id).value();

                        std::ranges::for_each(pair.second, [&](IPinData secondPin){
                            if (firstPin.pinDirection == EPinDirection::In)
                                g->controller()->connectPins(firstPin, secondPin);
                            else
                                g->controller()->connectPins(secondPin, firstPin);
                        });
                    });
                }
            }
        },
        objects
    );

    processAction(action);   
}

void Controller::deselectAll()
{
    if (_selectedNodes->empty()) return;

    QVector<const void*> objects = 
    { (void*)new QSet(*_selectedNodes)
    };

    IAction *action = new IAction(
        EAction::Selection,
        "Node selection",
        [](DGraph *g, QVector<const void*> *o)
        {
            QSet<uint32_t>* selectedNodes = (QSet<uint32_t>*)(o->at(0));

            std::ranges::for_each(*selectedNodes, [&](uint32_t id){
                g->nodes()[id]->setSelected(false);
            });
        },
        [](DGraph *g, QVector<const void*> *o)
        {
            QSet<uint32_t>* selectedNodes = (QSet<uint32_t>*)(o->at(0));

            std::ranges::for_each(*selectedNodes, [&](uint32_t id){
                g->nodes()[id]->setSelected(true);
            });
        },
        objects
    );

    processAction(action);
}

void Controller::clear()
{
    QList<uint32_t> l = _graph->_nodes.keys();
    removeNodes(QSet(l.begin(), l.end()));
}

DNode *Controller::addNode(QPoint canvasPosition, int typeID)
{
    DNode *node = _factory->makeNodeAndPinsOfType(typeID, _graph);
    node->setCanvasPosition(canvasPosition);
    return addNode(node);
}

DNode *Controller::addNode(QPoint canvasPosition, QString name)
{
    DNode *node = new DNode(_graph);
    node->setCanvasPosition(canvasPosition);
    node->setName(name);
    return addNode(node);
}

DNode *Controller::addNode(DNode *node)
{
    connect(node, &DNode::isSelectedChanged, this, &Controller::onIsSelectedChanged);
    DNode *nd = _graph->addNode(node);
    if (_canvas) _canvas->visualize();
    return nd;
}

void Controller::processNodeSelectSignal(INodeSelectSignal signal)
{
    if (!signal.selected.has_value() && _selectedNodes->size() == 1) return;

    QVector<const void*> objects = 
    { (void*)new uint32_t(signal.nodeID)
    , (void*)new std::optional<bool>(signal.selected)
    , (void*)new bool(signal.bIsMultiSelectionModifierDown)
    , (void*)new QSet(*_selectedNodes)
    };

    IAction *action = new IAction(
        EAction::Selection,
        "Node selection",
        [](DGraph *g, QVector<const void*> *o)
        {
            uint32_t nodeID = *(uint32_t*)(o->at(0));
            std::optional<bool> selected = *(std::optional<bool>*)(o->at(1));
            bool isMultiSelectionModifierDown = *(bool*)(o->at(2));
            QSet<uint32_t>* selectedNodes = (QSet<uint32_t>*)(o->at(3));

            if (selected.has_value())
                g->nodes()[nodeID]->setSelected(*selected);

            if (isMultiSelectionModifierDown) return;
            std::ranges::for_each(*selectedNodes, [&](uint32_t id){
                if (id == nodeID) return;
                g->nodes()[id]->setSelected(false);
            });
        },
        [](DGraph *g, QVector<const void*> *o)
        {
            uint32_t nodeID = *(uint32_t*)(o->at(0));
            std::optional<bool> selected = *(std::optional<bool>*)(o->at(1));
            bool isMultiSelectionModifierDown = *(bool*)(o->at(2));
            QSet<uint32_t>* selectedNodes = (QSet<uint32_t>*)(o->at(3));

            if (selected.has_value())
                g->nodes()[nodeID]->setSelected(!(*selected));

            if (isMultiSelectionModifierDown) return;
            std::ranges::for_each(*selectedNodes, [&](uint32_t id){
                if (id == nodeID) return;
                g->nodes()[id]->setSelected(true);
            });
        },
        objects
    );

    processAction(action);
}

void Controller::onIsSelectedChanged(bool selected, uint32_t nodeID)
{
    switch (selected)
    {
    case true: _selectedNodes->insert(nodeID); 
        return;
    case false: _selectedNodes->remove(nodeID); 
        return;
    }
}

void Controller::onNodeRemoved(uint32_t id)
{
    WANode *ptr = _wNodes->value(id);
    _wNodes->remove(id);
    if (_selectedNodes->contains(id)) 
        _selectedNodes->remove(id);

    delete ptr;
}

}