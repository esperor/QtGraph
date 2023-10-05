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
    INodeSpawnData(const QString &_name);

    QByteArray toByteArray();
    static INodeSpawnData fromByteArray(const QByteArray &byteArray);

    QString name;

};

class ITypedNodeSpawnData : public INodeSpawnData
{
public:
    ITypedNodeSpawnData() {}
    ITypedNodeSpawnData(const ITypedNodeSpawnData &other);
    ITypedNodeSpawnData(const QString &_name, int _typeID);

    QByteArray toByteArray();
    static ITypedNodeSpawnData fromByteArray(const QByteArray &byteArray);

    int typeID;

};

QDebug &operator<<(QDebug &debug, const INodeSpawnData &obj);

QDataStream &operator<<(QDataStream &out, const INodeSpawnData &obj);

inline bool operator<(const INodeSpawnData &first, const INodeSpawnData &second) { return first.name < second.name; }
inline bool operator>(const INodeSpawnData &first, const INodeSpawnData &second) { return first.name > second.name; }
bool operator==(const INodeSpawnData &first, const INodeSpawnData &second);


QDebug &operator<<(QDebug &debug, const ITypedNodeSpawnData &obj);

QDataStream &operator<<(QDataStream &out, const ITypedNodeSpawnData &obj);

inline bool operator<(const ITypedNodeSpawnData &first, const ITypedNodeSpawnData &second) { return first.typeID < second.typeID; }
inline bool operator>(const ITypedNodeSpawnData &first, const ITypedNodeSpawnData &second) { return first.typeID > second.typeID; }
bool operator==(const ITypedNodeSpawnData &first, const ITypedNodeSpawnData &second);

}

Q_DECLARE_METATYPE(qtgraph::INodeSpawnData)
Q_DECLARE_METATYPE(qtgraph::ITypedNodeSpawnData)
