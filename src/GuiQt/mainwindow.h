#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <BrainOpenGLWidget.h>
#include <Actions.h>

using namespace caret;
namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    explicit MainWindow(const int openGLSizeX,
                           const int openGLSizeY);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent* event);

public slots:
    void slotCloseProgram();

private slots:
    void on_actionExit_triggered();

private:
    Actions* actions;
    Ui::MainWindow *ui;
    BrainOpenGLWidget* openGLWidget;
};

#endif // MAINWINDOW_H
