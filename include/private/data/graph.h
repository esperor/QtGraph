#pragma once

#include <cstdint>
#include <fstream>

#include <QString>
#include <QObject>

#include "data/node.h"
#include "models/pindata.h"
#include "helpers/idgenerator.h"
#include "helpers/nodefactory.h"
#include "logics/typemanager.h"
#include "helpers/stack.h"
#include "models/action.h"
#include "logics/object.h"

#include <structure.pb.h>
#include <graph.pb.h>

namespace qtgraph {

class DGraph : public Object
{
    Q_OBJECT

    friend class Controller;

public:
    DGraph(QObject *parent = nullptr);
    ~DGraph();

    bool protocolize(protocol::Graph &graph) const;
    bool deprotocolize(protocol::Graph &graph);
    bool writeStructure(protocol::Structure *structure) const;
    bool readStructure(const protocol::Structure &structure);
    protocol::Structure getStructure() const;

    uint32_t ID() const { return _ID; }
    Controller *controller() { return (Controller*)parent(); }

    bool containsNode(uint32_t id) const { return _nodes.contains(id); }
    QMap<uint32_t, DNode*> &nodes() { return _nodes; }
    QMultiMap<IPinData, IPinData> &connectedPins() { return _connectedPins; }
    QString getPinText(uint32_t nodeID, uint32_t pinID) const;
    QString getNodeName(uint32_t nodeID) const;
    
    const QMultiMap<IPinData, IPinData> &getConnections() const { return _connectedPins; }

    DNode *addNode(DNode *node);

private:
    QMap<uint32_t, DNode*> _nodes;

    // Key for _connectedPins is an out-pin and the value is an in-pin
    QMultiMap<IPinData, IPinData> _connectedPins;

    uint32_t _ID;
};

}
