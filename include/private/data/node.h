#pragma once


#include <QMap>
#include <QObject>
#include <QPointF>

#include "helpers/idgenerator.h"
#include "data/pin.h"
#include "helpers/nodefactory.h"

#include "node.pb.h"

namespace qtgraph {

class DGraph;

class DNode : public Object
{
    Q_OBJECT

public:
    DNode(DGraph *graph, std::optional<uint32_t> id = {});
    ~DNode();

    void protocolize(protocol::Node *pNode) const;
    void deprotocolize(const protocol::Node &pNode);

    uint32_t ID() const { return _ID; }
    Controller *controller();

    bool isSelected() const { return _bIsSelected; }
    const QMap<uint32_t, DPin*> &pins() const { return _pins; }
    std::optional<DPin*> operator[](uint32_t id); 
    // Alias for [id]
    inline std::optional<DPin*> pin(uint32_t id) { return this->operator[](id); };
    const QPointF &canvasPosition() const { return _canvasPosition; }

    bool hasPinConnections() const;
    const QMap<uint32_t, QVector<IPinData> > *getPinConnections() const;
    QList<uint32_t> getPinIDs() const { return _pins.keys(); }
    const QString &getName() const { return _name; }
    bool doesPinExist(uint32_t id) const { return _pins.contains(id); };
    std::optional<uint32_t> getTypeID() const { return _typeID; }

    void setTypeID(uint32_t newTypeID) { _typeID = newTypeID; }
    void setNodeTypeManager(NodeTypeManager *manager);
    void setPinTypeManager(PinTypeManager *manager);
    inline void setTypeManagers(PinTypeManager *pins, NodeTypeManager *nodes)
    {
        setNodeTypeManager(nodes);
        setPinTypeManager(pins);
    }
    void moveCanvasPosition(QPointF by) { _canvasPosition += by; }
    void setCanvasPosition(QPointF newCanvasPosition) { _canvasPosition = newCanvasPosition; }
    void setID(uint32_t ID) { _ID = ID; }
    void setName(QString name) { _name = name; }
    void setSelected(bool b);
    void removePinConnection(uint32_t pinID, uint32_t connectedPinID);
    void setPinConnection(uint32_t pinID, IPinData connectedPin);

    // Returns pin id
    uint32_t addPin(DPin *pin);
    // Returns pin id
    uint32_t addPin(QString text, EPinDirection direction, QColor color = QColor(Qt::GlobalColor::black));

signals:
    void isSelectedChanged(bool selected, uint32_t id);
    void pinRemoved(uint32_t id);

private slots:
    void onPinDestroyed(QObject *obj);

private:
    NodeTypeManager *_nodeTypeManager;
    PinTypeManager *_pinTypeManager;

    uint32_t _ID;
    QString _name;
    bool _bIsSelected;
    QPointF _canvasPosition;
    std::optional<uint32_t> _typeID;

    QMap<uint32_t, DPin*> _pins;

};

}