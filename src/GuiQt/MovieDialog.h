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
    
private:
    Ui::MovieDialog *ui;
};

#endif // MOVIEDIALOG_H
