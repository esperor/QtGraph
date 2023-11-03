#include "helpers/keywatcher.h"

#include <QEvent>
#include <QKeyEvent>

#include "helpers/moc_keywatcher.cpp"

namespace qtgraph {

KeyWatcher::KeyWatcher(QObject* parent) : QObject(parent)
{
    if (parent)
        parent->installEventFilter(this);
}

bool KeyWatcher::eventFilter(QObject *obj, QEvent *event)
{
    switch(event->type())
    {
    case QEvent::KeyPress:
        emit keyPressEvent((QKeyEvent*)event);
        break;
    case QEvent::KeyRelease:
        emit keyReleaseEvent((QKeyEvent*)event);
        break;
    default: ;
    }
    return QObject::eventFilter(obj, event);
}

}