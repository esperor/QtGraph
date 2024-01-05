#pragma once

#include <QObject>
#include <QtWidgets/QWidget>
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
#include <cstdint>

#include "logics/graph.h"
#include "widgets/typebrowser.h"
#include "widgets/node.h"
#include "models/pindragsignal.h"

#include "structure.pb.h"


namespace qtgraph {

class WCanvas : public QWidget
{
    Q_OBJECT

public:
    WCanvas(QWidget *parent = nullptr);
    ~WCanvas();

    // Serializes attached graph and some canvas properties
    bool serialize(std::fstream *output) const;
    // Deserializes attached graph and some canvas properties
    bool deserialize(std::fstream *input);

    float getZoomMultiplier() const     { return _zoomMultipliers[_zoom]; }
    bool getTelemetricsEnabled() const  { return _bTelemetricsEnabled; }
    bool getSnappingEnabled() const     { return _bIsSnappingEnabled; }
    int getSnappingInterval() const     { return _snappingInterval; }
    const QPointF &getOffset() const    { return _offset; }
    LGraph *getGraph() { return _graph; }
    const LGraph *getGraph_const() const { return _graph; }

    // Visualizes attached graph. Performance-heavy, don't call in cycle
    void visualize();
    void setSnappingEnabled(bool enabled = true) { _bIsSnappingEnabled = enabled; }
    void setTelemetricsEnabled(bool enabled = true) { _bTelemetricsEnabled = enabled; }
    void toggleSnapping() { _bIsSnappingEnabled = !_bIsSnappingEnabled; }
    void setSnappingInterval(int num) { _snappingInterval = num; }
    void clear();
    inline void moveView(QVector2D vector) { moveCanvas(QPointF(-vector.x(), -vector.y())); }
    inline void moveViewUp(float by) { moveCanvas(QPointF(0, by)); }
    inline void moveViewDown(float by) { moveCanvas(QPointF(0, -by)); }
    inline void moveViewLeft(float by) { moveCanvas(QPointF(by, 0)); }
    inline void moveViewRight(float by) { moveCanvas(QPointF(-by, 0)); }

    void setNodeTypeManager(NodeTypeManager *manager);
    void setPinTypeManager(PinTypeManager *manager);
    inline void setTypeManagers(PinTypeManager *pins, NodeTypeManager *nodes)
    {
        setNodeTypeManager(nodes);
        setPinTypeManager(pins);
    }

    QPointF mapToCanvas(QPointF point) const;
    QPoint mapToCanvas(QPoint point) const;

    // If one or more of params of QPointF is negative, current mouse position will be used
    void zoomIn(int times = 1, QPointF where = QPointF(-1, -1));

    // If one of params of QPointF is negative, current mouse position will be used
    void zoomOut(int times = 1, QPointF where = QPointF(-1, -1));

    LNode* addNode(QPoint canvasPosition, QString name);
    LNode* addNode(LNode *lnode);
    LNode* addNode(QPoint canvasPosition, int typeID);

public slots:
    void moveCanvas(QPointF offset);

signals:
    void onNodeRemoved(uint32_t id);

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
    void onLNodeRemoved(uint32_t id);
    void onNodeSelect(bool bIsMultiSelectionModifierDown, uint32_t nodeID);
    void onNodeMove(uint32_t nodeID);
    void onPinDrag(IPinDragSignal signal);
    void onPinConnect(IPinData outPin, IPinData inPin);
    void onPinConnectionBreak(IPinData outPin, IPinData inPin);
    void onTypeBrowserMove(QVector2D offset);
    void tick();

private:
    void paint(QPainter *painter, QPaintEvent *event);
    void moveCanvasOnPinDragNearEdge(QPointF mousePosition);
    void zoom(int times, QPointF where);
    void processSelectionArea(const QMouseEvent *event);

    LGraph *_graph;

    QPainter *_painter;
    int _dotPaintGap;
    std::optional<IPinData> _draggedPin;

    // If there is no target, _draggedPinTargetInfo will be null
    // If the target is cursor, _draggedPinTargetInfo's value will be null
    // Else there will be an object of IPinData struct
    std::optional< std::optional<IPinData> > _draggedPinTargetInfo;
    QPoint _draggedPinTarget;
    QPointF _offset, _lastMouseDownPosition, _mousePosition;
    short _zoom;

    QSize *_lastResizedSize;

    int _snappingInterval;
    bool _bIsSnappingEnabled;
    bool _bTelemetricsEnabled;
    std::optional<QRect> _selectionRect;
    QSet<uint32_t> _selectionAreaPreviousNodes;

    QTimer *_timer;
    WTypeBrowser *_typeBrowser;
    // uint32_t represents node's id 
    QMap<uint32_t, WANode*> _selectedNodes;
    // uint32_t represents node's id
    QMap<uint32_t, WANode*> _nodes;

    const static QMap<short, float> _zoomMultipliers;

};

}
