#include "helpers/focuswatcher.h"

#include <QEvent>

#include "helpers/moc_focuswatcher.cpp"

namespace qtgraph {

FocusWatcher::FocusWatcher(QObject* parent) : QObject(parent)
{
    if (parent)
        parent->installEventFilter(this);
}

bool FocusWatcher::eventFilter(QObject *obj, QEvent *event)
{
    switch(event->type())
    {
    case QEvent::FocusIn:
        emit onFocusIn();
        break;
    case QEvent::FocusOut:
        emit onFocusOut();
        break;
    default: ;
    }
    return QObject::eventFilter(obj, event);
}

}