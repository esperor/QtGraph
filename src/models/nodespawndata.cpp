#include <QByteArray>

#include "models/nodespawndata.h"
#include "utilities/constants.h"

namespace qtgraph {

INodeSpawnData::INodeSpawnData(const INodeSpawnData &other) 
    : name{ other.name }
    , typeID{ other.typeID }
{}

INodeSpawnData::INodeSpawnData(const QString &_name, int _typeID)
    : name{ _name } 
{
    if (_typeID != -1) typeID = _typeID;
    else typeID = {};
}

QByteArray INodeSpawnData::toByteArray()
{
    QByteArray output;
    output.append(name.toStdString());
    output.append(c_dataSeparator);
    output.append(QByteArray::number(typeID ? *typeID : -1));
    return output;
}

INodeSpawnData INodeSpawnData::fromByteArray(const QByteArray &byteArray)
{
    unsigned short i = 0;

    INodeSpawnData data;
    QList<QByteArray> arrays = byteArray.split(c_dataSeparator);

    data.name = QString::fromStdString(arrays[i++].toStdString());
    data.typeID = arrays[i++].toInt();
    if (data.typeID == -1) data.typeID = {};

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

bool operator==(const INodeSpawnData &first, const INodeSpawnData &second) { return first.name == second.name && first.typeID == second.typeID; }

}
