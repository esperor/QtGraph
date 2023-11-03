#pragma once

#include "qtgraph.h"

#include <QObject>

namespace qtgraph {

class FocusWatcher : public QObject
{
   Q_OBJECT
public:
   explicit FocusWatcher(QObject* parent = nullptr);

   virtual bool eventFilter(QObject *, QEvent *event) override;

signals:
   void onFocusIn();
   void onFocusOut();
};

}