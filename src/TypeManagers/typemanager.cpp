#include <QCborMap>
#include <QCborValue>

#include "TypeManagers/typemanager.h"

#include "TypeManagers/moc_typemanager.cpp"

namespace GraphLib {

TypeManager::TypeManager(const TypeManager &other)
{
    *this = other;
}

protocol::TypeManager TypeManager::toProtocolTypeManager()
{
    protocol::TypeManager tm;
    *(tm.mutable_filename()) = _filename.toStdString();
    return tm;
}

bool TypeManager::operator!=(const TypeManager &other)
{
    return _types != other.Types()
        && _typeNames != other.TypeNames()
        && _filename != other.getFileName();
}

void TypeManager::operator=(const TypeManager &other)
{
    _types = other.Types();
    _typeNames = other.TypeNames();
    _filename = other.getFileName();
}

}