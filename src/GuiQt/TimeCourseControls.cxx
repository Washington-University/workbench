#include "TimeCourseControls.h"
#include "ui_timecoursecontrols.h"

TimeCourseControls::TimeCourseControls(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);
}

TimeCourseControls::~TimeCourseControls()
{
    delete ui;
}

void TimeCourseControls::on_TDClose_clicked()
{
    this->parent()->parent()->deleteLater();
}
