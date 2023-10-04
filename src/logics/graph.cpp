#include <ranges>

#include "logics/graph.h"

namespace qtgraph {

LGraph::LGraph()
    : _connectedPins{ QMultiMap<IPinData, IPinData>() }

{}

LGraph::~LGraph()
{}


// ------------------------ SERIALIZATION --------------------------


bool LGraph::serialize(std::fstream *output) const
{
    protocol::Graph graph;
    protocolize(graph);
    graph.SerializeToOstream(output);
    return true;
}

bool LGraph::protocolize(protocol::Graph &graph) const
{
    protocol::State *state = graph.mutable_state();
    *(state->mutable_node_type_manager()) = _nodeTypeManager.toProtocolTypeManager();
    *(state->mutable_pin_type_manager()) = _pinTypeManager.toProtocolTypeManager();

    std::ranges::for_each(_nodes, [state](QSharedPointer<LNode> node) {
        node->protocolize(state->add_nodes());
    });

    writeStructure(graph.mutable_structure());
    return true;
}

bool LGraph::deserialize(std::fstream *input)
{
    protocol::Graph graph;
    graph.ParseFromIstream(input);
    deprotocolize(graph);
    return true;
}

bool LGraph::deprotocolize(protocol::Graph &graph)
{
    protocol::State state = graph.state();
    setNodeTypeManager(NodeTypeManager::fromProtocolTypeManager(state.node_type_manager()));
    setPinTypeManager(PinTypeManager::fromProtocolTypeManager(state.pin_type_manager()));

    std::ranges::for_each(state.nodes(), [this](const protocol::Node &nd){
        LNode *node;
        if (nd.has_type()) 
            node = _factory->makeNodeOfType(nd.type(), this);
        else
            node = new LNode(this);

        // node automatically gets new id as it's created
        // that's what we don't need in case of deserialization
        _IDgenerator.removeTaken(node->ID());
        node->setID(nd.id());
        _IDgenerator.addTaken(node->ID());

        node->deprotocolize(nd);
        addNode(node);
    });

    // structure
    readStructure(graph.structure());
}

bool LGraph::writeStructure(protocol::Structure *structure) const
{
    auto *edges = structure->mutable_edges();
    std::ranges::for_each(_connectedPins.keys(), [this, edges](IPinData key){
        QList<IPinData> values = this->_connectedPins.values(key);

        protocol::IntArray arr;
        std::ranges::for_each(values, [&arr](IPinData &value){ arr.add_elements(value.pinID); });

        (*edges)[key.pinID] = arr;
    });
    return true;
}

// this function can perhaps be optimized 
// (but probably not without pain in the ass)
bool LGraph::readStructure(const protocol::Structure &structure)
{
    std::map<uint32_t, uint32_t> pin_node_map = {};
    int l = _nodes.size();
    std::ranges::for_each(_nodes, [this, &pin_node_map](const QSharedPointer<WANode> node){
        std::ranges::for_each(node->getPinIDs(), [&node, &pin_node_map](uint32_t id) {
            pin_node_map.insert({id, node->ID()});
        });
    });
    std::ranges::for_each(structure.edges(), [this, &pin_node_map](std::pair<uint32_t, protocol::IntArray> pair){
        IPinData first = _nodes[pin_node_map[pair.first]]->getPinByID(pair.first)->getData();

        std::ranges::for_each(*pair.second.mutable_elements(), 
        [this, &pin_node_map, &first](uint32_t &id){
            IPinData second = _nodes[pin_node_map[id]]->getPinByID(id)->getData();
            _nodes[second.nodeID]->setPinConnection(second.pinID, first);  
            _nodes[first.nodeID]->setPinConnection(first.pinID, second);
            _connectedPins.insert(first, second);
        });
    });
    return true;
}


// ---------------------- GENERAL FUNCTIONS ---------------------------


QString LGraph::getPinText(uint32_t nodeID, uint32_t pinID) const
{
    return _nodes[nodeID]->pin(pinID)->toStrongRef()->getText();
}
QString LGraph::getNodeName(uint32_t nodeID) const
{
    return _nodes[nodeID]->getName();
}

}