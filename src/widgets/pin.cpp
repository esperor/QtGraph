#include <QtDebug>
#include <QLabel>
#include <QMimeData>
#include <string>

#include "widgets/pin.h"
#include "widgets/node.h"
#include "utilities/constants.h"
#include "utilities/utility.h"
#include "widgets/canvas.h"
#include "logics/pin.h"

#include "widgets/moc_pin.cpp"

namespace qtgraph {

WPin::WPin(const LPin *lpin, WANode *parent)
    : QWidget{ parent }
    , _lpin{ lpin }
    , _parentNode{ parent }
    , _fakeConnected{ false }
    , _normalD{ c_normalPinD }
    , _breakConnectionActions{ QMap<int, QAction*>() }
    , _contextMenu{ QMenu(this) }
    , _painter{ new QPainter(this) }
{
    setAcceptDrops(true);

    connect(this, &WPin::onConnect, this, [this](){ _fakeConnected = false; });
}

WPin::~WPin() { delete _painter; }


// ------------------- GENERAL ---------------------


QPixmap WPin::getPixmap() const
{
    int width = 0.5 * c_normalPinD * _parentNode->getParentCanvasZoomMultiplier();
    QPixmap pixmap(QSize(width, width));
    pixmap.fill(QColor(255, 255, 255, 0));

//    _painter->begin(&pixmap);
//    _painter->setBrush(_color);
//    _painter->setPen(QPen(Qt::NoPen));
//    _painter->drawEllipse(QRect(0, 0, width, width));
//    _painter->end();
    return pixmap;
}

int WPin::getDesiredWidth(float zoom) const
{
    if (_lpin->getText() == "" || zoom <= c_changeRenderZoomMultiplier)
        return _normalD * zoom;

    QFont font = standardFont(static_cast<int>(_normalD * zoom * c_pinFontSizeCoef));
    QFontMetrics metrics(font);
    int textWidth = metrics.size(Qt::TextSingleLine, _lpin->getText()).width();
    return static_cast<int>(textWidth + _normalD * 2 * zoom);
}

void WPin::startDrag()
{
    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;

    mimeData->setData(c_mimeFormatForPinConnection, _lpin->getData().toByteArray());
    drag->setMimeData(mimeData);

    QPixmap pixmap = getPixmap();
    drag->setPixmap(pixmap);
    drag->setHotSpot(QPoint(pixmap.width() / 2, pixmap.height() / 2));

    onDrag(IPinDragSignal(_lpin->getData(), EPinDragSignalType::Start));
    drag->exec();
    onDrag(IPinDragSignal(_lpin->getData(), EPinDragSignalType::End));
}

void WPin::showContextMenu(const QMouseEvent *event)
{
    _contextMenu.clear();
    _contextMenu.setTitle(_lpin->getText());

    QVector<IPinData> connectedPins = _lpin->getConnectedPins();

    if (!connectedPins.isEmpty())
    {
        QMenu *breakMenu = _contextMenu.addMenu("Break connnection");
        std::ranges::for_each(connectedPins, [&](IPinData data){
            QString nodeName = _parentNode->getParentCanvas()->getGraph_const()->getNodeName(data.nodeID);
            breakMenu->addAction(new QAction("to " + nodeName, breakMenu));
            QAction *action = breakMenu->actions().last();
            action->setData(QVariant(data.toByteArray()));

            connect(action, &QAction::triggered, this, &WPin::onConnectionBreakActionClick);
        });
    }

    if (!_contextMenu.isEmpty())
        _contextMenu.popup(mapToGlobal(event->pos()));
}


// -------------------- EVENTS ---------------------


void WPin::mousePressEvent(QMouseEvent *event)
{
    switch (event->buttons())
    {
    case Qt::LeftButton:
        startDrag();
        break;
    case Qt::RightButton:
        showContextMenu(event);
        break;
    default:;
    }
}

void WPin::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat(c_mimeFormatForPinConnection))
    {
        event->setDropAction(Qt::LinkAction);
        event->acceptProposedAction();
        QByteArray byteArray = event->mimeData()->data(c_mimeFormatForPinConnection);
        IPinData sourceData = IPinData::fromByteArray(byteArray);

        if (_lpin->getData().pinDirection == EPinDirection::Out)
            onConnect(_lpin->getData(), sourceData);
        else
            onConnect(sourceData, _lpin->getData());
    }
}

