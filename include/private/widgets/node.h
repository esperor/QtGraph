#pragma once

#include <cstdint>

#include <QObject>
#include <QtWidgets/QWidget>
#include <QPoint>
#include <QPointF>
#include <QPainter>
#include <QPaintEvent>
#include <QFont>
#include <QFontMetrics>
#include <QMap>

#include "widgets/pin.h"
#include "data/node.h"
#include "models/action.h"
#include "models/nodeselectsignal.h"

#include "qtgraph.h"

#include "node.pb.h"

namespace qtgraph {

class WCanvas;

class WANode : public QWidget
{
    Q_OBJECT

public:
    WANode(const DNode *logical, WCanvas *canvas);
    ~WANode();
    
    const QSize &normalSize() const { return _normalSize; }
    float getParentCanvasZoomMultiplier() const;
    uint32_t ID() const { return _lnode->ID(); }
    
    QPoint getOutlineCoordinateForPinID(uint32_t pinID) const { return mapToParent(_pinsOutlineCoords[pinID]); }
    QRect getMappedRect() const;
    QSize getNameBounding(const QPainter *painter) const { return painter->fontMetrics().size(Qt::TextSingleLine, _lnode->getName()); }
    QPointF getCanvasPosition() const;
    const WCanvas *getParentCanvas() const { return _parentCanvas; }
    const DNode *getLogical() const { return _lnode; }

    void setSource(DNode *logical) { _lnode = logical; }
    void setNormalSize(QSize newSize) { _normalSize = newSize; }
    void setPinFakeConnected(uint32_t pinID, bool isConnected);
    inline void moveNode(QPointF by) { setNodePosition(_lnode->canvasPosition() + by); }
    void setNodePosition(QPointF pos);

    void addPin(WPin *pin);

signals:
    void selectSignal(INodeSelectSignal signal);
    void pinDrag(IPinDragSignal signal);
    void pinConnect(IPinData outPin, IPinData inPin);
    void pinConnectionBreak(IPinData outPin, IPinData inPin);
    void action(IAction *action);

private slots:
    void onPinDrag(IPinDragSignal signal);
    void onPinConnect(IPinData outPin, IPinData inPin);
    void onPinConnectionBreak(IPinData outPin, IPinData inPin);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

// -------------------- RENDER HELPERS -----------------------

    void paint(QPainter *painter, QPaintEvent *event);
    virtual void paintSimplifiedName(QPainter *painter, int desiredWidth, QPoint textOrigin);
    virtual void paintName(QPainter *painter, int desiredWidth, QPoint textOrigin);
    virtual int calculateRowsOffset(QPainter *painter) const;

    // return desired origin relative to this widget
    QPoint getDesiredOrigin() const { return mapFromParent(QPoint(this->pos())); }

// -----------------------------------------------------------


    const WCanvas *_parentCanvas;
    const DNode *_lnode;

    float _zoom;
    QSize _normalSize;
    QPainter *_painter;
    // Hidden position is used when the node is being moved for snapping
    QPointF _hiddenPosition;
    QPointF _lastMouseDownPosition;
    QPointF _mousePressPosition;
    QVector2D _positionDelta;
    
    QMap<uint32_t, QPoint> _pinsOutlineCoords;

    QMap<uint32_t, WPin*> _pins;
};

} 
