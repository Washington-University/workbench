#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QCloseEvent>
#include <QVBoxLayout>
#include <QMessageBox>

#include "BrainOpenGLWidget.h"
#include "GuiGlobals.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

MainWindow::MainWindow(const int openGLSizeX,
                       const int openGLSizeY)
    : QMainWindow(NULL), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);

    this->openGLWidget =
        new BrainOpenGLWidget(this,
                              CaretWindow::WINDOW_MAIN);
    this->openGLWidget->setMinimumSize(openGLSizeX,
                                       openGLSizeY);

    this->setCentralWidget(this->openGLWidget);
    this->actions = GuiGlobals::getActions();
}

void
MainWindow::closeEvent(QCloseEvent* event)
{
    this->actions->getExitProgram()->trigger();
    event->ignore();
}

void
MainWindow::slotCloseProgram()
{
    //
    // Return value of zero is YES button.
    //
    if (QMessageBox::warning(this,
                             "Caret 7",
                             "Are you sure you want to quit ?",
                             (QMessageBox::Yes | QMessageBox::No),
                             QMessageBox::Yes)
        == QMessageBox::Yes) {
        qApp->quit();
    }

}

void MainWindow::on_actionExit_triggered()
{
    this->close();
}
