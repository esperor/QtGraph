#include "widgets/customnode.h"

#include <QEvent>
#include <QSize>
#include "helpers/focuswatcher.h"
#include "helpers/keywatcher.h"

#include "widgets/moc_customnode.cpp"
#include "utilities/utility.h"
#include "utilities/constants.h"
#include "models/action.h"
#include "data/graph.h"

namespace qtgraph {
    
WCustomNode::WCustomNode(const DNode *logical, WCanvas *canvas) 
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

            setName(_renameEdit->text());
            deleteRenameEdit();
        });

        _renameEdit->show();
    }
}

void WCustomNode::setName(QString name)
{
    QVector<const void*> objects = 
    { (void*)new uint32_t(_lnode->ID())
    , (void*)new QString(name)
    , (void*)new QString(_lnode->getName())
    };

    IAction *_action = new IAction(
        EAction::Renaming,
        "Node renaming",
        [](DGraph *g, QVector<const void*> *o)
        {
            uint32_t id = *(uint32_t*)o->at(0);
            auto newName = (const QString*)o->at(1);

            g->nodes()[id]->setName(*newName);
        },
        [](DGraph *g, QVector<const void*> *o)
        {
            uint32_t id = *(uint32_t*)o->at(0);
            auto oldName = (const QString*)o->at(2);

            g->nodes()[id]->setName(*oldName);
        },
        [](QVector<const void*> *o)
        {
            delete (uint32_t*)o->at(0);
            delete (const QString*)o->at(1);
            delete (const QString*)o->at(2);
        },
        objects
    );
    emit action(_action);
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