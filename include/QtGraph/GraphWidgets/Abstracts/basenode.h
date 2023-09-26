#pragma once

#include <QObject>
#include <QWidget>
#include <QPoint>
#include <QPointF>
#include <QPainter>
#include <QPaintEvent>
#include <QFont>
#include <QFontMetrics>
#include <QMap>

#include <cstdint>

#include "QtGraph/GraphWidgets/Abstracts/abstractpin.h"
#include "QtGraph/GraphLib.h"
#include "QtGraph/idgenerator.h"
#include "QtGraph/NodeFactoryModule/nodefactory.h"

#include "node.pb.h"

namespace GraphLib {

class Canvas;

class GRAPHLIB_EXPORT BaseNode : public QWidget
{
    Q_OBJECT

public:
    BaseNode(Canvas *canvas);
    ~BaseNode();

    virtual void protocolize(protocol::Node *pNode) const;
    virtual void deprotocolize(const protocol::Node &pNode);

    static uint32_t newID() { return _IDgenerator.generate(); }
    // Static function! Returns taken id's for ALL THE NODES
    static const std::set<uint32_t> &getTakenPinIDs() { return _IDgenerator.getTakenIDs(); }

    const QPointF &canvasPosition() const { return _canvasPosition; }
    uint32_t ID() const { return _ID; }
    const QSize &normalSize() const { return _normalSize; }
    float getParentCanvasZoomMultiplier() const;
    const QString &name() const { return _name; }
    QPoint getOutlineCoordinateForPinID(uint32_t pinID) const { return mapToParent(_pinsOutlineCoords[pinID]); }
    bool hasPinConnections() const;
    QSharedPointer< QMap<uint32_t, QVector<PinData> > > getPinConnections() const;
    QList<uint32_t> getPinIDs() const { return _pins.keys(); }
    const AbstractPin *getPinByID(uint32_t pinID) const { return _pins[pinID]; }
    QRect getMappedRect() const;
    const Canvas *getParentCanvas() const { return _parentCanvas; }
    const QString &getName() const { return _name; }

    void setFactory(NodeFactoryModule::NodeFactory *factory) { _factory = factory; }
    void setCanvasPosition(QPointF newCanvasPosition) { _canvasPosition = newCanvasPosition; }
    void setID(uint32_t ID) { _ID = ID; }
    void setNormalSize(QSize newSize) { _normalSize = newSize; }
    void setName(QString name) { _name = name; }
    void removePinConnection(uint32_t pinID, uint32_t connectedPinID);
    void setPinConnection(uint32_t pinID, PinData connectedPin);
    void setPinConnected(uint32_t pinID, bool isConnected);
    void setSelected(bool b, bool bIsMultiSelectionModifierDown = false) { _bIsSelected = b; if (b) onSelect(bIsMultiSelectionModifierDown, _ID); }

    void moveCanvasPosition(QPointF vector) { _canvasPosition += vector; }

signals:
    void onSelect(bool bIsMultiSelectionModifierDown, uint32_t nodeID);
    void onPinDrag(PinDragSignal signal);
    void onPinConnect(PinData outPin, PinData inPin);
    void onPinConnectionBreak(PinData outPin, PinData inPin);

public slots:
    void addPin(AbstractPin *pin);
    void addPin(QString text, PinDirection direction, QColor color = QColor(Qt::GlobalColor::black));

private slots:
    void onPinDestroyed(QObject *obj);
    void slot_onPinDrag(PinDragSignal signal);
    void slot_onPinConnect(PinData outPin, PinData inPin);
    void slot_onPinConnectionBreak(PinData outPin, PinData inPin);

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
    static IDGenerator _IDgenerator;

    NodeFactoryModule::NodeFactory *_factory;
    const Canvas *_parentCanvas;
    uint32_t _ID;
    float _zoom;
    QSize _normalSize;
    QPainter *_painter;
    QPointF _canvasPosition;
    // Hidden position is used when the node is being moved for snapping
    QPointF _hiddenPosition;
    bool _bIsSelected;
    QPointF _lastMouseDownPosition;
    QPointF _mousePressPosition;
    QString _name;
    QMap<uint32_t, QPoint> _pinsOutlineCoords;

    QMap<uint32_t, AbstractPin*> _pins;
};

}
