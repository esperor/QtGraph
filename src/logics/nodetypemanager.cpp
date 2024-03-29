#include <QFile>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "logics/nodetypemanager.h"
#include "utilities/utility.h"

namespace qtgraph {

bool NodeTypeManager::readTypes(const char *file)
{
    _filename = file;

    std::optional<QJsonObject> opt = loadFile(file);
    if (!opt)
        return false;

    QJsonArray array = opt.value().value("nodes").toArray();

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

bool NodeTypeManager::operator!=(const NodeTypeManager &other)
{
    return this->_typeNames == other.TypeNames()
           && this->_types == other.Types()
           && this->_filename == other.getFileName();
}

NodeTypeManager *NodeTypeManager::fromProtocolTypeManager(const protocol::TypeManager &tm)
{
    NodeTypeManager *ntm = new NodeTypeManager;

    ntm->readTypes(QString::fromStdString(tm.filename()));

    return ntm;
}

}
