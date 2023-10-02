
#pragma once

#include <QString>
#include <QColor>
#include <QByteArray>
#include <QtDebug>

#include "qtgraph.h"

namespace qtgraph {

enum class EPinDirection
{
    In = 1,
    Out = 2,
};

class LPin;

struct IPinData
{
public:
    IPinData() {}
    IPinData(const IPinData &other);
    IPinData(EPinDirection _direction, uint32_t _nodeID, uint32_t _pinID, uint32_t _typeID = -1);
    IPinData(const LPin *pin);
    virtual ~IPinData();

    QByteArray toByteArray() const;
    static IPinData fromByteArray(const QByteArray &byteArray);
    void operator=(const IPinData &other);

    EPinDirection pinDirection;
    uint32_t nodeID;
    uint32_t pinID;
    uint32_t typeID;
};

QDebug &operator<<(QDebug &debug, const IPinData &obj);

QDataStream &operator<<(QDataStream &out, const IPinData &obj);

bool operator<(const IPinData &first, const IPinData &second);
bool operator>(const IPinData &first, const IPinData &second);
bool operator==(const IPinData &first, const IPinData &second);

}

Q_DECLARE_METATYPE(qtgraph::IPinData)
