#include <QPen>
#include <QRect>
#include <QtDebug>
#include <QApplication>
#include <QVector>
#include <algorithm>
#include <functional>

#include "widgets/node.h"
#include "widgets/canvas.h"
#include "utilities/utility.h"
#include "utilities/constants.h"
#include "widgets/pin.h"
#include "data/node.h"
#include "data/graph.h"

#include "widgets/moc_node.cpp"


namespace qtgraph {

WANode::WANode(const DNode *logical, WCanvas *canvas)
    : QWidget{ canvas }
    , _lnode{ logical }
    , _parentCanvas{ canvas }
    , _zoom{ _parentCanvas->getZoomMultiplier() }
    , _painter{ new QPainter() }
    , _hiddenPosition{ QPointF() }
    , _lastMouseDownPosition{ QPointF(0, 0) }
    , _mousePressPosition{ QPointF(0, 0) }
    , _positionDelta{ QPointF(0, 0) }
    , _pinsOutlineCoords{ QMap<uint32_t, QPoint>() }
    , _pins{ QMap<uint32_t, WPin*>() }
{
    _normalSize.setWidth(200);
    _normalSize.setHeight(150);

    this->setFixedSize(_normalSize);

    std::ranges::for_each(_lnode->pins(), [&, this](const auto &lpin){
        addPin(new WPin(lpin, this));
    });
}

WANode::~WANode()
{
    delete _painter;
}

// ------------------- GENERAL --------------------


void WANode::setPinFakeConnected(uint32_t pinID, bool isConnected)
{
    _pins[pinID]->setFakeConnected(isConnected);
}

void WANode::setNodePosition(QPointF pos)
{
    QVector<const void*> objects = 
    { (void*)new uint32_t(_lnode->ID())
    , (void*)new QPointF(pos)
    , (void*)new QPointF(_lnode->canvasPosition()) 
    };

    IAction *_action = new IAction(
        EAction::Moving,
        "Node movement",
        [](DGraph *g, QVector<const void*> *o)
        {
            uint32_t id = *(uint32_t*)o->at(0);
            auto newPos = (const QPointF*)o->at(1);

            g->nodes()[id]->setCanvasPosition(*newPos);
        },
        [](DGraph *g, QVector<const void*> *o)
        {
            uint32_t id = *(uint32_t*)o->at(0);
            auto oldPos = (const QPointF*)o->at(2);

            g->nodes()[id]->setCanvasPosition(*oldPos);
        },
        [](QVector<const void*> *o)
        {
            delete (uint32_t*)o->at(0);
            delete (const QPointF*)o->at(1);
            delete (const QPointF*)o->at(2);
        },
        objects
    );
    emit action(_action);
}

float WANode::getParentCanvasZoomMultiplier() const
{
    return _parentCanvas->getZoomMultiplier();
}

QRect WANode::getMappedRect() const
{
    QRect nodeRect = this->rect();
    QPoint mappedTopLeft = this->mapToParent(nodeRect.topLeft());
    QRect mapped(mappedTopLeft.x(), mappedTopLeft.y(), nodeRect.width(), nodeRect.height());
    return mapped;
}

QPointF WANode::getCanvasPosition() const
{
    return _lnode->canvasPosition() + _positionDelta.toPointF();
}

void WANode::addPin(WPin *pin)
{
    _pins.insert(pin->getLogical()->ID(), pin);
    connect(pin, &WPin::onDrag, this, &WANode::onPinDrag);
    connect(pin, &WPin::onConnect, this, &WANode::onPinConnect);
    connect(pin, &WPin::onConnectionBreak, this, &WANode::onPinConnectionBreak);
    pin->show();
    _pinsOutlineCoords.insert(pin->getLogical()->ID(), QPoint(0, 0));
}

// -------------------- SLOTS ---------------------


void WANode::onPinDrag(IPinDragSignal signal)
{
    switch (signal.type())
    {
    case EPinDragSignalType::Start:
        _pins[signal.source().pinID]->setFakeConnected(true);
        break;
    case EPinDragSignalType::End:
        _pins[signal.source().pinID]->setFakeConnected(false);
        break;
    default:;
    }
    emit pinDrag(signal);
}


void WANode::onPinConnect(IPinData outPin, IPinData inPin)
{ emit pinConnect(outPin, inPin); }

void WANode::onPinConnectionBreak(IPinData outPin, IPinData inPin)
{ emit pinConnectionBreak(outPin, inPin); }

// -------------------- EVENTS ---------------------


void WANode::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::MouseButton::LeftButton)
    {
        _lastMouseDownPosition = mapToParent(event->position());
        _mousePressPosition = event->position();
        _hiddenPosition = _lnode->canvasPosition();

        bool isMultiSelectionModifierDown = event->modifiers() & c_multiSelectionModifier;
        
        if (isMultiSelectionModifierDown && _lnode->isSelected())
            selectSignal({ false, true, _lnode->ID() });
        else if (_lnode->isSelected())
        
            // this means that selection state shouldn't change neither 
            // on action execution nor inversion
            selectSignal({ {}, isMultiSelectionModifierDown, _lnode->ID() });
        else
            selectSignal({ true, isMultiSelectionModifierDown, _lnode->ID() });
    }
}

