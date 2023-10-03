#include <QPen>
#include <QRect>
#include <QtDebug>
#include <QApplication>
#include <algorithm>

#include "GraphWidgets/Abstracts/basenode.h"
#include "GraphWidgets/canvas.h"
#include "utility.h"
#include "constants.h"
#include "GraphWidgets/pin.h"

#include "GraphWidgets/Abstracts/moc_basenode.cpp"

namespace qtgraph {

WANode::WANode(WCanvas *canvas)
    : QWidget{ canvas }
    , _parentCanvas{ canvas }
    , _ID{ canvas->newID() }
    , _zoom{ _parentCanvas->getZoomMultiplier() }
    , _painter{ new QPainter() }
    , _canvasPosition{ QPointF(0, 0) }
    , _hiddenPosition{ QPointF() }
    , _bIsSelected{ false }
    , _lastMouseDownPosition{ QPointF(0, 0) }
    , _mousePressPosition{ QPointF(0, 0) }
    , _name{ QString("") }
    , _pinsOutlineCoords{ QMap<uint32_t, QPoint>() }
    , _pins{ QMap<uint32_t, WPin*>() }
{
    _normalSize.setWidth(200);
    _normalSize.setHeight(150);

    this->setFixedSize(_normalSize);

    connect(this, &WANode::onSelect, this, [this](){
        _bIsSelected = true;
    });
}

WANode::~WANode()
{
    delete _painter;
    std::ranges::for_each(_pins, [](WPin *pin) { delete pin; });
}

IDGenerator WANode::_IDgenerator = IDGenerator();


// ---------------- SERIALIZATION -----------------


void WANode::protocolize(protocol::Node *pNode) const
{
    *(pNode->mutable_canvas_position()) = convertTo_protocolPointF(_canvasPosition);
    pNode->set_id(_ID);
    pNode->set_is_selected(_bIsSelected);
    pNode->set_name(_name.toStdString());
    std::ranges::for_each(_pins, [pNode](WPin *pin) {
        pin->protocolize(pNode->add_pins());
    });
}

void WANode::deprotocolize(const protocol::Node &pNode)
{
    setSelected(pNode.is_selected());
    setCanvasPosition(convertFrom_protocolPointF(pNode.canvas_position()));
    setName(QString::fromStdString(pNode.name()));

    std::ranges::for_each(pNode.pins(), [this](const protocol::Pin &pn){
        WPin *pin;

        // if (pn.has_type())
        //     pin = new Pin(this);
        // else
        pin = new Pin(this);

        // pin automatically gets new id as it's created
        // that's what we don't need in case of deserialization
        _IDgenerator.removeTaken(pin->ID());
        pin->setID(pn.id());
        _IDgenerator.addTaken(pin->ID());

        pin->deprotocolize(pn);
        addPin(pin);
    });
}


// ------------------- GENERAL --------------------


void WANode::setPinConnection(uint32_t pinID, IPinData connectedPin)
{
    _pins[pinID]->setFakeConnected(true);
    _pins[pinID]->addConnectedPin(connectedPin);
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

bool WANode::hasPinConnections() const
{
    return std::ranges::any_of(_pins, [&](WPin *pin){
        return !pin->getConnectedPins().isEmpty();
    });
}

QSharedPointer< QMap<uint32_t, QVector<IPinData> > > WANode::getPinConnections() const
{
    auto out = QSharedPointer<QMap<uint32_t, QVector<IPinData> >>(new QMap<uint32_t, QVector<IPinData> >());
    std::ranges::for_each(_pins, [&](WPin *pin){
        out->insert(pin->ID(), pin->getConnectedPins());
    });
    return out;
}

void WANode::removePinConnection(uint32_t pinID, uint32_t connectedPinID)
{
    _pins[pinID]->removeConnectedPinByID(connectedPinID);
}


// -------------------- SLOTS ---------------------


void WANode::onPinDestroyed(QObject *obj)
{
    if (obj == nullptr) qDebug() << "Pointer to destroyed pin is nullptr";

    _IDgenerator.removeTaken( ((WPin*)obj)->ID() );
}

void WANode::addPin(WPin *pin)
{
    _pins.insert(pin->ID(), pin);
    connect(pin, &WPin::onDrag, this, &WANode::slot_onPinDrag);
    connect(pin, &WPin::onConnect, this, &WANode::slot_onPinConnect);
    connect(pin, &WPin::onConnectionBreak, this, &WANode::slot_onPinConnectionBreak);
    connect(pin, &WPin::destroyed, this, &WANode::onPinDestroyed);
    pin->show();
}

void WANode::addPin(QString text, EPinDirection direction, QColor color)
{
    Pin *newPin = new Pin(this);
    uint32_t id = newPin->ID();
    
    _pinsOutlineCoords.insert(id, QPoint(0, 0));
    newPin->setColor(color);
    newPin->setText(text);
    newPin->setDirection(direction);
    addPin(newPin);
}

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
        _hiddenPosition = _canvasPosition;
    }
}

