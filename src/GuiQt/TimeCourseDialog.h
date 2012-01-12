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
//qwt includes
#include <qapplication.h>
#include "qlayout.h"
#include "qwt_plot.h"
#include "qwt_plot_marker.h"
#include "qwt_plot_curve.h"
#include "qwt_legend.h"
#include "qwt_series_data.h"
#include "qwt_plot_canvas.h"
#include "qwt_plot_panner.h"
#include "qwt_plot_magnifier.h"
#include "qwt_text.h"
#include "qwt_math.h"
#include "math.h"


namespace Ui {
    class TimeCourseDialog;
}

class TimeCourseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TimeCourseDialog(QWidget *parent = 0);
    ~TimeCourseDialog();

private slots:
    void on_TDClose_clicked();

    void on_TDClearChart_clicked();

    void on_TDMinTime_valueChanged(double arg1);

    void on_TDMaxTime_valueChanged(double arg1);

    void on_TDMinActivity_valueChanged(double arg1);

    void on_TDMaxActivity_valueChanged(double arg1);

    void on_TDShowAverage_stateChanged(int arg1);

    void on_TDShowAverage_toggled(bool checked);

    void on_TDKeepLast_valueChanged(double arg1);

private:
    Ui::TimeCourseDialog *ui;
};



//-----------------------------------------------------------------
//              simple.cpp
//
//      A simple example which shows how to use QwtPlot connected
//      to a data class without any storage, calculating each values
//      on the fly.
//-----------------------------------------------------------------

class PlotTC : public QwtPlot
{
public:
    PlotTC( QWidget *parent = NULL);
     void populate(std::vector<double> &x, std::vector<double> &y);

protected:
    virtual void resizeEvent( QResizeEvent * );

};


/*
int main(int argc, char **argv)
{
    QApplication a(argc, argv);

    Plot *plot = new Plot();

    // We put a dummy widget around to have
    // so that Qt paints a widget background
    // when resizing

    QWidget window;
    QHBoxLayout *layout = new QHBoxLayout( &window );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->addWidget( plot );

    window.resize(600,400);
    window.show();

    return a.exec();
}*/


#endif //__TIME_COURSE_DIALOG__