void WANode::mouseReleaseEvent(QMouseEvent *event)
{
    this->setCursor(QCursor(Qt::CursorShape::ArrowCursor));

    if (_positionDelta.length() >= c_nodeMoveMinimalDistance) 
        setNodePosition(_lnode->canvasPosition() + _positionDelta.toPointF());
    _positionDelta = { 0, 0 };
}

void WANode::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::MouseButton::LeftButton)
    {
        if ((event->position() - _mousePressPosition).manhattanLength()
            > QApplication::startDragDistance())
            this->setCursor(QCursor(Qt::CursorShape::OpenHandCursor));

        QPointF offset = mapToParent(event->position()) - _lastMouseDownPosition;
        if (_parentCanvas->getSnappingEnabled())
        {
            _hiddenPosition += (offset / _zoom);
            _positionDelta = QVector2D(snap(_hiddenPosition, _parentCanvas->getSnappingInterval()) - _lnode->canvasPosition());
        }
        else
            _positionDelta += QVector2D(offset / _zoom);

        _lastMouseDownPosition = mapToParent(event->position());
    }
}


// ----------------- PAINT HELPERS ------------------


void WANode::paintSimplifiedName(QPainter *painter, int desiredWidth, QPoint textOrigin)
{
    float roundingRadiusZoomed = _zoom * c_nodeRoundingRadius;
    float nameRoundedRectRoundingRadius = roundingRadiusZoomed * 0.1f;

    QRect bounded = painter->boundingRect(QRect(textOrigin.x(), textOrigin.y(), desiredWidth, getNameBounding(painter).height() * 2),
                                          Qt::AlignCenter, _lnode->getName());

    int oldWidth = bounded.width(), oldHeight = bounded.height();
    bounded.setWidth(bounded.width() * c_nodeNameRoundedRectSizeX);
    bounded.setHeight(bounded.height() * c_nodeNameRoundedRectSizeY);
    bounded.translate((oldWidth - bounded.width()) / 2, (oldHeight - bounded.height()) / 2);

    painter->drawRoundedRect(bounded, nameRoundedRectRoundingRadius, nameRoundedRectRoundingRadius);
}

int WANode::calculateRowsOffset(QPainter *painter) const
{
    QFont font = standardFont(c_nodeNameSize * _zoom);
    painter->setFont(font);
    return getNameBounding(painter).height() * 1.5 + c_normalPinD * _zoom;
}

void WANode::paintName(QPainter *painter, int desiredWidth, QPoint textOrigin)
{
    QFont font = standardFont(c_nodeNameSize * _zoom);
    painter->setFont(font);

    painter->drawText(QRect(textOrigin.x(), textOrigin.y(), desiredWidth, getNameBounding(painter).height() * 2),
                      (Qt::AlignVCenter | Qt::AlignHCenter), _lnode->getName());
}


// --------------------- PAINT ----------------------


void WANode::paintEvent(QPaintEvent *event)
{
    _painter->begin(this);
    _painter->setRenderHint(QPainter::Antialiasing, true);
    paint(_painter, event);
    _painter->end();
}

