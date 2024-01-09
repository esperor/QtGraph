#include <QPen>
#include <QRect>
#include <QPalette>
#include <algorithm>
#include <QMimeData>
#include <QLinearGradient>
#include <QPainterPath>
#include <QCborValue>
#include <QCborMap>
#include <cmath>
#include <fstream>

#include "widgets/canvas.h"
#include "utilities/constants.h"
#include "utilities/utility.h"
#include "models/nodespawndata.h"

#include "graph.pb.h"

#include "widgets/moc_canvas.cpp"

// this is not really a good practice
#ifdef NDEBUG
#define DEBUG false
#else
#define DEBUG true
#endif

namespace qtgraph {

WCanvas::WCanvas(QWidget *parent)
    : QWidget{ parent }
    , _graph{ new LGraph(this) }
    , _painter{ new QPainter(this) }
    , _dotPaintGap{ 40 }
    , _draggedPin{ std::nullopt }
    , _draggedPinTargetInfo{ std::nullopt }
    , _draggedPinTarget{ QPoint() }
    , _offset{ QPointF(0, 0) }
    , _lastMouseDownPosition{ QPointF() }
    , _mousePosition{ QPointF(0, 0) }
    , _zoom{ -4 }
    , _lastResizedSize{ nullptr }
    , _snappingInterval{ 20 }
    , _bIsSnappingEnabled{ true }
    , _bTelemetricsEnabled{ DEBUG }
    , _selectionRect{ std::nullopt }
    , _selectionAreaPreviousNodes{ QSet<uint32_t>() }
    , _typeBrowser{ new WTypeBrowser(this) }
    , _selectedNodes{ QMap<uint32_t, WANode*>() }
{
    setMouseTracking(true);
    setAutoFillBackground(true);
    setFocusPolicy(Qt::FocusPolicy::StrongFocus);

    QPalette palette(c_paletteDefaultColor);
    this->setPalette(palette);
    setAcceptDrops(true);

    _typeBrowser->show();

    connect(_typeBrowser, &WTypeBrowser::onMove, this, &WCanvas::onTypeBrowserMove);
    connect(_graph, &LGraph::onNodeRemoved, this, &WCanvas::onLNodeRemoved);

    _timer = new QTimer(this);
    connect(_timer, &QTimer::timeout, this, &WCanvas::tick);
    _timer->start(30);
}

WCanvas::~WCanvas()
{
    delete _lastResizedSize;
}

const QMap<short, float> WCanvas::_zoomMultipliers =
{
    {   0, 2.0f  },
    {  -1, 1.75f },
    {  -2, 1.5f  },
    {  -3, 1.25f },
    {  -4, 1.0f  },
    {  -5, 0.9f  },
    {  -6, 0.8f  },
    {  -7, 0.7f  },
    {  -8, 0.6f  },
    {  -9, 0.5f  },
    { -10, 0.4f  },
    { -11, 0.3f  },
    { -12, 0.2f  },
    { -13, 0.1f  },
};

// ------------------------ SERIALIZATION --------------------------


bool WCanvas::serialize(std::fstream *output) const
{
    protocol::Graph graph;
    if (!_graph->protocolize(graph))
    {
        qDebug() << "Couldn't protocolize graph";
        return false;
    }

    *(graph.mutable_offset()) = convertTo_protocolPointF(_offset);
    graph.set_zoom(_zoom);
    graph.SerializeToOstream(output);
    return true;
}

bool WCanvas::deserialize(std::fstream *input)
{
    setUpdatesEnabled(false);

    clear();

    protocol::Graph graph;
    graph.ParseFromIstream(input);

    if (!_graph->deprotocolize(graph))
    {
        qDebug() << "Couldn't deprotocolize graph";
        return false;
    }

    auto *ntm = _graph->getNodeTypeManager();
    auto *ptm = _graph->getPinTypeManager();

    if (ntm) setNodeTypeManager(ntm);
    if (ptm) setPinTypeManager(ptm);

    _offset = convertFrom_protocolPointF(graph.offset());
    _zoom = graph.zoom();

    visualize();

    setUpdatesEnabled(true);
    return true;
}

void WCanvas::visualize()
{
    std::ranges::for_each(_graph->nodes(), [this](LNode *lnode){
        addNode(lnode);
        if (lnode->isSelected())
            _selectedNodes.insert(lnode->ID(), _nodes[lnode->ID()]);
    });
}


// ---------------------- GENERAL FUNCTIONS ---------------------------


void WCanvas::clear()
{ 
    getGraph()->clear(); 
    _offset = QPointF(0, 0);
}

void WCanvas::moveCanvasOnPinDragNearEdge(QPointF mousePosition)
{
    auto lerp = [&](float actual, float max) {
        return std::lerp(0, c_standardPinDragEdgeCanvasMoveValue, actual / max);
    };

    QRect rect = this->rect();
    QRectF top = rect, bottom = rect, left = rect, right = rect;

    int percentOfHeight = rect.height() * c_percentOfCanvasSizeToConsiderNearEdge;
    int percentOfWidth = rect.width() * c_percentOfCanvasSizeToConsiderNearEdge;

    top.setBottom(rect.top() + percentOfHeight);
    bottom.setTop(rect.bottom() - percentOfHeight);
    left.setRight(rect.left() + percentOfWidth);
    right.setLeft(rect.right() - percentOfWidth);


    if (top.contains(mousePosition))
        moveViewUp(lerp(top.bottom() - mousePosition.y(), top.height()));

    if (bottom.contains(mousePosition))
        moveViewDown(lerp(mousePosition.y() - bottom.top(), bottom.height()));

    if (left.contains(mousePosition))
        moveViewLeft(lerp(left.right() - mousePosition.x(), left.width()));

    if (right.contains(mousePosition))
        moveViewRight(lerp(mousePosition.x() - right.left(), right.width()));
}

void WCanvas::moveCanvas(QPointF offset) { _offset -= offset / _zoomMultipliers[_zoom]; }

QPointF WCanvas::mapToCanvas(QPointF point) const
{
    return (point - this->rect().center()) / _zoomMultipliers[_zoom] + _offset;
}

QPoint WCanvas::mapToCanvas(QPoint point) const
{
    return ((point - this->rect().center()) / _zoomMultipliers[_zoom] + _offset.toPoint());
}

void WCanvas::zoom(int times, QPointF where)
{
    if (times == 0) return;
    if (where.x() < 0 || where.y() < 0) where = _mousePosition;

    QPointF initialWhereOnCanvas = mapToCanvas(where);

    _zoom += times;

    if (times < 0)
        _zoom = std::max(_zoom, _zoomMultipliers.firstKey());
    else
        _zoom = std::min(_zoom, _zoomMultipliers.lastKey());

    QPointF whereOffset = mapToCanvas(where) - initialWhereOnCanvas;
    _offset -= whereOffset;
}

void WCanvas::zoomIn(int times, QPointF where) { zoom(times, where); }

void WCanvas::zoomOut(int times, QPointF where) { zoom(-times, where); }

void WCanvas::processSelectionArea(const QMouseEvent *event)
{
    _selectionRect = QRect(_lastMouseDownPosition.toPoint(), event->position().toPoint());
    std::ranges::for_each(_nodes, [&](WANode *node){
        if (_selectionAreaPreviousNodes.contains(node->ID()))
        {
            if (!node->getMappedRect().intersects(*_selectionRect))
            {
                node->setSelected(false);
                _selectionAreaPreviousNodes.remove(node->ID());
            }
        }
        if (node->getMappedRect().intersects(*_selectionRect))
        {
            node->setSelected(true, true);
            _selectionAreaPreviousNodes.insert(node->ID());
        }
    });
}

void WCanvas::setNodeTypeManager(NodeTypeManager *manager)
{
    _graph->setNodeTypeManager(manager);
    _typeBrowser->_nodeTypeManager = manager;
    _typeBrowser->initTypes();
}

void WCanvas::setPinTypeManager(PinTypeManager *manager)
{
    _graph->setPinTypeManager(manager);
    _typeBrowser->_pinTypeManager = manager;

}


// ---------------------------- SLOTS --------------------------------


void WCanvas::onTypeBrowserMove(QVector2D)
{
    QSize desiredWidgetSize = _typeBrowser->getDesiredSize();

    if (_typeBrowser->getPosition().x() + desiredWidgetSize.width() > this->width())
        _typeBrowser->setX(this->width() - desiredWidgetSize.width());

    if (_typeBrowser->getPosition().y() + desiredWidgetSize.height() > this->height())
        _typeBrowser->setY(this->height() - desiredWidgetSize.height());

    if (_typeBrowser->getPosition().x() < 0)
        _typeBrowser->setX(0);

    if (_typeBrowser->getPosition().y() < 0)
        _typeBrowser->setY(0);
}

void WCanvas::tick()
{
    if (_draggedPin)
        moveCanvasOnPinDragNearEdge(_mousePosition);
}

void WCanvas::onPinConnect(IPinData outPin, IPinData inPin)
{
    _graph->connectPins(inPin, outPin);  
}

void WCanvas::onPinConnectionBreak(IPinData outPin, IPinData inPin)
{
    _graph->disconnectPins(inPin, outPin);
}

void WCanvas::onPinDrag(IPinDragSignal signal)
{
    switch (signal.type())
    {
    case EPinDragSignalType::Enter:
    {
        _nodes[signal.source().nodeID]->setPinConnected(signal.source().pinID, true);
        if (_draggedPinTargetInfo)
            _draggedPinTargetInfo.value() = signal.source();
        break;
    }
    case EPinDragSignalType::Leave:
    {
        _nodes[signal.source().nodeID]->setPinConnected(signal.source().pinID, false);
        if (_draggedPinTargetInfo)
            _draggedPinTargetInfo.value() = std::nullopt;
        break;
    }
    case EPinDragSignalType::Start:
    {
        _draggedPin = signal.source();
        _draggedPinTargetInfo.emplace(std::nullopt);
        break;
    }
    case EPinDragSignalType::End:
    {
        _draggedPin = std::nullopt;
        _draggedPinTargetInfo = std::nullopt;
        break;
    }
    default:;
    }
}

void WCanvas::onIsSelectedChanged(bool selected, uint32_t nodeID)
{
    switch (selected)
    {
    case true: _selectedNodes.insert(nodeID, _nodes[nodeID]); 
        return;
    case false: _selectedNodes.remove(nodeID); 
        return;
    }
}

void WCanvas::onNodeSelect(bool bIsMultiSelectionModifierDown, uint32_t nodeID)
{
    if (bIsMultiSelectionModifierDown) return;

    std::ranges::for_each(_selectedNodes.values(), [&](WANode *ptr){
        if (ptr->ID() == nodeID)
            return;
        ptr->setSelected(false);
    });
}

LNode *WCanvas::addNode(QPoint canvasPosition, QString name)
{
    LNode *node = _graph->addNode(canvasPosition, name);
    return addNode(node);
}

LNode *WCanvas::addNode(LNode *lnode)
{   
    if (!_graph->containsNode(lnode->ID()))
        _graph->addNode(lnode);
    
    uint32_t id = _nodes.insert(lnode->ID(), 
            _graph->getFactory()->makeSuitableWNode(lnode, this)
        ).key();

    _nodes[id]->show();

    connect(lnode, &LNode::onIsSelectedChanged, this, &WCanvas::onIsSelectedChanged);

    connect(_nodes[id], &WANode::onPinDrag, this, &WCanvas::onPinDrag);
    connect(_nodes[id], &WANode::onPinConnect, this, &WCanvas::onPinConnect);
    connect(_nodes[id], &WANode::onSelect, this, &WCanvas::onNodeSelect);
    connect(_nodes[id], &WANode::onPinConnectionBreak, this, &WCanvas::onPinConnectionBreak);

    return lnode;
}

LNode *WCanvas::addNode(QPoint canvasPosition, int typeID)
{
    LNode *node = _graph->addNode(canvasPosition, typeID);
    return addNode(node);
}

void WCanvas::onLNodeRemoved(uint32_t id)
{
    WANode *ptr = _nodes[id];
    _nodes.remove(id);
    if (_selectedNodes.contains(id)) 
        _selectedNodes.remove(id);

    delete ptr;
    onNodeRemoved(id);
}


// --------------------------- EVENTS ----------------------------------


void WCanvas::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Delete && !_selectedNodes.isEmpty())
        std::ranges::for_each(_selectedNodes.keys(), [&](uint32_t id) {
            _graph->removeNode(id);
        });

    QWidget::keyPressEvent(event);
}

