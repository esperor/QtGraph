#pragma once

#include <QObject>
#include <QtWidgets/QWidget>
#include <QPainter>
#include <QBoxLayout>
#include <QVector2D>
#include <QPushButton>
#include <QScrollArea>

#include "qtgraph.h"
#include "logics/typemanager.h"
#include "widgets/typednodeimage.h"
#include "widgets/nfbuttonminimize.h"
#include "widgets/nodeimage.h"

namespace qtgraph {

class WTypeBrowser : public QWidget
{
    Q_OBJECT

public:
    explicit WTypeBrowser(QWidget *parent = nullptr);
    ~WTypeBrowser();

    QSize getDesiredSize() const;
    const QPointF &getPosition() const { return _position; }
    int getGap() const { return _gap; }
    bool isMinimized() const { return _bIsMinimized; }

    inline void setDirection(QBoxLayout::Direction dir) { _layout->setDirection(dir); }
    void setGap(int g) { _gap = g; }
    void setPosition(QPointF pos) { _position = pos; }
    inline void setPosition(qreal x, qreal y) { setPosition(QPointF(x, y)); }
    void setX(qreal x) { setPosition(x, getPosition().y()); }
    void setY(qreal y) { setPosition(getPosition().x(), y); }
    void adjustPosition(QVector2D by) { _position += by.toPointF(); }
    inline void adjustPosition(qreal x, qreal y) { adjustPosition(QVector2D(x, y)); }
    void setMinimized(bool b) { _bIsMinimized = b; }
    bool initTypes();
    void clear();

    const NodeTypeManager *_nodeTypeManager;
    const PinTypeManager *_pinTypeManager;

public slots:
    void onNodeFactoryCleared();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    void paint(QPainter *painter, QPaintEvent *event);
    WNodeImage *createDefaultImage() { return new WNodeImage(this); }

signals:
    void onMove(QVector2D offset);

private slots:
    void onButtonMinimizeClick();

private:
    QPainter *_painter;
    int _gap;
    bool _bIsMinimized;
    QPointF _position, _lastMouseDownPosition, _mousePressPosition;
    QWidget *_layoutHolder;
    QBoxLayout *_layout;
    NFButtonMinimize *_btnMinimize;
    QVector<WTypedNodeImage*> _nodeImages;
    

};

}