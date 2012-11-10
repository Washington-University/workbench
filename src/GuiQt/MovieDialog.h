#ifndef MOVIEDIALOG_H
#define MOVIEDIALOG_H

#include <QDialog>

namespace Ui {
class movieDialog;
}

class movieDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit movieDialog(QWidget *parent = 0);
    ~movieDialog();
    
private:
    Ui::movieDialog *ui;
};

#endif // MOVIEDIALOG_H
