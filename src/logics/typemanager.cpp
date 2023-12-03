#include <QCborMap>
#include <QCborValue>
#include <QJsonArray>
#include <QJsonValue>

#include <optional>

#include "logics/typemanager.h"
#include "utilities/utility.h"

#include "logics/moc_typemanager.cpp"

namespace qtgraph {

TypeManager::TypeManager(const TypeManager &other)
{
    *this = other;
}

bool TypeManager::readTypes(const char *file)
{
    _filename = file;

    std::optional<QJsonObject> opt = loadFile(file);
    if (!opt)
        return false;

    QJsonArray array = opt.value().value(keyValue()).toArray();

    uint32_t i = 0;
    _types.reserve(array.size());
    for (auto elem : array)
    {
        _types.append(elem.toObject());
        _typeNames.insert(_types.at(i).value("name").toString(), i);
        i++;
    }

    return true;
}

protocol::TypeManager TypeManager::toProtocolTypeManager() const
{
    protocol::TypeManager tm;
    *(tm.mutable_filename()) = _filename.toStdString();
    return tm;
}

bool TypeManager::operator!=(const TypeManager &other)
{
    return _types != other.Types()
        || _typeNames != other.TypeNames()
        || _filename != other.getFileName();
}

void TypeManager::operator=(const TypeManager &other)
{
    _types = other.Types();
    _typeNames = other.TypeNames();
    _filename = other.getFileName();
}





NodeTypeManager *NodeTypeManager::fromProtocolTypeManager(const protocol::TypeManager &tm)
{
    NodeTypeManager *ntm = new NodeTypeManager;
    ntm->readTypes(QString::fromStdString(tm.filename()));
    return ntm;
}




PinTypeManager *PinTypeManager::fromProtocolTypeManager(const protocol::TypeManager &tm)
{
    PinTypeManager *ptm = new PinTypeManager;
    ptm->readTypes(QString::fromStdString(tm.filename()));
    return ptm;
}

}