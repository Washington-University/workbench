#ifndef MOVIEDIALOG_H
#define MOVIEDIALOG_H

#include <QDialog>

namespace Ui {
class MovieDialog;
}

class MovieDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit MovieDialog(QWidget *parent = 0);
    ~MovieDialog();
    
private slots:
    void on_closeButton_clicked();

    void on_animateButton_toggled(bool checked);

    void on_recordButton_toggled(bool checked);

    void on_cropImageCheckBox_toggled(bool checked);

private:
    Ui::MovieDialog *ui;
};

#endif // MOVIEDIALOG_H
