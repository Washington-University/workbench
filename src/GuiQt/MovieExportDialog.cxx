#include "MovieExportDialog.h"
#include "ui_MovieExportDialog.h"

MovieExportDialog::MovieExportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MovieExportDialog)
{
    ui->setupUi(this);
}

MovieExportDialog::~MovieExportDialog()
{
    delete ui;
}
