#include <QPaintEvent>
#include <QRect>
#include <QGroupBox>
#include <QApplication>

#include "utilities/constants.h"
#include "logics/typemanager.h"
#include "widgets/typebrowser.h"
#include "utilities/utility.h"

#include "widgets/moc_typebrowser.cpp"

namespace qtgraph {

WTypeBrowser::WTypeBrowser(QWidget *parent)
    : QWidget{ parent }
    , _painter{ new QPainter(this) }
    , _gap{ 20 }
    , _bIsMinimized{ false }
    , _position{ QPointF() }
    , _lastMouseDownPosition{ QPointF() }
    , _mousePressPosition{ QPointF() }
    , _layoutHolder{ new QWidget(this) }
    , _layout{ new QBoxLayout(QBoxLayout::TopToBottom, _layoutHolder) }
    , _btnMinimize{ new NFButtonMinimize(this) }
    , _nodeImages{ QVector<WTypedNodeImage*>() }
{
    setMouseTracking(true);
    _layoutHolder->setLayout(_layout);

    _btnMinimize->text = c_typeBrowserArrowUp;
    _btnMinimize->color = c_highlightColor;

    clear();

    connect(_btnMinimize, &NFButtonMinimize::onClick, this, &WTypeBrowser::onButtonMinimizeClick);
}

WTypeBrowser::~WTypeBrowser()
{ delete _painter; }

void WTypeBrowser::onButtonMinimizeClick()
{
    _bIsMinimized = !_bIsMinimized;
    _btnMinimize->text = _bIsMinimized ? c_typeBrowserArrowDown : c_typeBrowserArrowUp;
    if (_bIsMinimized)
        _layoutHolder->hide();
    else
        _layoutHolder->show();
}

QSize WTypeBrowser::getDesiredSize() const
{
    if (_bIsMinimized)
        return QSize(c_typeBrowserMinimalWidth, c_typeBrowserSpacing * 1.5f);

    QSize out;
    int maxMetric = 0;

    int maxImgWidth = c_typeBrowserMinimalWidth;
    int maxImgHeight = 0;
    int sumImgWidth = 0, sumImgHeight = 0;
    std::ranges::for_each(_nodeImages, [&](WTypedNodeImage *ptr){
        QSize imgSize = ptr->getDesiredSize();
        maxImgWidth = std::max(maxImgWidth, imgSize.width());
        maxImgHeight = std::max(maxImgHeight, imgSize.height());
        sumImgWidth += imgSize.width();
        sumImgHeight += imgSize.height();
    });

    if (_layout->direction() == QBoxLayout::LeftToRight ||
        _layout->direction() == QBoxLayout::RightToLeft)
    {
        maxMetric += c_typeBrowserSpacing;
        maxMetric += _nodeImages.size() * (maxImgWidth + _gap);
        out = QSize(maxMetric, c_typeBrowserSpacing + maxImgHeight);
    }
    else
    {
        maxMetric += _nodeImages.size() * _gap + sumImgHeight + c_typeBrowserSpacing * 1.5f;
        out = QSize(maxImgWidth, maxMetric);
    }

    return out;
}

void WTypeBrowser::clear()
{
    QLayoutItem* child;
    while(_layout->count() != 0)
    {
        child = _layout->takeAt(0);
        delete child;
    }
    
    std::ranges::for_each(_nodeImages, [&](WTypedNodeImage *img){
        delete img;
    });
    _nodeImages.clear();

    _layout->addSpacing(c_typeBrowserSpacing);

    _nodeImages.append(createDefaultImage());
    _layout->addWidget(_nodeImages.last());
}

bool WTypeBrowser::initTypes()
{
    if (_nodeTypeManager->Types().isEmpty())
        return false;

    clear();

    try {
        std::ranges::for_each(_nodeTypeManager->TypeNames().keys(), [&](const QString &typeName){
            _nodeImages.append(new WTypedNodeImage(typeName));
            WTypedNodeImage *image = _nodeImages.last();
            image->TypeManager = _nodeTypeManager;
            image->initType();
            _layout->addWidget(image);
        });
        _layout->addSpacing(c_typeBrowserSpacing / 2);
    } catch (...) { return false; }

    return true;
}

void WTypeBrowser::onNodeFactoryCleared()
{
    clear();
}


// ------------------ EVENTS --------------------


void WTypeBrowser::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::MouseButton::LeftButton)
    {
        _lastMouseDownPosition = mapToParent(event->position());
        _mousePressPosition = _lastMouseDownPosition - _position;
    }
}

void WTypeBrowser::mouseReleaseEvent(QMouseEvent *)
{
    this->setCursor(QCursor(Qt::CursorShape::ArrowCursor));
}

void WTypeBrowser::mouseMoveEvent(QMouseEvent *event)
{
    if (!(event->buttons() & Qt::MouseButton::LeftButton))
        return;

    if ((event->position() - _mousePressPosition).manhattanLength()
        > QApplication::startDragDistance())
        this->setCursor(QCursor(Qt::CursorShape::OpenHandCursor));

    QVector2D offset = QVector2D(mapToParent(event->position()) - _lastMouseDownPosition);
    _position = mapToParent(event->position()) - _mousePressPosition;

    _lastMouseDownPosition = mapToParent(event->position());

    onMove(offset);
}



// ------------------- PAINT ----------------------


void WTypeBrowser::paintEvent(QPaintEvent *event)
{
    _painter->begin(this);
    _painter->setRenderHint(QPainter::Antialiasing, true);
    paint(_painter, event);
    _painter->end();
}

void WTypeBrowser::paint(QPainter *painter, QPaintEvent *)
{
    // this line avoids flickering between this widget and nodes
    setUpdatesEnabled(false);

    QPen pen(Qt::SolidLine);
    pen.setColor(c_typeBrowserBackgroundColor);
    painter->setBrush(c_typeBrowserBackgroundColor);
    painter->setPen(pen);

    QRect rect = QRect(mapFromParent(_position).toPoint(), getDesiredSize());

    painter->drawRoundedRect(rect, c_nodeRoundingRadius, c_nodeRoundingRadius);

    pen.setColor(c_highlightColor);
    painter->setPen(pen);
    painter->setFont(standardFont(13));
    painter->drawText(QRect(rect.x(), rect.y() + c_typeBrowserSpacing / 3, rect.width(), rect.height()),
                      (Qt::AlignTop | Qt::AlignHCenter), "drag & place");


    _layoutHolder->setFixedSize(getDesiredSize());

    // button minimize
    {
        _btnMinimize->move((this->width() - _btnMinimize->width()) / 2, this->height() - _btnMinimize->height());
    }

    // this line avoids flickering between this widget and nodes
    setUpdatesEnabled(true);
}

}

