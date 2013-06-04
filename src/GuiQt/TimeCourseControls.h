#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include <QItemDelegate>
#include <QModelIndex>
#include <QObject>

namespace Ui {
class Form;
}

class TimeCourseControls : public QWidget
{
    Q_OBJECT
    
public:
    explicit TimeCourseControls(QWidget *parent = 0);
    ~TimeCourseControls();
    
private slots:
    void on_TDClose_clicked();

private:
    Ui::Form *ui;
};

#endif // FORM_H
