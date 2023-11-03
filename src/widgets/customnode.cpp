#include "widgets/customnode.h"

#include <QEvent>
#include <QSize>
#include "helpers/focuswatcher.h"
#include "helpers/keywatcher.h"

#include "widgets/moc_customnode.cpp"
#include "utilities/utility.h"
#include "utilities/constants.h"

namespace qtgraph {
    
WCustomNode::WCustomNode(LNode *logical, WCanvas *canvas) 
    : WANode{ logical, canvas }
    , _renameEdit{ nullptr }
{}

void WCustomNode::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() & Qt::MouseButton::LeftButton)
    {
        _renameEdit = new QLineEdit(this);
        _renameEdit->setText(_lnode->getName());
        _renameEdit->setAlignment(Qt::AlignCenter);
        _renameEdit->setFocus(Qt::FocusReason::PopupFocusReason);
        _renameEdit->setStyleSheet("* { background-color: rgba(0, 0, 0, 0); }");
        connect(new FocusWatcher(_renameEdit), &FocusWatcher::onFocusOut, this, &WCustomNode::deleteRenameEdit);
        connect(new KeyWatcher(_renameEdit), &KeyWatcher::keyPressEvent, this, [&, this](QKeyEvent *event){
            // idk why, but it works only this way
            if (event->key() + 1 != Qt::Key_Enter) return;

            _lnode->setName(_renameEdit->text());
            deleteRenameEdit();
        });

        _renameEdit->show();
    }
}

void WCustomNode::deleteRenameEdit()
{
    if (!_renameEdit) return;
    _renameEdit->deleteLater();
    _renameEdit = nullptr;
}

void WCustomNode::paintSimplifiedName(QPainter *painter, int desiredWidth, QPoint textOrigin)
{
    if (!_renameEdit)
        WANode::paintSimplifiedName(painter, desiredWidth, textOrigin);
    else 
        processRenameEdit();
}

void WCustomNode::paintName(QPainter *painter, int desiredWidth, QPoint textOrigin)
{
    if (!_renameEdit)
        WANode::paintName(painter, desiredWidth, textOrigin);
    else
        processRenameEdit();
}

void WCustomNode::processRenameEdit()
{
    if (!_renameEdit) return;
    _renameEdit->move(QPoint(getDesiredOrigin().x() + 1, getDesiredOrigin().y() - 1));

    QFont font = standardFont(c_nodeNameSize * _zoom);
    _renameEdit->setFont(font);

    _renameEdit->setFixedSize(this->size());
}

}