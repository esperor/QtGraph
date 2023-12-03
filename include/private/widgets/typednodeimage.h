#pragma once

#include <QObject>
#include <QtWidgets/QWidget>

#include "models/nodespawndata.h"
#include "logics/typemanager.h"

namespace qtgraph {

class WTypedNodeImage : public QWidget
{
    Q_OBJECT

public:
    explicit WTypedNodeImage(QWidget *parent = nullptr);
    WTypedNodeImage(QString type, QWidget *parent = nullptr);
    ~WTypedNodeImage();

    INodeSpawnData getData() const;
    QSize getDesiredSize() const;
    void initType() { typeID = TypeManager->TypeNames()[typeName]; }

    QString typeName;
    int typeID;
    int fontSize;
    const NodeTypeManager *TypeManager;

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    void paint(QPainter *painter, QPaintEvent *event);

signals:

private:
    QPainter *_painter;

};

}