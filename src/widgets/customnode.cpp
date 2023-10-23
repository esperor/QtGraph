#include "widgets/customnode.h"

#include <QEvent>
#include "helpers/focuswatcher.h"

namespace qtgraph {
    
void WCustomNode::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() & Qt::MouseButton::RightButton)
    {
        _renameEdit = new QLineEdit(this);
        _renameEdit->show();
        connect(new FocusWatcher(_renameEdit), &FocusWatcher::onFocusOut, this, [&, this](){
            delete _renameEdit;
            _renameEdit = nullptr;
        });
    }
}

}