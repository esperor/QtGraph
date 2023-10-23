#pragma once

#include <QLineEdit>

#include "widgets/node.h"

namespace qtgraph {

class WCustomNode : public WANode
{
    Q_OBJECT

public:
    WCustomNode(LNode *logical, WCanvas *canvas) : WANode{ logical, canvas }, _renameEdit{ nullptr } {}

protected:
    virtual void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QLineEdit *_renameEdit;
    
};

}