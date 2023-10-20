#include <ranges>

#include "logics/graph.h"

namespace qtgraph {

LGraph::LGraph(QObject *parent)
    : QObject(parent)
    , _nodes{ QMap<uint32_t, LNode*>() }
    , _connectedPins{ QMultiMap<IPinData, IPinData>() }
    , _factory{ new NodeFactory(this) }
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
    *(state->mutable_node_type_manager()) = _factory->getNodeTypeManager()->toProtocolTypeManager();
    *(state->mutable_pin_type_manager()) = _factory->getPinTypeManager()->toProtocolTypeManager();

    std::ranges::for_each(_nodes, [state](LNode *node) {
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
    return true;
}

bool LGraph::writeStructure(protocol::Structure *structure) const
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

protocol::Structure LGraph::getStructure() const
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
bool LGraph::readStructure(const protocol::Structure &structure)
{
    std::map<uint32_t, uint32_t> pin_node_map = {};
    int l = _nodes.size();
    std::ranges::for_each(_nodes, [this, &pin_node_map](LNode *node){
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


QString LGraph::getPinText(uint32_t nodeID, uint32_t pinID) const
{
    return _nodes[nodeID]->pin(pinID).value()->getText();
}
QString LGraph::getNodeName(uint32_t nodeID) const
{
    return _nodes[nodeID]->getName();
}

void LGraph::removeNode(uint32_t nodeID)
{
    if (!_nodes.contains(nodeID)) return;
    LNode *ptr = _nodes[nodeID];
    uint32_t id = ptr->ID();
    if (ptr->hasPinConnections())
    {
        const QMap<uint32_t, QVector<IPinData> > *connections = ptr->getPinConnections();
        std::ranges::for_each(connections->asKeyValueRange(), [&](std::pair<const uint32_t&, const QVector<IPinData>&> pair){
            uint32_t id = pair.first;
            std::ranges::for_each(pair.second, [&](IPinData connectedPin){
                _nodes[connectedPin.nodeID]->removePinConnection(connectedPin.pinID, id);

                const LPin *pin = ptr->pin(id).value();
                if (pin->getData().pinDirection == EPinDirection::Out)
                    _connectedPins.remove(pin->getData());
                else
                    _connectedPins.remove(connectedPin, pin->getData());
            });
        });
    }
    _nodes.remove(id);
}


bool LGraph::connectPins(IPinData in, IPinData out)
{
    if (_connectedPins.contains(out, in)) return false;

    _connectedPins.insert(out, in);
    _nodes[out.nodeID]->setPinConnection(out.pinID, in);
    _nodes[in.nodeID]->setPinConnection(in.pinID, out);
    return true;
}

void LGraph::disconnectPins(IPinData in, IPinData out)
{
    auto it = _connectedPins.find(out, in);
    if (it != _connectedPins.end())
    {
        _connectedPins.erase(it);

        _nodes[out.nodeID]->removePinConnection(out.pinID, in.pinID);
        _nodes[in.nodeID]->removePinConnection(in.pinID, out.pinID);
    }
}

LNode *LGraph::addNode(QPoint canvasPosition, QString name)
{
    LNode *node = new LNode(this);
    node->setCanvasPosition(canvasPosition);
    node->setName(name);
    return addNode(node);
}

LNode *LGraph::addNode(LNode *node)
{
    uint32_t id = node->ID();
    _nodes.insert(id, node);
    connect(_nodes[id], &LNode::destroyed, this, &LGraph::onNodeDestroyed);
    return _nodes[id];
}

LNode *LGraph::addNode(QPoint canvasPosition, int typeID)
{
    LNode *node = _factory->makeNodeAndPinsOfType(typeID, this);
    node->setCanvasPosition(canvasPosition);
    return addNode(node);
}


// ------------------------- SLOTS ----------------------


void LGraph::onNodeDestroyed(QObject *obj)
{
    if (obj == nullptr) qDebug() << "Pointer to destroyed node is nullptr";
    uint32_t id = ((LNode*)obj)->ID();
    _IDgenerator.removeTaken(id);
    emit onNodeRemoved(id);
}
    

}