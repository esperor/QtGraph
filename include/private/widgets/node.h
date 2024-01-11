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
#include "logics/node.h"
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
    WANode(const LNode *logical, WCanvas *canvas);
    ~WANode();
    
    const QSize &normalSize() const { return _normalSize; }
    float getParentCanvasZoomMultiplier() const;
    uint32_t ID() const { return _lnode->ID(); }
    
    QPoint getOutlineCoordinateForPinID(uint32_t pinID) const { return mapToParent(_pinsOutlineCoords[pinID]); }
    QRect getMappedRect() const;
    QSize getNameBounding(const QPainter *painter) const { return painter->fontMetrics().size(Qt::TextSingleLine, _lnode->getName()); }
    QPointF getCanvasPosition() const;
    const WCanvas *getParentCanvas() const { return _parentCanvas; }
    const LNode *getLogical() const { return _lnode; }

    void setLogical(LNode *logical) { _lnode = logical; }
    void setNormalSize(QSize newSize) { _normalSize = newSize; }
    void setPinConnected(uint32_t pinID, bool isConnected);
    inline void moveLNode(QPointF by) { setLNodePosition(_lnode->canvasPosition() + by); }
    void setLNodePosition(QPointF pos);

    void addPin(WPin *pin);

signals:
    void onSelect(INodeSelectSignal signal);
    void onPinDrag(IPinDragSignal signal);
    void onPinConnect(IPinData outPin, IPinData inPin);
    void onPinConnectionBreak(IPinData outPin, IPinData inPin);
    void onAction(IAction *action);

private slots:
    void slot_onPinDrag(IPinDragSignal signal);
    void slot_onPinConnect(IPinData outPin, IPinData inPin);
    void slot_onPinConnectionBreak(IPinData outPin, IPinData inPin);

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
    const LNode *_lnode;

    float _zoom;
    QSize _normalSize;
    QPainter *_painter;
    // Hidden position is used when the node is being moved for snapping
    QPointF _hiddenPosition;
    QPointF _lastMouseDownPosition;
    QPointF _mousePressPosition;
    QPointF _positionDelta;
    
    QMap<uint32_t, QPoint> _pinsOutlineCoords;

    QMap<uint32_t, WPin*> _pins;
};

} 
