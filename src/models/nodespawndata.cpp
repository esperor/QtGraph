#include <QByteArray>

#include "models/nodespawndata.h"
#include "utilities/constants.h"

namespace qtgraph {

INodeSpawnData::INodeSpawnData(const INodeSpawnData &other) : name{ other.name }
{}

INodeSpawnData::INodeSpawnData(const QString &_name)
    : name{ _name } {}

QByteArray INodeSpawnData::toByteArray()
{
    QByteArray output;
    output.append(name.toStdString());
    return output;
}

INodeSpawnData INodeSpawnData::fromByteArray(const QByteArray &byteArray)
{
    unsigned short i = 0;

    INodeSpawnData data;
    QList<QByteArray> arrays = byteArray.split(c_dataSeparator);

    data.name = QString::fromStdString(arrays[i++].toStdString());

    return data;
}



TypedNodeSpawnData::TypedNodeSpawnData(const TypedNodeSpawnData &other)
    : INodeSpawnData{ other.name }
    , typeID{ other.typeID }
{}

TypedNodeSpawnData::TypedNodeSpawnData(const QString &_name, int _typeID)
    : INodeSpawnData{ _name }
    , typeID{ _typeID }
{}

QByteArray TypedNodeSpawnData::toByteArray()
{
    QByteArray output;
    output.append(name.toStdString());
    output.append(c_dataSeparator);
    output.append(QByteArray::number(typeID));
    return output;
}

TypedNodeSpawnData TypedNodeSpawnData::fromByteArray(const QByteArray &byteArray)
{
    unsigned short i = 0;

    TypedNodeSpawnData data;
    QList<QByteArray> arrays = byteArray.split(c_dataSeparator);

    data.name = QString::fromStdString(arrays[i++].toStdString());
    data.typeID = arrays[i++].toInt();

    return data;
}


QDebug &operator<<(QDebug &debug, const INodeSpawnData &obj)
{
    bool temp = debug.autoInsertSpaces();
    debug.setAutoInsertSpaces(false);

    debug << "NodeSpawnData(" << obj.name << ")";

    debug.setAutoInsertSpaces(temp);
    return debug;
}

QDataStream &operator<<(QDataStream &out, const INodeSpawnData &obj)
{
    out << "INodeSpawnData(" << obj.name << ")";
    return out;
}

bool operator==(const INodeSpawnData &first, const INodeSpawnData &second) { return first.name == second.name; }

QDebug &operator<<(QDebug &debug, const TypedNodeSpawnData &obj)
{
    bool temp = debug.autoInsertSpaces();
    debug.setAutoInsertSpaces(false);

    debug << "TypedNodeSpawnData(" << obj.name << ", " << obj.typeID << ")";

    debug.setAutoInsertSpaces(temp);
    return debug;
}

QDataStream &operator<<(QDataStream &out, const TypedNodeSpawnData &obj)
{
    out << "TypedNodeSpawnData(" << obj.name << ", " << obj.typeID << ")";
    return out;
}

bool operator==(const TypedNodeSpawnData &first, const TypedNodeSpawnData &second)
{
    return first.name == second.name &&
           first.typeID == second.typeID;
}

}
