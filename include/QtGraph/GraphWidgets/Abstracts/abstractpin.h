#pragma once

#include <QObject>
#include <QWidget>
#include <QColor>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QFont>
#include <QFontMetrics>
#include <QDrag>
#include <QList>
#include <QByteArray>
#include <QDropEvent>
#include <QMenu>

#include <cstdint>

#include "QtGraph/GraphLib.h"
#include "QtGraph/DataClasses/pindata.h"
#include "QtGraph/DataClasses/pindragsignal.h"

#include "pin.pb.h"

namespace GraphLib {

class BaseNode;

enum class PinDirection
{
    In = 1,
    Out = 2,
};

class GRAPHLIB_EXPORT AbstractPin : public QWidget
{
    Q_OBJECT

public:
    AbstractPin(BaseNode *parent);
    ~AbstractPin();

    void protocolize(protocol::Pin *pPin) const;

    void setID(uint32_t newID) { _data.pinID = newID; }
    void setConnected(bool isConnected);
    void setColor(QColor color) { _color = color; }
    void setNormalD(float newD) { _normalD = newD; }
    void setText(QString text) { _text = text; }
    void setDirection(PinDirection dir) { _data.pinDirection = dir; }
    void addConnectedPin(PinData pin);
    void removeConnectedPinByID(uint32_t ID);

    uint32_t ID() const { return _data.pinID; }
    uint32_t getNodeID() const;
    bool isConnected() const { return _bIsConnected; }
    const QColor &getColor() const { return _color; }
    const float &getNormalD() const { return _normalD; }
    QString getText() const { return _text; }
    int getDesiredWidth(float zoom) const;
    PinDirection getDirection() const { return _data.pinDirection; }
    bool isInPin() const { return _data.pinDirection == PinDirection::In; }
    QPoint getCenter() const { return mapToParent(_center); }
    QPixmap getPixmap() const;
    PinData getData() const;

    // int here is pinID of connected pin
    QVector<PinData> getConnectedPins() const { return _connectedPins.values(); }

    static bool static_isInPin(const AbstractPin *pin) { return pin->getDirection() == PinDirection::In; }

signals:
    void onDrag(PinDragSignal signal);
    void onConnect(PinData outPin, PinData Pin);
    void onConnectionBreak(PinData outPin, PinData Pin);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;

private slots:
    void onConnectionBreakActionClick();

private:
    void paint(QPainter *painter, QPaintEvent *event);
    void startDrag();
    void showContextMenu(const QMouseEvent *event);

    BaseNode *_parentNode;
    PinData _data;
    QColor _color;
    float _normalD;
    bool _bIsConnected;
    QString _text;
    QPoint _center;
    // int here is pinID of connected pin
    QMap<uint32_t, PinData> _connectedPins;
    QMap<int, QAction*> _breakConnectionActions;

    QMenu _contextMenu;

    QPainter *_painter;
};

}