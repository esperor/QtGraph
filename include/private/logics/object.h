#pragma once

#include <QObject>

#include "qtgraph.h"

namespace qtgraph {

class Controller;
class IDGenerator;

class Object : public QObject
{
    Q_OBJECT

public: 
    Object(QObject *obj) : QObject(obj) {}

    virtual uint32_t ID() const = 0;

    virtual Controller *controller() = 0;

protected:
    IDGenerator *_idGen = nullptr;
};

}