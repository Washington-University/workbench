#ifndef PLOT2D_H
#define PLOT2D_H

#include <QWidget>

namespace Ui {
class Plot2D;
}

class Plot2D : public QWidget
{
    Q_OBJECT
    
public:
    explicit Plot2D(QWidget *parent = 0);
    ~Plot2D();
    
private:
    Ui::Plot2D *ui;
};

#endif // PLOT2D_H
