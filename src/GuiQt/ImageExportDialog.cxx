#include "ImageExportDialog.h"
#include "ui_ImageExportDialog.h"

ImageExportDialog::ImageExportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImageExportDialog)
{
    ui->setupUi(this);
}

ImageExportDialog::~ImageExportDialog()
{
    delete ui;
}
