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


// protobuf
#include "data.pb.h"

#include "widgets/moc_canvas.cpp"

namespace qtgraph {

WCanvas::WCanvas(QWidget *parent)
    : QWidget{ parent }
    , _painter{ new QPainter() }
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
    , _selectionRect{ std::nullopt }
    , _selectionAreaPreviousNodes{ QSet<uint32_t>() }
    , _connectedPins{ QMultiMap<IPinData, IPinData>() }
    , _typeBrowser{ new TypeBrowser(this) }
    , _selectedNodes{ QMap<uint32_t, QSharedPointer<WANode>>() }
{
    setMouseTracking(true);
    setAutoFillBackground(true);
    setFocusPolicy(Qt::FocusPolicy::StrongFocus);

    QPalette palette(c_paletteDefaultColor);
    this->setPalette(palette);
    setAcceptDrops(true);

    _typeBrowser->show();

    connect(_typeBrowser, &TypeBrowser::onMove, this, &WCanvas::onTypeBrowserMove);

    _timer = new QTimer(this);
    connect(_timer, &QTimer::timeout, this, &WCanvas::tick);
    _timer->start(30);
}

WCanvas::~WCanvas()
{
    delete _painter;
    delete _timer;
    delete _typeBrowser;
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
    return graph->serialize(output);
}

bool WCanvas::deserialize(std::fstream *input)
{
    setUpdatesEnabled(false);
    protocol::Data data;
    data.ParseFromIstream(input);

    // canvas
    protocol::State state = data.state();
    setNodeTypeManager(NodeTypeManager::fromProtocolTypeManager(state.node_type_manager()));
    setPinTypeManager(PinTypeManager::fromProtocolTypeManager(state.pin_type_manager()));
    _offset = convertFrom_protocolPointF(state.offset());
    _zoom = state.zoom();
    _snappingInterval = state.snapping_interval();
    _bIsSnappingEnabled = state.is_snapping_enabled();

    // nodes
    std::ranges::for_each(state.nodes(), [this](const protocol::Node &nd){
        WANode *node;
        if (nd.has_type()) 
            node = _factory->makeNodeOfType(nd.type(), this);
        else
            node = new WANode(this);

        // node automatically gets new id as it's created
        // that's what we don't need in case of deserialization
        _IDgenerator.removeTaken(node->ID());
        node->setID(nd.id());
        _IDgenerator.addTaken(node->ID());

        node->deprotocolize(nd);
        addNode(node);

        if (nd.is_selected())
            _selectedNodes.insert(nd.id(), _nodes[nd.id()]);
    });

    // structure
    readStructure(data.structure());

    setUpdatesEnabled(true);
    return true;
}

bool WCanvas::writeStructure(protocol::Structure *structure) const
{
    auto *edges = structure->mutable_edges();
    std::ranges::for_each(_connectedPins.keys(), [this, edges](IPinData key){
        QList<IPinData> values = this->_connectedPins.values(key);

        protocol::IntArray arr;
        std::ranges::for_each(values, [&arr](IPinData &value){ arr.add_elements(value.pinID); });

        (*edges)[key.pinID] = arr;
    });
    return true;
}

// this function can perhaps be optimized 
// (but probably not without pain in the ass)
bool WCanvas::readStructure(const protocol::Structure &structure)
{
    std::map<uint32_t, uint32_t> pin_node_map = {};
    int l = _nodes.size();
    std::ranges::for_each(_nodes, [this, &pin_node_map](const QSharedPointer<WANode> node){
        std::ranges::for_each(node->getPinIDs(), [&node, &pin_node_map](uint32_t id) {
            pin_node_map.insert({id, node->ID()});
        });
    });
    std::ranges::for_each(structure.edges(), [this, &pin_node_map](std::pair<uint32_t, protocol::IntArray> pair){
        IPinData first = _nodes[pin_node_map[pair.first]]->getPinByID(pair.first)->getData();

        std::ranges::for_each(*pair.second.mutable_elements(), 
        [this, &pin_node_map, &first](uint32_t &id){
            IPinData second = _nodes[pin_node_map[id]]->getPinByID(id)->getData();
            _nodes[second.nodeID]->setPinConnection(second.pinID, first);  
            _nodes[first.nodeID]->setPinConnection(first.pinID, second);
            _connectedPins.insert(first, second);
        });
    });
    return true;
}


// ---------------------- GENERAL FUNCTIONS ---------------------------


QString WCanvas::getPinText(uint32_t nodeID, uint32_t pinID) const
{
    return _nodes[nodeID]->getPinByID(pinID)->getText();
}
QString WCanvas::getNodeName(uint32_t nodeID) const
{
    return _nodes[nodeID]->getName();
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
    std::ranges::for_each(_nodes, [&](QSharedPointer<WANode> &node){
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
    _nodeTypeManager = *manager;
    _factory->setNodeTypeManager(manager);
    _typeBrowser->_nodeTypeManager = &_nodeTypeManager;
    _typeBrowser->initTypes();
}

void WCanvas::setPinTypeManager(PinTypeManager *manager)
{
    _pinTypeManager = *manager;
    _factory->setPinTypeManager(manager);
    _typeBrowser->_pinTypeManager = &_pinTypeManager;

}



// ---------------------------- SLOTS --------------------------------



void WCanvas::onNodeDestroyed(QObject *obj)
{
    if (obj == nullptr) qDebug() << "Pointer to destroyed node is nullptr";

    _IDgenerator.removeTaken( ((WANode*)obj)->ID() );
}

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
    if (!_connectedPins.contains(outPin, inPin))
    {
        _connectedPins.insert(outPin, inPin);
        _nodes[outPin.nodeID]->setPinConnection(outPin.pinID, inPin);
        _nodes[inPin.nodeID]->setPinConnection(inPin.pinID, outPin);
    }
}

void WCanvas::onPinConnectionBreak(IPinData outPin, IPinData inPin)
{
    auto it = _connectedPins.find(outPin, inPin);
    if (it != _connectedPins.end())
    {
        _connectedPins.erase(it);

        _nodes[outPin.nodeID]->removePinConnection(outPin.pinID, inPin.pinID);
        _nodes[inPin.nodeID]->removePinConnection(inPin.pinID, outPin.pinID);
    }
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

void WCanvas::onNodeSelect(bool bIsMultiSelectionModifierDown, uint32_t nodeID)
{
    _selectedNodes.insert(nodeID, _nodes[nodeID]);

    if (bIsMultiSelectionModifierDown) return;

    std::ranges::for_each(_selectedNodes.values(), [&](QSharedPointer<WANode> &ptr){
        if (ptr->ID() == nodeID)
            return;
        ptr->setSelected(false);
    });

    _selectedNodes.removeIf([&](QMap<uint32_t, QSharedPointer<WANode>>::iterator &it){
        return it.value()->ID() != nodeID;
    });
}

QWeakPointer<WANode> WCanvas::addBaseNode(QPoint canvasPosition, QString name)
{
    WANode *node = new WANode(this);
    node->setCanvasPosition(canvasPosition);
    node->setName(name);
    return addNode(node);
}

QWeakPointer<WANode> WCanvas::addNode(WANode *node)
{
    uint32_t id = node->ID();

    node->setFactory(_factory.get());
    _nodes.insert(id, QSharedPointer<WANode>(node));
    _nodes[id]->show();

    connect(_nodes[id].get(), &WANode::onPinDrag, this, &WCanvas::onPinDrag);
    connect(_nodes[id].get(), &WANode::onPinConnect, this, &WCanvas::onPinConnect);
    connect(_nodes[id].get(), &WANode::onSelect, this, &WCanvas::onNodeSelect);
    connect(_nodes[id].get(), &WANode::onPinConnectionBreak, this, &WCanvas::onPinConnectionBreak);
    connect(_nodes[id].get(), &WANode::destroyed, this, &WCanvas::onNodeDestroyed);

    return QWeakPointer<WANode>(_nodes[id]);
}

QWeakPointer<WANode> WCanvas::addTypedNode(QPoint canvasPosition, int typeID)
{
    TypedNode *node = _factory->makeNodeAndPinsOfType(typeID, this);
    node->setCanvasPosition(canvasPosition);
    return addNode(node);
}

void WCanvas::deleteNode(QSharedPointer<WANode> &ptr)
{
    uint32_t id = ptr->ID();
    if (ptr->hasPinConnections())
    {
        QSharedPointer< QMap<uint32_t, QVector<IPinData> > > connections = ptr->getPinConnections();
        std::ranges::for_each(connections->asKeyValueRange(), [&](std::pair<const uint32_t&, QVector<IPinData>&> pair){
            uint32_t id = pair.first;
            std::ranges::for_each(pair.second, [&](IPinData connectedPin){
                _nodes[connectedPin.nodeID]->removePinConnection(connectedPin.pinID, id);

                const WPin *pin = ptr->getPinByID(id);
                if (pin->getDirection() == EPinDirection::Out)
                    _connectedPins.remove(pin->getData());
                else
                    _connectedPins.remove(connectedPin, pin->getData());
            });
        });
    }
    _nodes.remove(id);
//    _IDgenerator.removeTaken(id);
}


// --------------------------- EVENTS ----------------------------------


void WCanvas::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Delete && !_selectedNodes.isEmpty())
    {
        std::ranges::for_each(_selectedNodes, [&](QSharedPointer<WANode> &ptr){ deleteNode(ptr); });
        _selectedNodes.clear();
        onNodesRemoved();
    }

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

        std::ranges::for_each(_selectedNodes.values(), [&](QSharedPointer<WANode> &ptr){
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
        TypedNodeSpawnData data = TypedNodeSpawnData::fromByteArray(byteArray);

        addTypedNode(mapToCanvas(event->position().toPoint()), data.typeID);
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
        const WPin *pin = _nodes[data.nodeID]->getPinByID(data.pinID);
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
    std::ranges::for_each(_nodes, [&](QSharedPointer<WANode> &node) {

        // this->rect()->center() is used instead of center purposefully
        // in order to fix flicking and lagging of the nodes (dk why it fixes the problem)
        const QPointF offset = zoomMult * (node->canvasPosition() - _offset) + this->rect().center();

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
        std::ranges::for_each(_connectedPins.asKeyValueRange(), [&](std::pair<IPinData, IPinData> pair) {
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
    {
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
        painter->drawText(QPoint(20, 140), QString( "Node IDs: " + parseSet(_IDgenerator.getTakenIDs()) ));
        painter->drawText(QPoint(20, 160), QString( "Pin IDs: " + parseSet(WANode::getTakenPinIDs()) ));
    }

    setUpdatesEnabled(true);
}

}
