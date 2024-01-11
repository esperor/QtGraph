#pragma once

#include <cstdint>
#include <fstream>

#include <QString>
#include <QObject>

#include "logics/node.h"
#include "models/pindata.h"
#include "helpers/idgenerator.h"
#include "helpers/nodefactory.h"
#include "logics/typemanager.h"
#include "logics/stack.h"
#include "models/action.h"

#include <structure.pb.h>
#include <graph.pb.h>

namespace qtgraph {

class LGraph : public QObject
{
    Q_OBJECT

public:
    LGraph(QObject *parent = nullptr);
    ~LGraph();

    uint32_t newID() { return _IDgenerator.generate(); }

    bool serialize(std::fstream *output) const;
    bool protocolize(protocol::Graph &graph) const;
    bool deserialize(std::fstream *input);
    bool deprotocolize(protocol::Graph &graph);
    bool writeStructure(protocol::Structure *structure) const;
    bool readStructure(const protocol::Structure &structure);
    protocol::Structure getStructure() const;

    bool containsNode(uint32_t id) const { return _nodes.contains(id); }
    const QMap<uint32_t, LNode*> &nodes() const { return _nodes; }
    const std::set<uint32_t> &getTakenIDs() const { return _IDgenerator.getTakenIDs(); }
    QString getPinText(uint32_t nodeID, uint32_t pinID) const;
    QString getNodeName(uint32_t nodeID) const;
    const NodeFactory *getFactory() const { return _factory; }
    const QMultiMap<IPinData, IPinData> &getConnections() const { return _connectedPins; }

    void removeNode(uint32_t nodeID);
    void clear();
    bool connectPins(IPinData in, IPinData out);
    void disconnectPins(IPinData in, IPinData out);

    // Executes and records custom action. 
    // Must NOT be used for deletion of any objects such as nodes and pins.
    void executeAction(IAction *action);

    // Reverses last recorded action.
    void undo(int num = 1);

    NodeTypeManager *getNodeTypeManager() { return _factory->getNodeTypeManager(); }
    PinTypeManager *getPinTypeManager() { return _factory->getPinTypeManager(); }

    void setNodeTypeManager(NodeTypeManager *manager) { _factory->setNodeTypeManager(manager); }
    void setPinTypeManager(PinTypeManager *manager) { _factory->setPinTypeManager(manager); }
    inline void setTypeManagers(PinTypeManager *pins, NodeTypeManager *nodes)
    {
        setNodeTypeManager(nodes);
        setPinTypeManager(pins);
    }

    LNode *addNode(QPoint canvasPosition, QString name);
    LNode *addNode(LNode *node);
    LNode *addNode(QPoint canvasPosition, int typeID);

signals:
    void onNodeRemoved(uint32_t id);

private slots:
    void onNodeDestroyed(QObject *obj);

private:
    IDGenerator _IDgenerator = IDGenerator();

    NodeFactory *_factory;

    LStack<IAction*> _stack;

    // Whether graph is recording actions into stack
    bool _bIsRecording; 

    QMap<uint32_t, LNode*> _nodes;

    // Key for _connectedPins is an out-pin and the value is an in-pin
    QMultiMap<IPinData, IPinData> _connectedPins;
};

}
