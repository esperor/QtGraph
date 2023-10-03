#include <QPen>
#include <QRect>
#include <QtDebug>
#include <QApplication>
#include <algorithm>

#include "widgets/node.h"
#include "widgets/canvas.h"
#include "utilities/utility.h"
#include "utilities/constants.h"
#include "widgets/pin.h"

#include "widgets/moc_node.cpp"

namespace qtgraph {

WANode::WANode(WCanvas *canvas)
    : QWidget{ canvas }
    , _parentCanvas{ canvas }
    , _zoom{ _parentCanvas->getZoomMultiplier() }
    , _painter{ new QPainter() }
    , _hiddenPosition{ QPointF() }
    , _lastMouseDownPosition{ QPointF(0, 0) }
    , _mousePressPosition{ QPointF(0, 0) }
    , _pinsOutlineCoords{ QMap<uint32_t, QPoint>() }
    , _pins{ QMap<uint32_t, QSharedPointer<WPin>>() }
{
    _normalSize.setWidth(200);
    _normalSize.setHeight(150);

    this->setFixedSize(_normalSize);
}

WANode::~WANode()
{
    delete _painter;
}

// ------------------- GENERAL --------------------


void WANode::setSelected(bool b, bool bIsMultiSelectionModifierDown = false)
{ 
    if (b) onSelect(bIsMultiSelectionModifierDown, ID()); 
}

void WANode::setPinConnected(uint32_t pinID, bool isConnected)
{
    _pins[pinID]->setFakeConnected(isConnected);
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

void WANode::addPin(WPin *pin)
{
    _pins.insert(pin->getLogical()->ID(), QSharedPointer<WPin>(pin));
    connect(pin, &WPin::onDrag, this, &WANode::slot_onPinDrag);
    connect(pin, &WPin::onConnect, this, &WANode::slot_onPinConnect);
    connect(pin, &WPin::onConnectionBreak, this, &WANode::slot_onPinConnectionBreak);
    pin->show();
    _pinsOutlineCoords.insert(pin->getLogical()->ID(), QPoint(0, 0));
}

// -------------------- SLOTS ---------------------


void WANode::slot_onPinDrag(IPinDragSignal signal)
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
    onPinDrag(signal);
}

void WANode::slot_onPinConnect(IPinData outPin, IPinData inPin)
{ onPinConnect(outPin, inPin); }

void WANode::slot_onPinConnectionBreak(IPinData outPin, IPinData inPin)
{ onPinConnectionBreak(outPin, inPin); }


// -------------------- EVENTS ---------------------


void WANode::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::MouseButton::LeftButton)
    {
        _lastMouseDownPosition = mapToParent(event->position());
        _mousePressPosition = event->position();
        _hiddenPosition = _lnode->canvasPosition();
    }
}

void WANode::mouseReleaseEvent(QMouseEvent *event)
{
    this->setCursor(QCursor(Qt::CursorShape::ArrowCursor));
    onSelect(event->modifiers() & c_multiSelectionModifier, _lnode->ID());
}

void WANode::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::MouseButton::LeftButton)
    {
        if ((event->position() - _mousePressPosition).manhattanLength()
            > QApplication::startDragDistance())
        {
            this->setCursor(QCursor(Qt::CursorShape::OpenHandCursor));
            if (!_lnode->isSelected())
                onSelect(event->modifiers() & c_multiSelectionModifier, _lnode->ID());
        }

        QPointF offset = mapToParent(event->position()) - _lastMouseDownPosition;
        if (_parentCanvas->getSnappingEnabled())
        {
            _hiddenPosition += (offset / _zoom);
            _lnode->setCanvasPosition(snap(_hiddenPosition, _parentCanvas->getSnappingInterval()));
        }
        else
            _lnode->moveCanvasPosition(offset / _zoom);

        _lastMouseDownPosition = mapToParent(event->position());
    }
}


// ----------------- PAINT HELPERS ------------------


void WANode::paintSimplifiedName(QPainter *painter, int desiredWidth, QPoint textOrigin)
{
    QSize nameBounding = painter->fontMetrics().size(Qt::TextSingleLine, _lnode->getName());
    float roundingRadiusZoomed = _zoom * c_nodeRoundingRadius;
    float nameRoundedRectRoundingRadius = roundingRadiusZoomed * 0.1f;



    QRect bounded = painter->boundingRect(QRect(textOrigin.x(), textOrigin.y(), desiredWidth, nameBounding.height() * 2),
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
    QSize nameBounding = painter->fontMetrics().size(Qt::TextSingleLine, _lnode->getName());
    return nameBounding.height() * 1.5 + c_normalPinD * _zoom;
}

void WANode::paintName(QPainter *painter, int desiredWidth, QPoint textOrigin)
{
    QSize nameBounding = painter->fontMetrics().size(Qt::TextSingleLine, _lnode->getName());
    QFont font = standardFont(c_nodeNameSize * _zoom);
    painter->setFont(font);

    painter->drawText(QRect(textOrigin.x(), textOrigin.y(), desiredWidth, nameBounding.height() * 2),
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

    auto calculateWidth = [&](){
        int maxInWidth = 0, maxOutWidth = 0;

        std::ranges::for_each(_pins, [&](const QSharedPointer<WPin> &pin){
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

        return maxInWidth + maxOutWidth + normalPinDZoomed * (bShouldSimplifyRender ? 8 : 4);
    };

    int inPins = std::ranges::count_if(_pins, [](const QSharedPointer<WPin> &pin){
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

    QPoint desiredOrigin = mapFromParent(QPoint(this->pos()));

    QPainterPath path;

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

        auto manage = [&](const QSharedPointer<WPin> &pin){
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

            const QSharedPointer<LPin> &logicalPin = logicalPin;

            pin->setFixedSize(pin->getDesiredWidth(_zoom), pin->getNormalD() * _zoom);
            _pinsOutlineCoords[logicalPin->ID()] = QPoint(pin->isInPin() ? 0 : desiredWidth, pin->getCenter().y());

            if (logicalPin->isConnected())
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

