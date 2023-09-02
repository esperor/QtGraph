
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QtGraph/constants.h"
#include "QtGraph/TypeManagers/nodetypemanager.h"
#include "QtGraph/TypeManagers/pintypemanager.h"
#include "QtGraph/utility.h"

#include <QFileDialog>
#include <QJsonDocument>

//#include "QtGraph/TypeManagers/moc_typemanager.cpp"

using namespace GraphLib;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui{ new Ui::MainWindow }
{
    ui->setupUi(this);

    _canvas = new Canvas(this);
    setCentralWidget(_canvas);

    menu = new QMenu("Menu", this);
    openTypes = new QAction("Load types json", this);
    menu->addAction(openTypes);
    connect(openTypes, &QAction::triggered, this, [this](){
        QFileDialog dialog(this);
        dialog.setFileMode(QFileDialog::ExistingFile);
        dialog.setNameFilter("Json file (*.json)");
        dialog.setDirectory(QCoreApplication::applicationDirPath());

        QString fileName;
        if (dialog.exec())
            fileName = dialog.selectedFiles()[0];
        
        NodeTypeManager *nodeManager = new NodeTypeManager();
        PinTypeManager *pinManager = new PinTypeManager();
        nodeManager->readTypes(fileName);
        pinManager->readTypes(fileName);
        _canvas->setNodeTypeManager(nodeManager);
        _canvas->setPinTypeManager(pinManager);
    });

    bar = new QMenuBar(this);
    bar->addMenu(menu);

    this->setMenuBar(bar);

    QPalette palette(c_paletteDefaultColor);
    palette.setColor(QPalette::ColorRole::Window, c_paletteDefaultColor);
    this->setPalette(palette);

    setFocusPolicy(Qt::StrongFocus);

    _timer = new QTimer(this);
    connect(_timer, &QTimer::timeout, this, [&](){
        _canvas->update();
    });
    _timer->start(10);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::focusOutEvent(QFocusEvent *)
{
    _timer->stop();
}

void MainWindow::focusInEvent(QFocusEvent *)
{
    _timer->start();
}