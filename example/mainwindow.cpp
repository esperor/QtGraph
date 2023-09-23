
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QtGraph/constants.h"
#include "QtGraph/TypeManagers/nodetypemanager.h"
#include "QtGraph/TypeManagers/pintypemanager.h"
#include "QtGraph/utility.h"

#include <fstream>
#include <iostream>

#include <QFileDialog>
#include <QJsonDocument>

// #include "QtGraph/TypeManagers/moc_typemanager.cpp"

using namespace GraphLib;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui{new Ui::MainWindow}
{
    ui->setupUi(this);

    _canvas = new Canvas(this);
    setCentralWidget(_canvas);

    _menu = new QMenu("Menu", this);

    _openTypes = new QAction("Load types json", this);
    _menu->addAction(_openTypes);
    connect(_openTypes, &QAction::triggered, this, &MainWindow::openTypes);

    _save = new QAction("Save", this);
    _menu->addAction(_save);
    connect(_save, &QAction::triggered, this, &MainWindow::save);

    _open = new QAction("Open saved", this);
    _menu->addAction(_open);
    connect(_open, &QAction::triggered, this, &MainWindow::open);

    _menuBar = new QMenuBar(this);
    _menuBar->addMenu(_menu);

    this->setMenuBar(_menuBar);

    QPalette palette(c_paletteDefaultColor);
    palette.setColor(QPalette::ColorRole::Window, c_paletteDefaultColor);
    this->setPalette(palette);

    setFocusPolicy(Qt::StrongFocus);

    _timer = new QTimer(this);
    connect(_timer, &QTimer::timeout, this, [&]()
            { _canvas->update(); });
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

void MainWindow::openTypes()
{
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
}

void MainWindow::save()
{  
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setDirectory(QCoreApplication::applicationDirPath());

    std::string file;
    if (dialog.exec() && !dialog.selectedFiles().empty())
        file = dialog.selectedFiles()[0].toStdString();
    else return;


    qDebug() << file;
    std::fstream out(file, std::ios::out | std::ios::trunc | std::ios::binary);
    if (!out.is_open())
    {
        // TODO: ui friendly error
        qDebug() << "[Example] error opening the file";
    }
    else
    {
        if (!_canvas->serialize(&out))
            // TODO: ui friendly error
            qDebug() << "[Example] error serializing";
    }

    _associatedFileName = QString::fromStdString(file);
    out.close();
}

void MainWindow::open()
{
    // TODO
}