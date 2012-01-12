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


#include "TimeCourseDialog.h"
#include "ui_TimeCourseDialog.h"

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



TimeCourseDialog::TimeCourseDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TimeCourseDialog)
{
    ui->setupUi(this);

    PlotTC *plot = new PlotTC();




    // We put a dummy widget around to have
    // so that Qt paints a widget background
    // when resizing
    std::vector<double>x;
    std::vector<double>y;
    for(int64_t i=0;i<5;i++)
    {
        x.push_back((double)i);
        y.push_back((double)i);
    }


    plot->populate(x,y);
    ui->verticalLayout_4->setContentsMargins(0,0,0,0);
    ui->verticalLayout_4->insertWidget(0,plot,100);
    //plot->setGeometry(0,0,5,5);
    //plot->update();

}

TimeCourseDialog::~TimeCourseDialog()
{
    delete ui;
}

void TimeCourseDialog::addTimeLine(TimeLine &tl)
{
    if(tlV.size() > ui->TDKeepLast->value())
        tlV.erase(tlV.begin());
    tlV.push_back(tl);
}

void TimeCourseDialog::updateDialog()
{
    if(tlV.size() == 0) return;
    this->setVisible(true);
    this->show();
    this->activateWindow();
}




void TimeCourseDialog::on_TDClose_clicked()
{
    this->hide();
}

void TimeCourseDialog::on_TDClearChart_clicked()
{

}

void TimeCourseDialog::on_TDMinTime_valueChanged(double arg1)
{

}

void TimeCourseDialog::on_TDMaxTime_valueChanged(double arg1)
{

}

void TimeCourseDialog::on_TDMinActivity_valueChanged(double arg1)
{

}

void TimeCourseDialog::on_TDMaxActivity_valueChanged(double arg1)
{

}

void TimeCourseDialog::on_TDShowAverage_stateChanged(int arg1)
{

}

void TimeCourseDialog::on_TDShowAverage_toggled(bool checked)
{

}

void TimeCourseDialog::on_TDKeepLast_valueChanged(double arg1)
{

}

TimeLine::TimeLine(uint64_t nodeidIn, double *pointIn, std::vector<double> &xIn, std::vector<double> &yIn)
{
    nodeid = nodeidIn;
    for(int i=0;i<3;i++) point[i] = pointIn[i];
    x = xIn;
    y = yIn;
}

TimeLine::~TimeLine()
{

}

PlotTC::PlotTC(QWidget *parent):
    QwtPlot( parent )
{
    // panning with the left mouse button
    (void) new QwtPlotPanner( canvas() );

    // zoom in/out with the wheel
    (void) new QwtPlotMagnifier( canvas() );

    setAutoFillBackground( true );
    setPalette( QPalette( QColor( 165, 193, 228 ) ) );    

    setTitle("Time Course");
    insertLegend(new QwtLegend(), QwtPlot::RightLegend);

    // canvas
    canvas()->setLineWidth( 1 );
    canvas()->setFrameStyle( QFrame::Box | QFrame::Plain );
    canvas()->setBorderRadius( 1 );//15

    QPalette canvasPalette( Qt::white );
    canvasPalette.setColor( QPalette::Foreground, QColor( 133, 190, 232 ) );
    canvas()->setPalette( canvasPalette );


}

void PlotTC::populate(std::vector<double> &x, std::vector<double> &y)
{
    // Insert new curves
    //setAxisScale(xBottom,0,x.size()-1);
    //setAxisScale(yLeft,0,y.size()-1);

    QwtPlotCurve *tc = new QwtPlotCurve();
    tc->setRenderHint(QwtPlotItem::RenderAntialiased);
    //tc->setLegendAttribute(QwtPlotCurve::LegendShowLine, true);
    tc->setPen(QPen(Qt::red));


    QVector<double> qx;
    QVector<double> qy;
    qx = qx.fromStdVector(x);
    qy = qy.fromStdVector(y);

    tc->setSamples(qx,qy);
    tc->attach(this);


}

void PlotTC::resizeEvent( QResizeEvent *event )
{
    QwtPlot::resizeEvent( event );
}