void WPin::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat(c_mimeFormatForPinConnection))
    {
        IPinData data = IPinData::fromByteArray(event->mimeData()->data(c_mimeFormatForPinConnection));
        EPinDirection sourceDirection = data.pinDirection;
        uint32_t sourceNodeID = data.nodeID;
        if (sourceDirection != _lpin->getData().pinDirection && sourceNodeID != _parentNode->ID())
        {
            event->setDropAction(Qt::LinkAction);
            event->acceptProposedAction();
            onDrag(IPinDragSignal(_lpin->getData(), EPinDragSignalType::Enter));
            return;
        }
    }
}

void WPin::dragLeaveEvent(QDragLeaveEvent *)
{
    onDrag(IPinDragSignal(_lpin->getData(), EPinDragSignalType::Leave));
}


// -------------------- SLOTS ---------------------


void WPin::onConnectionBreakActionClick()
{
    QAction *sender = (QAction*)QObject::sender();
    IPinData data = IPinData::fromByteArray(sender->data().toByteArray());

    IPinData out = getDirection() == EPinDirection::Out ? _lpin->getData() : data;
    IPinData in  = getDirection() == EPinDirection::Out ? data : _lpin->getData();
    onConnectionBreak(out, in);
}


// -------------------- PAINT ---------------------


void WPin::paintEvent(QPaintEvent *event)
{
    _painter->begin(this);
    _painter->setRenderHint(QPainter::Antialiasing, true);
    paint(_painter, event);
    _painter->end();
}

void WPin::paint(QPainter *painter, QPaintEvent *)
{

    float canvasZoom = _parentNode->getParentCanvasZoomMultiplier();

    // D stands for diameter
    int desiredD = _normalD * canvasZoom;

    QPen pen(Qt::NoPen);
    painter->setPen(pen);
    QFont font = standardFont(desiredD * c_pinFontSizeCoef);
    painter->setFont(font);
    painter->setBrush(_lpin->getColor());

    int outlineWidth = c_globalOutlineWidth * canvasZoom;

    QPoint desiredOrigin = mapFromParent(QPoint(this->pos()));
    QPoint textOrigin = QPoint(desiredOrigin.x() + desiredD * 2, desiredOrigin.y());

    QRect rectangle = QRect(desiredOrigin.x(), desiredOrigin.y(), desiredD, desiredD);

    QSize textBounding = painter->fontMetrics().size(Qt::TextSingleLine, _lpin->getText());



    if (_lpin->getData().pinDirection == EPinDirection::Out && !(canvasZoom <= c_changeRenderZoomMultiplier))
    {
        rectangle.setX(this->width() - desiredD);
        rectangle.setWidth(desiredD);
    }

    _center = rectangle.center();


    // Drawing main circle
    painter->drawEllipse(rectangle);

    // If needed, draw inner circle the color of the background
    if (!(canvasZoom <= c_changeRenderZoomMultiplier || _fakeConnected || _lpin->isConnected()))
    {
        painter->setBrush(c_nodesBackgroundColor);
        QRect innerRect = QRect(rectangle.x() + outlineWidth
                              , rectangle.y() + outlineWidth
                              , rectangle.width() - outlineWidth * 2
                              , rectangle.height() - outlineWidth * 2);

        painter->drawEllipse(innerRect);
    }


    // Text-related stuff
    if (_lpin->getText() != "" && !(canvasZoom <= c_changeRenderZoomMultiplier))
    {
        pen.setStyle(Qt::SolidLine);
        pen.setColor(_lpin->getColor());
        painter->setPen(pen);

        if (_lpin->getData().pinDirection == EPinDirection::Out)
            textOrigin.setX(this->width() - desiredD * 2 - textBounding.width());

        painter->drawText(QRect(textOrigin.x(), textOrigin.y(), textBounding.width(), rectangle.height())
                          , Qt::AlignVCenter, _lpin->getText());
    }
}

}