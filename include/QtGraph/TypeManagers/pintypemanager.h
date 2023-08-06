#pragma once


#include "QtGraph/TypeManagers/typemanager.h"
#include "QtGraph/GraphLib.h"

namespace GraphLib {

class GRAPHLIB_EXPORT PinTypeManager final : public TypeManager
{
public:
    PinTypeManager() {}

    bool loadTypes(const char *file) override;

    inline bool loadTypes(const QString &file) { return loadTypes(file.toStdString().c_str()); }
    inline bool loadTypes(const std::string &file) { return loadTypes(file.c_str()); }

    bool operator!=(const PinTypeManager &other);
};

}
