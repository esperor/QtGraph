#pragma once


#include "QtGraph/TypeManagers/typemanager.h"
#include "QtGraph/GraphLib.h"

namespace GraphLib {

class GRAPHLIB_EXPORT NodeTypeManager final : public TypeManager
{
public:
    NodeTypeManager() {}

    bool readTypes(const char *file) override;

    inline bool readTypes(const QString &file) { return readTypes(file.toStdString().c_str()); }
    inline bool readTypes(const std::string &file) { return readTypes(file.c_str()); }

    static NodeTypeManager fromProtocolTypeManager(const protocol::TypeManager &tm);

    bool operator!=(const NodeTypeManager &other);
};

}
