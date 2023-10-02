
#pragma once


#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QString>

namespace qtgraph {

class NFButtonMinimize : public QWidget
{
    Q_OBJECT
    
public:
    explicit NFButtonMinimize(QWidget *parent = nullptr);

    QString text = "";
    QColor color = QColor(0,0,0);
    QColor backgroundColor = QColor(0,0,0,0);

signals:
    void onClick();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *) override { onClick(); }
    void mouseMoveEvent(QMouseEvent *) override {}
    void mouseReleaseEvent(QMouseEvent *) override {}

private:
    QPainter *_painter;
};

}