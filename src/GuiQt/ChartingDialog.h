#ifndef CHARTINGDIALOG_H
#define CHARTINGDIALOG_H

#include <QDialog>

namespace Ui {
class ChartingDialog;
}

#include "CaretMappableDataFile.h"
#include <QTableView>
class TimeCourseControls;
class Table;
class Plot2D;

namespace caret {
class ChartingDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ChartingDialog(QWidget *parent = 0);
    ~ChartingDialog();
    void setChartMode(QString type);
    void openPconnMatrix(CaretMappableDataFile *pconnFile);    
    QTableView * getMatrixTableView();
    void showDialog();


private slots:
    void on_closeButton_clicked();

    void on_comboBox_currentIndexChanged(const QString &arg1);  
    
    void showToolBar(bool show=true);
    void showToolBox(bool show=true);

    void customContextMenuRequestedSlot(const QPoint &pos);

private:
    Table *table;
    Ui::ChartingDialog *ui;
    TimeCourseControls *form;    
    Plot2D *plot2d;
};
}
#endif // CHARTINGDIALOG_H
