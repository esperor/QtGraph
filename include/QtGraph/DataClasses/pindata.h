
#pragma once

#include <QString>
#include <QColor>
#include <QByteArray>
#include <QtDebug>

#include "QtGraph/GraphLib.h"

namespace GraphLib {
enum class PinDirection;
class AbstractPin;

struct GRAPHLIB_EXPORT PinData
{
public:
    PinData() {}
    PinData(const PinData &other);
    PinData(PinDirection _direction, unsigned int _nodeID, unsigned int _pinID, unsigned int _typeID = -1);
    PinData(const AbstractPin *pin);
    virtual ~PinData();

    QByteArray toByteArray() const;
    static PinData fromByteArray(const QByteArray &byteArray);
    void operator=(const PinData &other);

    PinDirection pinDirection;
    unsigned int nodeID;
    unsigned int pinID;
    unsigned int typeID;
};

QDebug GRAPHLIB_EXPORT &operator<<(QDebug &debug, const PinData &obj);

QDataStream GRAPHLIB_EXPORT &operator<<(QDataStream &out, const PinData &obj);

bool GRAPHLIB_EXPORT operator<(const PinData &first, const PinData &second);
bool GRAPHLIB_EXPORT operator>(const PinData &first, const PinData &second);
bool GRAPHLIB_EXPORT operator==(const PinData &first, const PinData &second);

}

Q_DECLARE_METATYPE(GraphLib::PinData)
