#pragma once

#include "widgets/node.h"

namespace qtgraph {

class WCustomNode : public WANode
{
    Q_OBJECT

public:
    WCustomNode(LNode *logical, WCanvas *canvas) : WANode{ logical, canvas } {}
    
};

}