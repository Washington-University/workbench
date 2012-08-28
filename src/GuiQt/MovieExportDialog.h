#ifndef MOVIEEXPORTDIALOG_H
#define MOVIEEXPORTDIALOG_H

#include <QDialog>

namespace Ui {
class MovieExportDialog;
}

class MovieExportDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit MovieExportDialog(QWidget *parent = 0);
    ~MovieExportDialog();
    
private:
    Ui::MovieExportDialog *ui;
};

#endif // MOVIEEXPORTDIALOG_H
