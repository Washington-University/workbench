#include "MovieDialog.h"
#include "ui_MovieDialog.h"

movieDialog::movieDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::movieDialog)
{
    ui->setupUi(this);
}

movieDialog::~movieDialog()
{
    delete ui;
}