void WCanvas::mousePressEvent(QMouseEvent *event)
{
    switch (event->button())
    {
    case Qt::MouseButton::RightButton:
        _lastMouseDownPosition = event->position();
        this->setCursor(QCursor(Qt::CursorShape::OpenHandCursor));
        break;
    case Qt::MouseButton::LeftButton:
        _lastMouseDownPosition = event->position();
        if (event->modifiers() & c_multiSelectionModifier)
            break;

        std::ranges::for_each(_selectedNodes.values(), [&](WANode *ptr){
            ptr->setSelected(false);
        });

        _selectedNodes.clear();
        break;
    default:;
    }
}

void WCanvas::mouseMoveEvent(QMouseEvent *event)
{
    QPointF offset;
    switch (event->buttons())
    {
    case Qt::MouseButton::RightButton:
        offset = event->position() - _lastMouseDownPosition;
        moveCanvas(offset);
        _lastMouseDownPosition = event->position();
        break;
    case Qt::MouseButton::LeftButton:
        processSelectionArea(event);
        break;
    default:;
    }
    _mousePosition = event->position();
}

void WCanvas::mouseReleaseEvent(QMouseEvent *event)
{
    switch (event->button())
    {
    case Qt::MouseButton::RightButton:
        this->setCursor(QCursor(Qt::CursorShape::ArrowCursor));
        break;
    case Qt::MouseButton::LeftButton:
        _selectionRect = std::nullopt;
        _selectionAreaPreviousNodes.clear();
        break;
    default:;
    }
}

