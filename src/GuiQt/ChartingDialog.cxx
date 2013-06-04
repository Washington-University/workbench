#include "ChartingDialog.h"
#include "ui_chartingdialog.h"
#include "TimeCourseControls.h"
#include "Plot2d.h"
#include "Table.h"
#include "QVBoxLayout"

ChartingDialog::ChartingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChartingDialog)
{
    ui->setupUi(this);
    form = new TimeCourseControls();
    table = new Table();
    plot2d = new Plot2D();
    ui->comboBox->setCurrentIndex(1);
}

ChartingDialog::~ChartingDialog()
{
    delete ui;    
}

void ChartingDialog::on_closeButton_clicked()
{
    this->parent()->deleteLater();
}

void ChartingDialog::setChartMode(QString type)
{
    QVBoxLayout *layout = (QVBoxLayout *)this->layout();
    layout->removeWidget(this->table);
    layout->removeWidget(this->form);
    this->form->hide();
    this->table->hide();
    this->plot2d->hide();
    if(type == "2D Plot")
    {
        layout->insertWidget(1,plot2d,50);
        layout->insertWidget(2,form, 50);
        this->form->show();
        this->plot2d->show();
    }
    else if(type == "Matrix View")
    {
        layout->insertWidget(1,table,50);
        this->table->show();
    }
}

void ChartingDialog::on_comboBox_currentIndexChanged(const QString &arg1)
{
    setChartMode(arg1);
}
