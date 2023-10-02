#include <cstdint>
#include <fstream>

#include <QString>
#include <QObject>

#include "logics/node.h"
#include "models/pindata.h"

#include <structure.pb.h>
#include <graph.pb.h>

class LGraph : public QObject
{
    Q_OBJECT

public:
    LGraph();

    // std:: istream and ostream serialization

    bool serialize(std::fstream *output) const;
    bool protocolize(protocol::Graph &graph) const;
    bool deserialize(std::fstream *input);
    bool deprotocolize(protocol::Graph &graph);

    bool writeStructure(protocol::Structure *structure) const;
    bool readStructure(const protocol::Structure &structure);

    QString getPinText(uint32_t nodeID, uint32_t pinID) const;
    QString getNodeName(uint32_t nodeID) const;

    void removeNode(QSharedPointer<WANode> &ptr);
    void setNodeTypeManager(NodeTypeManager *manager);
    void setPinTypeManager(PinTypeManager *manager);
    inline void setTypeManagers(PinTypeManager *pins, NodeTypeManager *nodes)
    {
        setNodeTypeManager(nodes);
        setPinTypeManager(pins);
    }

    QWeakPointer<LNode> addNode(QPoint canvasPosition, QString name);
    QWeakPointer<LNode> addNode(LNode *node);
    QWeakPointer<LNode> addTypedNode(QPoint canvasPosition, int typeID);

private:
    IDGenerator _IDgenerator = IDGenerator();

    QSharedPointer<NodeFactory> _factory;
    NodeTypeManager _nodeTypeManager;
    PinTypeManager _pinTypeManager;

    QMap<uint32_t, QSharedPointer<LNode>> _nodes;

    // Key for _connectedPins is an out-pin and the value is an in-pin
    QMultiMap<IPinData, IPinData> _connectedPins;
};