void WCanvas::resizeEvent(QResizeEvent *event)
{
    QSize oldSize = _lastResizedSize ? *_lastResizedSize : event->oldSize();

    auto approxEqual = [](int x, int y){
        const int approximation = 3;
        return abs(x - y) < approximation;
    };

    bool widgetBoundToRight = approxEqual(_typeBrowser->getPosition().x() + _typeBrowser->getDesiredSize().width(), oldSize.width());
    bool widgetBoundToBottom = approxEqual(_typeBrowser->getPosition().y() + _typeBrowser->getDesiredSize().height(), oldSize.height());
    bool widgetBoundToLeft = approxEqual(_typeBrowser->getPosition().x(), 0);
    bool widgetBoundToTop = approxEqual(_typeBrowser->getPosition().y(), 0);

    // manage NFWidget position
    if (widgetBoundToRight)
        _typeBrowser->setX(event->size().width() - _typeBrowser->getDesiredSize().width());
    if (widgetBoundToLeft)
        _typeBrowser->setX(0);

    if (!widgetBoundToLeft && !widgetBoundToRight)
    {
        float widthDiff = event->size().width() - oldSize.width();
        if (widthDiff)
        {
            float widthDiffCoeff = widthDiff / oldSize.width();
            float xCenter = _typeBrowser->getPosition().x() + _typeBrowser->getDesiredSize().width() / 2.0f;
            _typeBrowser->adjustPosition(xCenter * widthDiffCoeff, 0);
        }
    }


    if (widgetBoundToBottom)
        _typeBrowser->setY(event->size().height() - _typeBrowser->getDesiredSize().height());
    if (widgetBoundToTop)
        _typeBrowser->setY(0);

    if (!widgetBoundToTop && !widgetBoundToBottom)
    {
        float heightDiff = event->size().height() - oldSize.height();
        if (heightDiff)
        {
            float heightDiffCoeff = heightDiff / oldSize.height();
            float yCenter = _typeBrowser->getPosition().y() + _typeBrowser->getDesiredSize().height() / 2.0f;
            _typeBrowser->adjustPosition(0, yCenter * heightDiffCoeff);
        }
    }

    if (_lastResizedSize)
        *_lastResizedSize = event->size();
    else
        _lastResizedSize = new QSize(event->size());

}

