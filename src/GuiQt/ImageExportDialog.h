#ifndef IMAGEEXPORTDIALOG_H
#define IMAGEEXPORTDIALOG_H

#include <QDialog>

namespace Ui {
class ImageExportDialog;
}

class ImageExportDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ImageExportDialog(QWidget *parent = 0);
    ~ImageExportDialog();
    
private:
    Ui::ImageExportDialog *ui;
};

#endif // IMAGEEXPORTDIALOG_H
