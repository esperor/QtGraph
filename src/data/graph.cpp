#include <ranges>

#include "data/graph.h"
#include "logics/controller.h"

namespace qtgraph {

DGraph::DGraph(QObject *parent)
    : Object(parent)
    , _nodes{ QMap<uint32_t, DNode*>() }
    , _connectedPins{ QMultiMap<IPinData, IPinData>() }   
{
    _ID = ID::generate<DGraph>();

}

DGraph::~DGraph()
{
    ID::removeTaken<DGraph>(ID());
    
}


// ------------------------ SERIALIZATION --------------------------



bool DGraph::protocolize(protocol::Graph &graph) const
{
    protocol::State *state = graph.mutable_state();
    
    std::ranges::for_each(_nodes, [state](DNode *node) {
        node->protocolize(state->add_nodes());
    });

    writeStructure(graph.mutable_structure());
    return true;
}

bool DGraph::deprotocolize(protocol::Graph &graph)
{
    protocol::State state = graph.state();

    std::ranges::for_each(state.nodes(), [this](const protocol::Node &nd){
        DNode *node;
        if (nd.has_type()) 
            node = controller()->getFactory()->makeNodeOfType(nd.type(), this);
        else
            node = new DNode(this);

        node->deprotocolize(nd);
        addNode(node);
    });

    // structure
    readStructure(graph.structure());
    return true;
}

bool DGraph::writeStructure(protocol::Structure *structure) const
{
    try 
    {
        *structure = getStructure(); 
    } 
    catch (std::exception *e)
    {
        qDebug() << e->what();
        return false;
    }
    
    return true;
}

protocol::Structure DGraph::getStructure() const
{
    protocol::Structure st;
    auto edges = st.mutable_edges();
    std::ranges::for_each(_connectedPins.keys(), [this, edges](IPinData key){
        QList<IPinData> values = this->_connectedPins.values(key);

        protocol::IntArray arr;
        std::ranges::for_each(values, [&arr](IPinData &value){ arr.add_elements(value.pinID); });

        (*edges)[key.pinID] = arr;
    });

    return st;
}

// this function can perhaps be optimized 
// (but probably not without pain in the ass)
bool DGraph::readStructure(const protocol::Structure &structure)
{
    std::map<uint32_t, uint32_t> pin_node_map = {};
    int l = _nodes.size();
    std::ranges::for_each(_nodes, [this, &pin_node_map](DNode *node){
        std::ranges::for_each(node->getPinIDs(), [&node, &pin_node_map](uint32_t id) {
            pin_node_map.insert({id, node->ID()});
        });
    });
    std::ranges::for_each(structure.edges(), [this, &pin_node_map](std::pair<uint32_t, protocol::IntArray> pair){
        IPinData first = _nodes[pin_node_map[pair.first]]->pin(pair.first).value()->getData();

        std::ranges::for_each(*pair.second.mutable_elements(), 
        [this, &pin_node_map, &first](uint32_t &id){
            IPinData second = _nodes[pin_node_map[id]]->pin(id).value()->getData();
            _nodes[second.nodeID]->setPinConnection(second.pinID, first);  
            _nodes[first.nodeID]->setPinConnection(first.pinID, second);
            _connectedPins.insert(first, second);
        });
    });
    return true;
}


// ---------------------- GENERAL FUNCTIONS ---------------------------


QString DGraph::getPinText(uint32_t nodeID, uint32_t pinID) const
{
    return _nodes[nodeID]->pin(pinID).value()->getText();
}
QString DGraph::getNodeName(uint32_t nodeID) const
{
    return _nodes[nodeID]->getName();
}

DNode *DGraph::addNode(DNode *node)
{
    node->setParent(this);
    uint32_t id = node->ID();
    _nodes.insert(id, node);
    return _nodes[id];
}
    

}