// accumulative zoom delta is used for mice with finer-resolution wheels
// https://doc.qt.io/qt-6/qwheelevent.html#angleDelta
void WCanvas::wheelEvent(QWheelEvent *event)
{
    static int _accumulativeZoomDelta = 0;
    _accumulativeZoomDelta += event->angleDelta().y();
    if (_accumulativeZoomDelta >= 120)
    {
        short zoomSteps = qFloor(_accumulativeZoomDelta / 120);
        _accumulativeZoomDelta = _accumulativeZoomDelta % 120;

        zoomIn(zoomSteps, event->position());
    }
    else if (_accumulativeZoomDelta <= -120)
    {
        short zoomSteps = qFloor(_accumulativeZoomDelta / -120);
        _accumulativeZoomDelta = _accumulativeZoomDelta % 120;

        zoomOut(zoomSteps, event->position());
    }
}

void WCanvas::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat(c_mimeFormatForNodeFactory))
    {
        event->setDropAction(Qt::CopyAction);
        event->acceptProposedAction();
        QByteArray byteArray = event->mimeData()->data(c_mimeFormatForNodeFactory);
        INodeSpawnData data = INodeSpawnData::fromByteArray(byteArray);

        if (data.typeID)
            addNode(mapToCanvas(event->position()).toPoint(), *data.typeID);
        else  
            addNode(mapToCanvas(event->position()).toPoint(), data.name);
    }
}

