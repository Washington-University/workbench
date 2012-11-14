#include "MovieDialog.h"
#include "ui_MovieDialog.h"

MovieDialog::MovieDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MovieDialog)
{
    ui->setupUi(this);
}

MovieDialog::~MovieDialog()
{
    delete ui;
}
