#ifndef __TIME_COURSE_DIALOG__
#define __TIME_COURSE_DIALOG__
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 
/*LICENSE_END*/

#include <QWidget>
#include "AString.h"

#include <QDialog>
#include "TimeCoursePlotter.h"
#include "QList"
#include <stdio.h>



namespace Ui {
    class TimeCourseDialog;
}
using namespace caret;

class TimeCourseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TimeCourseDialog(QWidget *parent = 0);
    ~TimeCourseDialog();
    void updateDialog(const bool &forceUpdate = false, const bool &forceDisableAutoScale = false);
    void addTimeLine(TimeLine &tl);
    void addTimeLines(QList<TimeLine> &tlV);
    void setTimeSeriesGraphEnabled(bool enabled);
    void setAnimationStartTime(const double &time);
    void updateExtents();
protected:
    //virtual bool eventFilter(QObject * object,QEvent *event);
private slots:
    void on_TDClose_clicked();

    void on_TDClearChart_clicked();

    void on_TDMinTime_valueChanged(double arg1);

    void on_TDMaxTime_valueChanged(double arg1);

    void on_TDMinActivity_valueChanged(double arg1);

    void on_TDMaxActivity_valueChanged(double arg1);

    void on_TDShowAverage_toggled(bool checked);

    void on_TDKeepLast_valueChanged(int arg1);

    void on_zoomXCheckBox_toggled(bool checked);

    void on_zoomYCheckBox_toggled(bool checked);    

    void on_lineWidthSpinBox_valueChanged(int arg1);

    void on_autoFitTimeLinesCheckBox_toggled(bool checked);

    void on_resetViewButton_clicked();

public slots:
    virtual void plotTimeEndValueChanged(double time);
    virtual void plotTimeStartValueChanged(double time);
    virtual void plotActivityMaxValueChanged(double time);
    virtual void plotActivityMinValueChanged(double time);

private:
    void populateHistory();
    TimeCoursePlotter *plot;
    Ui::TimeCourseDialog *ui;
    QList<TimeLine> tlV;
    AString filename;
    bool isEnabled;
    double startOffset; 
    bool ctrlKeyDown;
    bool shiftKeyDown;

};







#endif //__TIME_COURSE_DIALOG__