void WCanvas::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat(c_mimeFormatForPinConnection) ||
        event->mimeData()->hasFormat(c_mimeFormatForNodeFactory))
    {
        event->setDropAction(Qt::CopyAction);
        event->acceptProposedAction();
    }
}

void WCanvas::dragMoveEvent(QDragMoveEvent *event)
{
    if (_draggedPin && event->mimeData()->hasFormat(c_mimeFormatForPinConnection))
    {
        QPoint mousePos = event->position().toPoint();
        _draggedPinTarget = mousePos;
    }
    _mousePosition = event->position();
}


// -------------------------- PAINT -----------------------------------


void WCanvas::paintEvent(QPaintEvent *event)
{
    _painter->begin(this);
    _painter->setRenderHint(QPainter::Antialiasing, true);
    paint(_painter, event);
    _painter->end();
}

void WCanvas::paint(QPainter *painter, QPaintEvent *event)
{
    setUpdatesEnabled(false);

    auto getColorOfPinByPinData = [&](const IPinData &data){
        const LPin *pin = _graph->nodes()[data.nodeID]->pin(data.pinID).value();
        return pin->getColor();
    };

    QPen pen(Qt::SolidLine);
    pen.setColor(c_dotsColor);
    painter->setPen(pen);

    QRect rectangle = this->rect();

    QPoint center = rectangle.center();

    int halfWidth = center.x();
    int halfHeight = center.y();

    float zoomMult = _zoomMultipliers[_zoom];
    float dotPaintGapZoomedf = zoomMult * _dotPaintGap;

    // this line is responsible for increasing the dots' gap when it's too small
    int dotPaintGapZoomed = static_cast<int>(dotPaintGapZoomedf < _dotPaintGap / 2 ? dotPaintGapZoomedf * 6 : dotPaintGapZoomedf);

    auto calculateFirstDotCoord = [&](const int &half, const float &positionCoord) {
        return static_cast<int>( (half % dotPaintGapZoomed) - zoomMult * positionCoord ) % dotPaintGapZoomed;
    };

    int leftDotCoordX = calculateFirstDotCoord(halfWidth, _offset.x());
    int topDotCoordY = calculateFirstDotCoord(halfHeight, _offset.y());

    for (int x = leftDotCoordX; x < rectangle.width(); x += dotPaintGapZoomed)
    {
        for (int y = topDotCoordY; y < rectangle.height(); y += dotPaintGapZoomed)
        {
            painter->drawPoint(x, y);
        }
    }

    // manage NODES
    std::ranges::for_each(_nodes, [&](WANode *node) {

        // this->rect()->center() is used instead of center purposefully
        // in order to fix flicking and lagging of the nodes (dk why it fixes the problem)
        const QPointF offset = 
            zoomMult * (node->getLogical()->canvasPosition() - _offset) 
            + this->rect().center();

        node->move(offset.toPoint());
        node->setFixedSize(node->normalSize() * zoomMult);
    });


    // draw SELECTION RECT
    if (_selectionRect)
    {
        pen.setStyle(Qt::PenStyle::DashLine);
        pen.setColor(c_selectionRectColor);
        painter->setPen(pen);

        painter->drawRect(*_selectionRect);
        pen.setStyle(Qt::SolidLine);
        painter->setPen(pen);
    }


    // draw PINS CONNECTIONS
    {
        pen.setWidth(c_pinConnectLineWidth * zoomMult);
        painter->setPen(pen);
        // manage currently dragged pin
        if (_draggedPin && _draggedPinTargetInfo)
        {
            bool bThereIsTargetPin = static_cast<bool>(_draggedPinTargetInfo.value());
            QPoint origin = _nodes[_draggedPin->nodeID]->getOutlineCoordinateForPinID(_draggedPin->pinID);
            QPoint target = bThereIsTargetPin
                                ? _nodes[_draggedPinTargetInfo.value()->nodeID]->
                getOutlineCoordinateForPinID(_draggedPinTargetInfo.value()->pinID)
                                : _draggedPinTarget;

            // origin is always either an out-pin or the cursor
            if (_draggedPin->pinDirection != EPinDirection::Out)
                std::swap(origin, target);

            QLinearGradient gradient(origin, target);



            QColor color0 = getColorOfPinByPinData(*_draggedPin);
            QColor color1 = bThereIsTargetPin ? getColorOfPinByPinData(*_draggedPinTargetInfo.value()) : color0;

            if (_draggedPin->pinDirection == EPinDirection::In)
                std::swap(color0, color1);

            gradient.setColorAt(0, color0);
            gradient.setColorAt(1, color1);
            pen.setBrush(QBrush(gradient));
            painter->setPen(pen);

            painter->drawPath(standardPath(origin, target, zoomMult));
        }

        // draw all existing pins connections
        std::ranges::for_each(_graph->getConnections().asKeyValueRange(), [&](std::pair<IPinData, IPinData> pair) {
            // connections are being drawed from out- to in-pins only
            if (pair.first.pinDirection == EPinDirection::In) return;

            QPoint origin = _nodes[pair.first.nodeID]->getOutlineCoordinateForPinID(pair.first.pinID);
            QPoint target = _nodes[pair.second.nodeID]->getOutlineCoordinateForPinID(pair.second.pinID);

            QLinearGradient gradient(origin, target);
            gradient.setColorAt(0, getColorOfPinByPinData(pair.first));
            gradient.setColorAt(1, getColorOfPinByPinData(pair.second));
            pen.setBrush(QBrush(gradient));
            painter->setPen(pen);

            painter->drawPath(standardPath(origin, target, zoomMult));
        });
    }


    // manage TYPEBROWSER
    {
        _typeBrowser->setFixedSize(_typeBrowser->getDesiredSize());
        _typeBrowser->move(_typeBrowser->getPosition().toPoint());
        _typeBrowser->raise();

    }


    // telemetrics 
    if (_bTelemetricsEnabled) {
        pen.setColor(c_dotsColor);
        painter->setPen(pen);

        auto pointToString = [](QPoint point) {
            return QString::number(point.x()) + ", " + QString::number(point.y());
        };

        auto pointfToString = [](QPointF point) {
            return QString::number(point.x()) + ", " + QString::number(point.y());
        };

        auto parseSet = [](std::set<uint32_t> set) {
            QString str = "[";
            std::ranges::for_each(set, [&str](uint32_t num) {
                str += " ";
                str += QString::number(num);
                str += ",";
            });
            return str.removeLast().append(" ]");
        };

        QPoint mouseCanvasPosition = mapToCanvas(_mousePosition.toPoint());
        painter->drawText(QPoint(20, 20), QString( "Mouse on canvas: " + pointToString(mouseCanvasPosition) ));
        painter->drawText(QPoint(20, 40), QString( "Mouse in viewport: " + pointfToString(_mousePosition) ));
        painter->drawText(QPoint(20, 60), QString( "Size: " + QString::number(rectangle.width()) + ", " + QString::number(rectangle.height()) ));
        painter->drawText(QPoint(20, 80), QString( "Center: " + pointfToString(_offset) ));
        painter->drawText(QPoint(20, 100), QString( "Zoom: " + QString::number(_zoom) ));
        painter->drawText(QPoint(20, 120), QString( "Drag pos: " + pointToString(_draggedPinTarget) ));
        painter->drawText(QPoint(20, 140), QString( "Node IDs: " + parseSet(_graph->getTakenIDs()) ));
        painter->drawText(QPoint(20, 160), QString( "Pin IDs: " + parseSet(LNode::getTakenPinIDs()) ));
    }

    setUpdatesEnabled(true);
}

}
