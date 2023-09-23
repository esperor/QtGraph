
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

    _menuFile = new QMenu("Menu", this);

    _save = new QAction("Save", this);
    _menuFile->addAction(_save);
    connect(_save, &QAction::triggered, this, &MainWindow::save);

    _saveAs = new QAction("Save as", this);
    _menuFile->addAction(_saveAs);
    connect(_saveAs, &QAction::triggered, this, &MainWindow::save_as);

    _open = new QAction("Open saved", this);
    _menuFile->addAction(_open);
    connect(_open, &QAction::triggered, this, &MainWindow::open);

    _menuFile->addSeparator();

    _openTypes = new QAction("Load types json", this);
    _menuFile->addAction(_openTypes);
    connect(_openTypes, &QAction::triggered, this, &MainWindow::openTypes);

    _menuBar = new QMenuBar(this);
    _menuBar->addMenu(_menuFile);

    _menuOptions = new QMenu("Options", this);

    _snapping = new QAction("Toggle snapping", this);
    _menuOptions->addAction(_snapping);
    connect(_snapping, &QAction::triggered, _canvas, &Canvas::toggleSnapping);

    _menuBar->addMenu(_menuOptions);

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

std::string MainWindow::getFileName(QFileDialog::FileMode mode)
{
    QFileDialog dialog(this);
    dialog.setFileMode(mode);
    dialog.setDirectory(QCoreApplication::applicationDirPath());

    std::string file;
    if (dialog.exec() && !dialog.selectedFiles().empty())
        file = dialog.selectedFiles()[0].toStdString();
    else
        return "";

    return file;
}

void MainWindow::save()
{
    std::string file = _associatedFileName.empty() ? getFileName(QFileDialog::AnyFile) : _associatedFileName;
    internal_save(file);
    _associatedFileName = file;  
}

void MainWindow::save_as()
{
    std::string file = getFileName(QFileDialog::AnyFile);
    internal_save(file);
    if (_associatedFileName.empty())
        _associatedFileName = file;
}

void MainWindow::internal_save(std::string file)
{
    if (file.empty())
        return;

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

    out.close();
}

void MainWindow::open()
{
    std::string file = getFileName(QFileDialog::ExistingFile);
    if (file.empty())
        return;

    std::fstream in(file, std::ios::in | std::ios::binary);
    if (!in.is_open())
    {
        // TODO: ui friendly error
        qDebug() << "[Example] error opening the file";
    }
    else
    {
        if (!_canvas->deserialize(&in))
            // TODO: ui friendly error
            qDebug() << "[Example] error deserializing";
    }

    _associatedFileName = file;
    in.close();
}