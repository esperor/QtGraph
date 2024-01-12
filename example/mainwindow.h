#pragma once

#include <QMainWindow>
#include <QObject>
#include <QTimer>
#include <QFileDialog>

#include <QtGraph/WCanvas>


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
    void close();
    void clear();
    std::string getFileName(QFileDialog::FileMode mode);

    void initMenuBar();

    std::optional<std::string> _associatedFileName = {};

    int _fps;

    Ui::MainWindow *ui;
    qtgraph::Controller *_controller;
    qtgraph::WCanvas *_canvas;
    QTimer *_timer;

    QAction *_openTypes, *_save, *_saveAs, *_open, *_close, *_snapping, *_clear, *_telemetrics, *_undo;
    QMenu *_menuFile, *_menuOptions, *_menuEdit;
    QMenuBar *_menuBar;
};

