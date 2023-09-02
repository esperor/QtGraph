#include <QFile>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "TypeManagers/nodetypemanager.h"
#include "utility.h"

namespace GraphLib {

bool NodeTypeManager::readTypes(const char *file)
{
    _filename = file;

    std::optional<QJsonObject> opt = loadFile(file);
    if (!opt)
        return false;

    QJsonArray array = opt.value().value("nodes").toArray();

    unsigned int i = 0;
    _types.reserve(array.size());
    for (auto elem : array)
    {
        _types.append(elem.toObject());
        _typeNames.insert(_types.at(i).value("name").toString(), i);
        i++;
    }

    return true;
}

bool NodeTypeManager::operator!=(const NodeTypeManager &other)
{
    return this->_typeNames == other.TypeNames()
           && this->_types == other.Types()
           && this->_filename == other.getFileName();
}

}
