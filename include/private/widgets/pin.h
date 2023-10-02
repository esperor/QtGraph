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

#include "qtgraph.h"
#include "models/pindata.h"
#include "models/pindragsignal.h"

#include "pin.pb.h"

namespace qtgraph {

class WANode;

class WPin : public QWidget
{
    Q_OBJECT

public:
    WPin(WANode *parent);
    ~WPin();

    virtual void protocolize(protocol::Pin *pPin) const;
    virtual void deprotocolize(const protocol::Pin &pPin);

    void setID(uint32_t newID) { _data.pinID = newID; }
    void setConnected(bool isConnected);
    void setColor(QColor color) { _color = color; }
    void setNormalD(float newD) { _normalD = newD; }
    void setText(QString text) { _text = text; }
    void setDirection(EPinDirection dir) { _data.pinDirection = dir; }
    void addConnectedPin(IPinData pin);
    void removeConnectedPinByID(uint32_t ID);

    uint32_t ID() const { return _data.pinID; }
    uint32_t getNodeID() const;
    bool isConnected() const { return _bIsConnected; }
    const QColor &getColor() const { return _color; }
    const float &getNormalD() const { return _normalD; }
    QString getText() const { return _text; }
    int getDesiredWidth(float zoom) const;
    EPinDirection getDirection() const { return _data.pinDirection; }
    bool isInPin() const { return _data.pinDirection == EPinDirection::In; }
    QPoint getCenter() const { return mapToParent(_center); }
    QPixmap getPixmap() const;
    IPinData getData() const;

    QVector<IPinData> getConnectedPins() const { return _connectedPins.values(); }

    static bool static_isInPin(const WPin *pin) { return pin->getDirection() == EPinDirection::In; }

signals:
    void onDrag(PinDragSignal signal);
    void onConnect(IPinData outPin, IPinData inPin);
    void onConnectionBreak(IPinData outPin, IPinData inPin);

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

protected:
    WANode *_parentNode;
    IPinData _data;
    QColor _color;
    float _normalD;
    bool _bIsConnected;
    QString _text;
    QPoint _center;
    // uint32_t here is pinID of connected pin
    QMap<uint32_t, IPinData> _connectedPins;
    QMap<int, QAction*> _breakConnectionActions;

    QMenu _contextMenu;

    QPainter *_painter;
};

}