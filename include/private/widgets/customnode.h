#pragma once

#include <QLineEdit>

#include "widgets/node.h"

namespace qtgraph {

class WCustomNode : public WANode
{
    Q_OBJECT

public:
    WCustomNode(const LNode *logical, WCanvas *canvas);

    void setName(QString name); 

protected slots:
    void deleteRenameEdit();

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
    virtual void paintSimplifiedName(QPainter *painter, int desiredWidth, QPoint textOrigin) override;
    virtual void paintName(QPainter *painter, int desiredWidth, QPoint textOrigin) override;
    virtual void processRenameEdit();

    QLineEdit *_renameEdit;
    
};

}