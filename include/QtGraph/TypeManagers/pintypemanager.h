#pragma once


#include "QtGraph/TypeManagers/typemanager.h"
#include "QtGraph/GraphLib.h"

namespace GraphLib {

class GRAPHLIB_EXPORT PinTypeManager final : public TypeManager
{
public:
    PinTypeManager() {}

    bool readTypes(const char *file) override;

    inline bool readTypes(const QString &file) { return readTypes(file.toStdString().c_str()); }
    inline bool readTypes(const std::string &file) { return readTypes(file.c_str()); }

    bool operator!=(const PinTypeManager &other);
};

}
