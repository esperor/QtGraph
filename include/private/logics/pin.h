#pragma once

#include <QObject>
#include <QMap>

#include "models/pindata.h"
#include "logics/node.h"

#include "pin.pb.h"

namespace qtgraph {

class LPin : public QObject
{
    Q_OBJECT

public:
    LPin(uint32_t parentID);

    void protocolize(protocol::Pin *pPin) const;
    void deprotocolize(const protocol::Pin &pPin);

    IPinData getData() const { return _data; }
    bool isConnected() const { return _bIsConnected; }
    const QColor &getColor() const { return _color; }
    QString getText() const { return _text; }
    inline uint32_t ID() const { return getData().pinID; }
    inline uint32_t getNodeID() const { return getData().nodeID; }
    QVector<IPinData> getConnectedPins() const { return _connectedPins.values(); }

    void setParentID(uint32_t newParentID) { _data.nodeID = newParentID; }
    void setTypeID(uint32_t id) { _data.typeID = id; }
    void setID(uint32_t id) { _data.pinID = id; }
    void setDirection(EPinDirection dir) { _data.pinDirection = dir; }
    void setColor(QColor color) { _color = color; }
    void setText(QString text) { _text = text; }

    void addConnectedPin(IPinData pin);
    void removeConnectedPinByID(uint32_t ID);

private:
    IPinData _data;
    // uint32_t here is pinID of connected pin
    QMap<uint32_t, IPinData> _connectedPins;
    // Bound to _connectedPins emptyness
    bool _bIsConnected;
    QString _text;
    QColor _color;
};

}