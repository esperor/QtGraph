#pragma once

#include <QObject>
#include <QtWidgets/QWidget>

#include "models/nodespawndata.h"
#include "logics/nodetypemanager.h"
#include "widgets/typednodeimage.h"

namespace qtgraph {

class WNodeImage : public WTypedNodeImage
{
    Q_OBJECT

public:
    WNodeImage(QWidget *parent = nullptr);

};

}