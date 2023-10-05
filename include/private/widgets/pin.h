#pragma once

#include <QObject>
#include <QtWidgets/QWidget>
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
    WPin(LPin *lpin, WANode *parent);
    ~WPin();

    void setFakeConnected(bool isConnected) { _fakeConnected = isConnected; }
    void setNormalD(float newD) { _normalD = newD; }
    void setLogical(QSharedPointer<LPin> logical) { _lpin.swap(logical); }

    const float &getNormalD() const { return _normalD; }
    int getDesiredWidth(float zoom) const;
    EPinDirection getDirection() const { return _lpin->getData().pinDirection; }
    bool isInPin() const { return _lpin->getData().pinDirection == EPinDirection::In; }
    QPoint getCenter() const { return mapToParent(_center); }
    QPixmap getPixmap() const;
    const QSharedPointer<LPin> &getLogical() const { return _lpin; }

signals:
    void onDrag(IPinDragSignal signal);
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
    QSharedPointer<LPin> _lpin;
    float _normalD;
    // used to show pin as connected when connection is in progress
    bool _fakeConnected;
    QPoint _center;
    QMap<int, QAction*> _breakConnectionActions;
    QMenu _contextMenu;
    QPainter *_painter;
};

}