void WANode::paint(QPainter *painter, QPaintEvent *)
{
    _zoom = _parentCanvas->getZoomMultiplier();
    bool bShouldSimplifyRender = _zoom <= c_changeRenderZoomMultiplier;
    int normalPinDZoomed = c_normalPinD * _zoom;

    auto calculateWidth = [&, painter](){
        int maxInWidth = 0, maxOutWidth = 0;

        std::ranges::for_each(_pins, [&](WPin* pin){
            switch (pin->getDirection())
            {
            case EPinDirection::In:
                maxInWidth = std::max(maxInWidth, pin->getDesiredWidth(_zoom));
                break;
            case EPinDirection::Out:
                maxOutWidth = std::max(maxOutWidth, pin->getDesiredWidth(_zoom));
                break;
            default:;
            }
        });

        return std::max(
            maxInWidth + maxOutWidth + normalPinDZoomed * (bShouldSimplifyRender ? 8 : 4),
            (int)(getNameBounding(painter).width() * (bShouldSimplifyRender ? 0.5 : 1) + normalPinDZoomed * 2)
        );
    };

    int inPins = std::ranges::count_if(_pins, [](WPin* pin){
        return pin->getDirection() == EPinDirection::In;
    });
    int pinRows = std::max(inPins, static_cast<int>(_pins.size() - inPins));

    QPen pen(Qt::NoPen);
    painter->setPen(pen);

    int pinsOffsetY = calculateRowsOffset(painter);

    int desiredWidth = calculateWidth();
    int desiredHeight = pinsOffsetY + pinRows * normalPinDZoomed * 2;

    _normalSize.setWidth(desiredWidth / _zoom);
    _normalSize.setHeight(desiredHeight / _zoom);

    int outlineWidth = static_cast<int>(std::min(c_globalOutlineWidth * _zoom, c_nodeMaxOutlineWidth));
    int innerWidth = desiredWidth - outlineWidth * 2;
    int innerHeight = desiredHeight - outlineWidth * 2;

    float innerRoundingRadius = _zoom * c_nodeRoundingRadius;

    QPoint desiredOrigin = getDesiredOrigin();

    QPainterPath path;

    // painter paints inner and outer at the same time
    // where the outer is being drawn by the pen

    // paint OUTER
    if (_lnode->isSelected())
    {
        QPen pen(Qt::SolidLine);
        pen.setColor(c_selectionColor);
        painter->setPen(pen);
    }

    // paint INNER
    {
        painter->setBrush(c_nodesBackgroundColor);

        QRect innerRect(desiredOrigin.x() + outlineWidth, desiredOrigin.y() + outlineWidth, innerWidth, innerHeight);
        path.addRoundedRect(innerRect, innerRoundingRadius, innerRoundingRadius);
        path.setFillRule(Qt::WindingFill);

        painter->drawPath(path);
    }


    // paint NAME
    {
        pen.setStyle(Qt::SolidLine);
        pen.setColor(c_highlightColor);
        painter->setPen(pen);
        painter->setBrush(c_highlightColor);

        const QPoint &textOrigin = desiredOrigin;

        if (bShouldSimplifyRender)
            paintSimplifiedName(painter, desiredWidth, textOrigin);
        else
            paintName(painter, desiredWidth, textOrigin);
    }


    // manage PINS
    {
        pen.setWidth(c_pinConnectLineWidth * _zoom);

        int inPinsOffsetY = pinsOffsetY;
        int outPinsOffsetY = pinsOffsetY;

        auto manage = [&](WPin* pin){
            switch (pin->getDirection())
            {
            case EPinDirection::In:
                pin->move(QPoint(normalPinDZoomed, inPinsOffsetY));
                inPinsOffsetY += 2 * normalPinDZoomed;
                break;
            case EPinDirection::Out:
                pin->move(QPoint(desiredWidth - normalPinDZoomed - pin->getDesiredWidth(_zoom), outPinsOffsetY));
                outPinsOffsetY += 2 * normalPinDZoomed;
                break;
            default:;
            }

            const DPin *logicalPin = pin->getLogical();

            pin->setFixedSize(pin->getDesiredWidth(_zoom), pin->getNormalD() * _zoom);
            _pinsOutlineCoords[logicalPin->ID()] = QPoint(pin->isInPin() ? 0 : desiredWidth, pin->getCenter().y());

            if (logicalPin->isConnected() || pin->isFakeConnected())
            {
                pen.setColor(logicalPin->getColor());
                painter->setPen(pen);
                painter->drawLine(_pinsOutlineCoords[logicalPin->ID()], pin->getCenter());
            }
        };

        std::ranges::for_each(_pins, manage);
    }
}

}

