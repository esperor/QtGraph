#include <QCborMap>
#include <QCborValue>

#include "TypeManagers/typemanager.h"

#include "TypeManagers/moc_typemanager.cpp"

namespace GraphLib {

TypeManager::TypeManager(const TypeManager &other)
{
    *this = other;
}

QCborValue TypeManager::serialize()
{
    QCborMap map;



    return map.toCborValue();
}

void TypeManager::deserialize(const QCborValue &val)
{

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