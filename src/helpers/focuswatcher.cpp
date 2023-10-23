#include "focuswatcher.h"

#include <QEvent>

#include "helpers/moc_focuswatcher.cpp"

namespace qtgraph {

FocusWatcher::FocusWatcher(QObject* parent = nullptr) : QObject(parent)
{
    if (parent)
        parent->installEventFilter(this);
}

bool FocusWatcher::eventFilter(QObject *obj, QEvent *event)
{
    switch(event->type())
    {
    case QEvent::FocusIn:
        emit onFocusIn;
        return true;
    case QEvent::FocusOut:
        emit onFocusOut;
        return true;
    default: return false;
    }
    return QObject::eventFilter(obj, event);
}

}