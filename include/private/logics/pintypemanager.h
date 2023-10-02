#pragma once


#include "logics/typemanager.h"
#include "qtgraph.h"

namespace qtgraph {

class PinTypeManager final : public TypeManager
{
public:
    PinTypeManager() {}

    bool readTypes(const char *file) override;

    inline bool readTypes(const QString &file) { return readTypes(file.toStdString().c_str()); }
    inline bool readTypes(const std::string &file) { return readTypes(file.c_str()); }

    static PinTypeManager *fromProtocolTypeManager(const protocol::TypeManager &tm);

    bool operator!=(const PinTypeManager &other);
};

}
