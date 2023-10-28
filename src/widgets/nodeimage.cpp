#include "widgets/nodeimage.h"
#include "utilities/constants.h"

#include "widgets/moc_nodeimage.cpp"

namespace qtgraph {

WNodeImage::WNodeImage(QWidget *parent)
    : WTypedNodeImage(parent)
{
    typeName = c_customNodeTypeName;
    typeID = -1;
}

}