#pragma once

#include <QMainWindow>
#include <QObject>
#include <QTimer>
#include <QFileDialog>

#include "QtGraph/GraphLib.h"
#include "QtGraph/GraphWidgets/canvas.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow

{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void focusOutEvent(QFocusEvent *) override;
    void focusInEvent(QFocusEvent *) override;

private:
    void openTypes();
    void save();
    void save_as();
    void internal_save(std::string file);
    void open();
    std::string getFileName(QFileDialog::FileMode mode);

    std::string _associatedFileName = "";

    Ui::MainWindow *ui;
    GraphLib::WCanvas *_canvas;
    QTimer *_timer;

    QAction *_openTypes, *_save, *_saveAs, *_open, *_snapping;
    QMenu *_menuFile, *_menuOptions;
    QMenuBar *_menuBar;
};

