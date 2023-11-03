#pragma once

#include "qtgraph.h"

#include <QObject>
#include <QKeyEvent>

namespace qtgraph {

class KeyWatcher : public QObject
{
   Q_OBJECT
public:
   explicit KeyWatcher(QObject* parent = nullptr);

   virtual bool eventFilter(QObject *, QEvent *event) override;

signals:
   void keyPressEvent(QKeyEvent *event);
   void keyReleaseEvent(QKeyEvent *event);
};

}