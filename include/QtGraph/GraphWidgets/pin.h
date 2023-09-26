#pragma once

#include "QtGraph/GraphWidgets/Abstracts/abstractpin.h"
#include "QtGraph/DataClasses/pindata.h"

namespace GraphLib {

class GRAPHLIB_EXPORT Pin : public AbstractPin
{
    Q_OBJECT

public:
    Pin(BaseNode *parent);

    void protocolize(protocol::Pin *pPin) const override;

    void setTypeID(int id) { _data.typeID = id; }
    int typeID() { return _data.typeID; }
};

}