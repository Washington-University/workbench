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

void MovieDialog::on_closeButton_clicked()
{
    this->close();
}

void MovieDialog::on_animateButton_toggled(bool checked)
{

}

void MovieDialog::on_recordButton_toggled(bool checked)
{

}

void MovieDialog::on_cropImageCheckBox_toggled(bool checked)
{

}
