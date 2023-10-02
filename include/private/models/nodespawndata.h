#pragma once

#include <QString>
#include <QtDebug>

#include "qtgraph.h"

namespace qtgraph {

class NodeSpawnData
{
public:
    NodeSpawnData() {}
    NodeSpawnData(const NodeSpawnData &other);
    NodeSpawnData(const QString &_name);

    QByteArray toByteArray();
    static NodeSpawnData fromByteArray(const QByteArray &byteArray);

    QString name;

};

class TypedNodeSpawnData : public NodeSpawnData
{
public:
    TypedNodeSpawnData() {}
    TypedNodeSpawnData(const TypedNodeSpawnData &other);
    TypedNodeSpawnData(const QString &_name, int _typeID);

    QByteArray toByteArray();
    static TypedNodeSpawnData fromByteArray(const QByteArray &byteArray);

    int typeID;

};

QDebug &operator<<(QDebug &debug, const NodeSpawnData &obj);

QDataStream &operator<<(QDataStream &out, const NodeSpawnData &obj);

inline bool operator<(const NodeSpawnData &first, const NodeSpawnData &second) { return first.name < second.name; }
inline bool operator>(const NodeSpawnData &first, const NodeSpawnData &second) { return first.name > second.name; }
bool operator==(const NodeSpawnData &first, const NodeSpawnData &second);


QDebug &operator<<(QDebug &debug, const TypedNodeSpawnData &obj);

QDataStream &operator<<(QDataStream &out, const TypedNodeSpawnData &obj);

inline bool operator<(const TypedNodeSpawnData &first, const TypedNodeSpawnData &second) { return first.typeID < second.typeID; }
inline bool operator>(const TypedNodeSpawnData &first, const TypedNodeSpawnData &second) { return first.typeID > second.typeID; }
bool operator==(const TypedNodeSpawnData &first, const TypedNodeSpawnData &second);

}

Q_DECLARE_METATYPE(qtgraph::NodeSpawnData)
Q_DECLARE_METATYPE(qtgraph::TypedNodeSpawnData)
