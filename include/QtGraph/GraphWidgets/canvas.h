#pragma once

#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QWeakPointer>
#include <QSharedPointer>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QPointF>
#include <QVector>
#include <QTimer>
#include <QWheelEvent>
#include <optional>
#include <variant>

#include "QtGraph/NodeFactoryModule/nodefactory.h"
#include "QtGraph/NodeFactoryModule/typebrowser.h"
#include "QtGraph/TypeManagers/nodetypemanager.h"
#include "QtGraph/TypeManagers/pintypemanager.h"
#include "QtGraph/GraphWidgets/Abstracts/basenode.h"
#include "QtGraph/GraphLib.h"
#include "QtGraph/constants.h"


namespace GraphLib {

class GRAPHLIB_EXPORT Canvas : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(NodeTypeManager _nodeTypeManager MEMBER _nodeTypeManager)
    Q_PROPERTY(PinTypeManager _pinTypeManager MEMBER _pinTypeManager)
    Q_PROPERTY(QPointF _offset MEMBER _offset)
    Q_PROPERTY(short _zoom MEMBER _zoom)
    Q_PROPERTY(int _snappingInterval MEMBER _snappingInterval)
    Q_PROPERTY(bool _bIsSnappingEnabled MEMBER _bIsSnappingEnabled)
    Q_PROPERTY(QMap<int, QSharedPointer<BaseNode>> _nodes MEMBER _nodes)
    Q_PROPERTY(QMultiMap<PinData, PinData> _connectedPins MEMBER _connectedPins)
    Q_PROPERTY(QMap<int, QSharedPointer<BaseNode>> _selectedNodes MEMBER _selectedNodes)

public:
    Canvas(QWidget *parent = nullptr);
    ~Canvas();

    // std:: istream and ostream serialization
    bool serialize(std::fstream *output);
    bool deserialize(std::fstream *input);


    static unsigned int newID() { return IDgenerator++; }

    float getZoomMultiplier() const     { return _zoomMultipliers[_zoom]; }
    bool getSnappingEnabled() const     { return _bIsSnappingEnabled; }
    int getSnappingInterval() const     { return _snappingInterval; }
    const QPointF &getOffset() const    { return _offset; }
    QString getPinText(int nodeID, int pinID) const;
    QString getNodeName(int nodeID) const;

    void setSnappingInterval(int num) { _snappingInterval = num; }
    void setNodeTypeManager(NodeTypeManager *manager);
    void setPinTypeManager(PinTypeManager *manager);
    inline void setTypeManagers(PinTypeManager *pins, NodeTypeManager *nodes) { setNodeTypeManager(nodes); setPinTypeManager(pins); }
    inline void moveView(QVector2D vector) { moveCanvas(QPointF(-vector.x(), -vector.y())); }
    inline void moveViewUp(float by) { moveCanvas(QPointF(0, by)); }
    inline void moveViewDown(float by) { moveCanvas(QPointF(0, -by)); }
    inline void moveViewLeft(float by) { moveCanvas(QPointF(by, 0)); }
    inline void moveViewRight(float by) { moveCanvas(QPointF(-by, 0)); }

    QPointF mapToCanvas(QPointF point) const;
    QPoint mapToCanvas(QPoint point) const;

    // If one or more of params of QPointF is negative, current mouse position will be used
    void zoomIn(int times = 1, QPointF where = QPointF(-1, -1));

    // If one of params of QPointF is negative, current mouse position will be used
    void zoomOut(int times = 1, QPointF where = QPointF(-1, -1));

    QWeakPointer<BaseNode> addBaseNode(QPoint canvasPosition, QString name);
    QWeakPointer<BaseNode> addNode(BaseNode *node);
    QWeakPointer<BaseNode> addTypedNode(QPoint canvasPosition, int typeID);

public slots:
    void moveCanvas(QPointF offset);

signals:
    void onNodesRemoved();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onNodeSelect(bool bIsMultiSelectionModifierDown, int nodeID);
    void onPinDrag(PinDragSignal signal);
    void onPinConnect(PinData outPin, PinData inPin);
    void onPinConnectionBreak(PinData outPin, PinData inPin);
    void onNFWidgetMove(QVector2D offset);
    void tick();

private:
    void paint(QPainter *painter, QPaintEvent *event);
    void moveCanvasOnPinDragNearEdge(QPointF mousePosition);
    void zoom(int times, QPointF where);
    void deleteNode(QSharedPointer<BaseNode> &ptr);
    void processSelectionArea(const QMouseEvent *event);
    static unsigned int IDgenerator;

    QSharedPointer<NodeFactoryModule::NodeFactory> _factory;
    NodeTypeManager _nodeTypeManager;
    PinTypeManager _pinTypeManager;

    QPainter *_painter;
    int _dotPaintGap;
    std::optional<PinData> _draggedPin;

    // If there is no target, _draggedPinTargetInfo will be null
    // If the target is cursor, _draggedPinTargetInfo's value will be null
    // Else there will be an object of PinData struct
    std::optional< std::optional<PinData> > _draggedPinTargetInfo;
    QPoint _draggedPinTarget;
    QPointF _offset, _lastMouseDownPosition, _mousePosition;
    short _zoom;

    QSize *_lastResizedSize;

    int _snappingInterval;
    bool _bIsSnappingEnabled;
    std::optional<QRect> _selectionRect;
    QSet<int> _selectionAreaPreviousNodes;

    QMap<int, QSharedPointer<BaseNode>> _nodes;

    // Key for _connectedPins is an out-pin and the value is an in-pin
    QMultiMap<PinData, PinData> _connectedPins;
    QTimer *_timer;
    NodeFactoryModule::TypeBrowser *_typeBrowser;
    QMap<int, QSharedPointer<BaseNode>> _selectedNodes;

    const static QMap<short, float> _zoomMultipliers;

};

}
