#pragma once

#include <QString>
#include <QtDebug>

#include "qtgraph.h"

namespace qtgraph {

class INodeSpawnData
{
public:
    INodeSpawnData() {}
    INodeSpawnData(const INodeSpawnData &other);
    INodeSpawnData(const QString &_name, int _typeID = -1);

    QByteArray toByteArray();
    static INodeSpawnData fromByteArray(const QByteArray &byteArray);

    QString name;
    std::optional<int> typeID;
};


QDebug &operator<<(QDebug &debug, const INodeSpawnData &obj);

QDataStream &operator<<(QDataStream &out, const INodeSpawnData &obj);

inline bool operator<(const INodeSpawnData &first, const INodeSpawnData &second) { return first.name < second.name; }
inline bool operator>(const INodeSpawnData &first, const INodeSpawnData &second) { return first.name > second.name; }
bool operator==(const INodeSpawnData &first, const INodeSpawnData &second);

}

Q_DECLARE_METATYPE(qtgraph::INodeSpawnData)
