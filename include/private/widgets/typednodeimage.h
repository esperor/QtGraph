#pragma once

#include <QObject>
#include <QtWidgets/QWidget>

#include "QtGraph/DataClasses/nodespawndata.h"
#include "QtGraph/TypeManagers/nodetypemanager.h"

namespace qtgraph {

class TypedNodeImage : public QWidget
{
    Q_OBJECT

public:
    explicit TypedNodeImage(QWidget *parent = nullptr);
    TypedNodeImage(QString type, QWidget *parent = nullptr);
    ~TypedNodeImage();

    TypedNodeSpawnData getData() const;
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