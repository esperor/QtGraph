#pragma once

#include "qtgraph.h"

#include <QObject>

#include "data/graph.h"
#include "models/action.h"
#include "helpers/stack.h"
#include "widgets/canvas.h"
#include "models/selectionrectprocess.h"

namespace qtgraph {

class Controller : public QObject
{
    Q_OBJECT

    friend struct SelectionRectProcess;

public:
    Controller(QObject *parent = nullptr);
    ~Controller();

    bool serialize(std::fstream *output) const;
    bool deserialize(std::fstream *input);

    WCanvas *createCanvas(QWidget *parent = nullptr);

    /*  This disconnects type browser from canvas so that the browser 
        becomes independant widget but still receives updates from the controller. */
    WTypeBrowser *exportTypeBrowser();

    // Records and optionally executes custom action. 
    // Must NOT be used for deletion of any objects such as nodes and pins.
    void addAction(IAction *action, bool execute = true);

    // Executes action without recording
    void executeAction(IAction *action);

    // Executes and/or records action using the _bIsRecording var
    void processAction(IAction *action);

    // Reverses last recorded action.
    void undo(int num = 1);

    void setRecordingActions(bool b) { _bIsRecording = b; }
    bool getRecordingActions() const { return _bIsRecording; }

    void removeNode(uint32_t id);
    void removeNodes(QSet<uint32_t> &&ids);
    void deselectAll();
    void clear();
    void connectPins(IPinData in, IPinData out);
    void disconnectPins(IPinData in, IPinData out);
    void processNodeSelectSignal(INodeSelectSignal signal);

    void reset();

    SelectionRectProcess *startSelectionRectProcess() { return new SelectionRectProcess(this); }

    const DGraph *getGraph_const() const { return _graph; }
    const QMap<uint32_t, DNode*> &nodes() const { return _graph->nodes(); }
    const Stack<IAction*> *getStack() const { return &_stack; }

    const NodeFactory *getFactory() const { return _factory; }
    NodeTypeManager *getNodeTypeManager() { return _factory->getNodeTypeManager(); }
    PinTypeManager *getPinTypeManager() { return _factory->getPinTypeManager(); }
    WTypeBrowser *getTypeBrowser() { return _typeBrowser; }

    void setTypeManagers(PinTypeManager *pins, NodeTypeManager *nodes);

    DNode *addNode(QPoint canvasPosition, QString name);
    DNode *addNode(QPoint canvasPosition, int typeID);
    DNode *addNode(DNode *node);

signals:
    void nodeRemoved(uint32_t id);    

public slots:
    void onIsSelectedChanged(bool selected, uint32_t nodeID);
    void onNodeAdded(DNode *node);

private slots: 
    void onActionExecuted(EAction e);

private:
    IAction *createActionRemoveNodes(QSet<uint32_t> &&ids);

    DGraph *_graph;

    WCanvas *_canvas = nullptr;
    WTypeBrowser *_typeBrowser = nullptr;
    QSet<uint32_t> *_selectedNodes = nullptr;
    QMap<uint32_t, WANode*> *_wNodes = nullptr;

    // Whether graph is recording actions into stack
    bool _bIsRecording; 
    Stack<IAction*> _stack;

    NodeFactory *_factory;     
};

}