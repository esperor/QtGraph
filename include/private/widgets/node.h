#pragma once

#include <cstdint>

#include <QObject>
#include <QWidget>
#include <QPoint>
#include <QPointF>
#include <QPainter>
#include <QPaintEvent>
#include <QFont>
#include <QFontMetrics>
#include <QMap>

#include "widgets/pin.h"
#include "qtgraph.h"
#include "logics/node.h"

#include "node.pb.h"

namespace qtgraph {

class WCanvas;

class WANode : public QWidget
{
    Q_OBJECT

public:
    WANode(WCanvas *canvas);
    ~WANode();
    
    const QSize &normalSize() const { return _normalSize; }
    float getParentCanvasZoomMultiplier() const;
    uint32_t ID() const { return _lnode->ID(); }
    
    QPoint getOutlineCoordinateForPinID(uint32_t pinID) const { return mapToParent(_pinsOutlineCoords[pinID]); }
    QRect getMappedRect() const;
    const WCanvas *getParentCanvas() const { return _parentCanvas; }

    void setNormalSize(QSize newSize) { _normalSize = newSize; }
    void setSelected(bool b, bool bIsMultiSelectionModifierDown = false);
    void setPinConnected(uint32_t pinID, bool isConnected);

    void addPin(WPin *pin);

signals:
    void onSelect(bool bIsMultiSelectionModifierDown, uint32_t nodeID);
    void onPinDrag(IPinDragSignal signal);
    void onPinConnect(IPinData outPin, IPinData inPin);
    void onPinConnectionBreak(IPinData outPin, IPinData inPin);

private slots:
    void slot_onPinDrag(IPinDragSignal signal);
    void slot_onPinConnect(IPinData outPin, IPinData inPin);
    void slot_onPinConnectionBreak(IPinData outPin, IPinData inPin);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:

// -------------------- RENDER HELPERS -----------------------

    void paint(QPainter *painter, QPaintEvent *event);
    virtual void paintSimplifiedName(QPainter *painter, int desiredWidth, QPoint textOrigin);
    virtual void paintName(QPainter *painter, int desiredWidth, QPoint textOrigin);
    virtual int calculateRowsOffset(QPainter *painter) const;

// -----------------------------------------------------------

protected:
    QSharedPointer<LNode> _lnode;

    const WCanvas *_parentCanvas;
    float _zoom;
    QSize _normalSize;
    QPainter *_painter;
    // Hidden position is used when the node is being moved for snapping
    QPointF _hiddenPosition;
    QPointF _lastMouseDownPosition;
    QPointF _mousePressPosition;
    
    QMap<uint32_t, QPoint> _pinsOutlineCoords;

    QMap<uint32_t, QSharedPointer<WPin>> _pins;
};

} 
