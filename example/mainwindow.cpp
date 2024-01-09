
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "private/utilities/constants.h"
#include <QtGraph/TypeManagers>
#include "private/utilities/utility.h"

#include <fstream>
#include <iostream>

#include <QFileDialog>
#include <QJsonDocument>
#include <QApplication>

// #include "logics/moc_typemanager.cpp"

using namespace qtgraph;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui{new Ui::MainWindow}
    , _fps{ 60 }
{
    ui->setupUi(this);

    _canvas = new WCanvas(this);
    setCentralWidget(_canvas);

    initMenuBar();

    QPalette palette(c_paletteDefaultColor);
    palette.setColor(QPalette::ColorRole::Window, c_paletteDefaultColor);
    this->setPalette(palette);

    setFocusPolicy(Qt::StrongFocus);

    _timer = new QTimer(this);
    connect(_timer, &QTimer::timeout, this, [&]()
            { _canvas->update(); });
    _timer->start(1000 / _fps);
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

    QString fileName = "";
    if (dialog.exec())
        fileName = dialog.selectedFiles()[0];
    else return;

    if (fileName.isEmpty()) return;

    NodeTypeManager *nodeManager = new NodeTypeManager();
    PinTypeManager *pinManager = new PinTypeManager();
    nodeManager->readTypes(fileName);
    pinManager->readTypes(fileName);
    _canvas->setTypeManagers(pinManager, nodeManager);
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
    std::string file = _associatedFileName ? *_associatedFileName : getFileName(QFileDialog::AnyFile);
    internal_save(file);
    _associatedFileName = file;  
}

void MainWindow::save_as()
{
    std::string file = getFileName(QFileDialog::AnyFile);
    internal_save(file);
    if (_associatedFileName)
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

void MainWindow::close()
{
    clear();
    _associatedFileName = {};
}

void MainWindow::clear()
{
    _canvas->clear();
}

void MainWindow::initMenuBar()
{
    _menuBar = new QMenuBar(this);

    _menuFile = new QMenu("Menu", this);

    _open = new QAction("Open saved", this);
    _open->setShortcut(QKeySequence::Open);
    _menuFile->addAction(_open);
    connect(_open, &QAction::triggered, this, &MainWindow::open);

    _save = new QAction("Save", this);
    _save->setShortcut(QKeySequence::Save);
    _menuFile->addAction(_save);
    connect(_save, &QAction::triggered, this, &MainWindow::save);

    _saveAs = new QAction("Save as", this);
    _saveAs->setShortcut(QKeySequence::SaveAs);
    _menuFile->addAction(_saveAs);
    connect(_saveAs, &QAction::triggered, this, &MainWindow::save_as);

    _close = new QAction("Close file", this);
    _close->setShortcut(QKeySequence::Close);
    _close->setToolTip("Clears related file and canvas. Doesn't save the file.");
    _menuFile->addAction(_close);
    connect(_close, &QAction::triggered, this, &MainWindow::close);

    _menuFile->addSeparator();

    _openTypes = new QAction("Load types json", this);
    _menuFile->addAction(_openTypes);
    connect(_openTypes, &QAction::triggered, this, &MainWindow::openTypes);

    _menuBar->addMenu(_menuFile);

    _menuEdit = new QMenu("Edit", this);

    _undo = new QAction("Undo", this);
    _menuEdit->addAction(_undo);
    connect(_snapping, &QAction::triggered, this, [this](){
        _canvas->undo(); 
    });

    _menuOptions = new QMenu("Options", this);

    _snapping = new QAction("Toggle snapping", this);
    _menuOptions->addAction(_snapping);
    _snapping->setCheckable(true);
    _snapping->setChecked(_canvas->getSnappingEnabled());
    connect(_snapping, &QAction::triggered, this, [this](bool checked){
        _canvas->setSnappingEnabled(checked); 
    });

    _clear = new QAction("Clear canvas", this);
    _menuOptions->addAction(_clear);
    connect(_clear, &QAction::triggered, this, &MainWindow::clear);

    _menuOptions->addSeparator();

    _telemetrics = new QAction("Enable telemetrics", this);
    _telemetrics->setCheckable(true);
    _telemetrics->setChecked(_canvas->getTelemetricsEnabled());
    _menuOptions->addAction(_telemetrics);
    connect(_telemetrics, &QAction::triggered, this, [this](bool checked){ 
        _canvas->setTelemetricsEnabled(checked); 
    });

    _menuBar->addMenu(_menuOptions);

    this->setMenuBar(_menuBar);
}