void WANode::mouseReleaseEvent(QMouseEvent *event)
{
    this->setCursor(QCursor(Qt::CursorShape::ArrowCursor));
    onSelect(event->modifiers() & c_multiSelectionModifier, _ID);
}

void WANode::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::MouseButton::LeftButton)
    {
        if ((event->position() - _mousePressPosition).manhattanLength()
            > QApplication::startDragDistance())
        {
            this->setCursor(QCursor(Qt::CursorShape::OpenHandCursor));
            if (!_bIsSelected)
                onSelect(event->modifiers() & c_multiSelectionModifier, _ID);
        }

        QPointF offset = mapToParent(event->position()) - _lastMouseDownPosition;
        if (_parentCanvas->getSnappingEnabled())
        {
            _hiddenPosition += (offset / _zoom);
            _canvasPosition = snap(_hiddenPosition, _parentCanvas->getSnappingInterval());
        }
        else
            _canvasPosition += (offset / _zoom);

        _lastMouseDownPosition = mapToParent(event->position());
    }
}


// ----------------- PAINT HELPERS ------------------


void WANode::paintSimplifiedName(QPainter *painter, int desiredWidth, QPoint textOrigin)
{
    QSize nameBounding = painter->fontMetrics().size(Qt::TextSingleLine, _name);
    float roundingRadiusZoomed = _zoom * c_nodeRoundingRadius;
    float nameRoundedRectRoundingRadius = roundingRadiusZoomed * 0.1f;



    QRect bounded = painter->boundingRect(QRect(textOrigin.x(), textOrigin.y(), desiredWidth, nameBounding.height() * 2),
                                          Qt::AlignCenter, _name);

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
    QSize nameBounding = painter->fontMetrics().size(Qt::TextSingleLine, _name);
    return nameBounding.height() * 1.5 + c_normalPinD * _zoom;
}

void WANode::paintName(QPainter *painter, int desiredWidth, QPoint textOrigin)
{
    QSize nameBounding = painter->fontMetrics().size(Qt::TextSingleLine, _name);
    QFont font = standardFont(c_nodeNameSize * _zoom);
    painter->setFont(font);

    painter->drawText(QRect(textOrigin.x(), textOrigin.y(), desiredWidth, nameBounding.height() * 2),
                      (Qt::AlignVCenter | Qt::AlignHCenter), _name);
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

        std::ranges::for_each(_pins, [&](WPin *pin){
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

    // TODO: write lambda
    int inPins = std::ranges::count_if(_pins, &WPin::static_isInPin);
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
    if (_bIsSelected)
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

        auto manage = [&](WPin *pin){
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

            pin->setFixedSize(pin->getDesiredWidth(_zoom), pin->getNormalD() * _zoom);
            _pinsOutlineCoords[pin->ID()] = QPoint(pin->isInPin() ? 0 : desiredWidth, pin->getCenter().y());

            if (pin->isConnected())
            {
                pen.setColor(pin->getColor());
                painter->setPen(pen);
                painter->drawLine(_pinsOutlineCoords[pin->ID()], pin->getCenter());
            }
        };

        std::ranges::for_each(_pins, manage);
    }
}

}

