#pragma once

#include <QSet>

#include "qtgraph.h"

namespace qtgraph {

class Controller;

struct SelectionRectProcess
{
public:
    SelectionRectProcess(Controller *c);
    ~SelectionRectProcess();

    void addSelected(uint32_t nodeID);
    void removeSelected(uint32_t nodeID);

private:
    Controller *c;
    QSet<uint32_t> *selected;
    QSet<uint32_t> initialSelected;
};

}