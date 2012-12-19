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

#include <algorithm>

#include "TimeCoursePlotter.h"

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
#include "PlotPanner.h"
using namespace caret;
TimeCoursePlotter::TimeCoursePlotter(QWidget *parent):
QwtPlot( parent )
{
    ctrlKeyDown = false;
    shiftKeyDown = false;
    max = 5;
    lineWidth = 1;
    autoScaleEnabled = true;
    // panning with the left mouse button
    (void) new PlotPanner( canvas() );

    // zoom in/out with the wheel
    magnifier = new PlotMagnifier( canvas() );    
    magnifier->setAxisEnabled(QwtPlot::yLeft,false);
    magnifier->setAxisEnabled(QwtPlot::yRight,false);    

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
    this->displayAverage = false;
}

void TimeCoursePlotter::clear(QList<TimeLine> &tlV)
{
    for(int i =0;i<tlV.size();)
    {
        colors.removeColor(tlV.at(0).colorID);
        tlV.pop_front();
    }
}

void TimeCoursePlotter::populate(QList<TimeLine> &tlV, const bool &forceDisableAutoScale)
{
    if(tlV.isEmpty()) return;
    //Delete oldest time lines first if we are displaying more than max number of time lines
    if(tlV.size()>max)
    {
        for(int i =0;i<(tlV.size()-max);)
        {
            colors.removeColor(tlV.at(0).colorID);
            tlV.pop_front();
        }
    }
    // Make sure all timelines have a valid color assigned to them
    for(int i=0;i<tlV.size();i++)
    {
        if(tlV.at(i).colorID == -1)
        {
            tlV[i].colorID = colors.getNewColor();
        }
    }
    // Insert new curves
    //setAxisScale(xBottom,0,x.size()-1);
    //setAxisScale(yLeft,0,y.size()-1);

    //Plot Time Lines
    if(forceDisableAutoScale)//we disable autoscale for updates like changing line width
    {
        setAxisAutoScale(QwtPlot::xBottom,false);
        setAxisAutoScale(QwtPlot::yLeft,false);
    }
    else
    {
        setAxisAutoScale(QwtPlot::xBottom,this->getAutoScale());
        setAxisAutoScale(QwtPlot::yLeft,this->getAutoScale());
    }

    double amin,amax,tmin,tmax;
    amin = tlV[0].y.first();
    amax = tlV[0].y.first();
    tmin = tlV[0].x.first();
    tmax = tlV[0].x.last();
    for(int i = 0;i<tlV.size();i++)
    {    
        amin = std::min(amin, *(std::min_element(tlV[i].y.begin(),tlV[i].y.end())));
        amax = std::max(amax, *(std::max_element(tlV[i].y.begin(),tlV[i].y.end())));
        tmin = std::min(tmin, *(std::min_element(tlV[i].x.begin(),tlV[i].x.end())));
        tmax = std::max(tmax, *(std::max_element(tlV[i].x.begin(),tlV[i].x.end())));

        drawTimeLine(tlV[i]);
    }
    if(this->displayAverage && tlV.size()) calculateAndDisplayAverage(tlV);


}

void TimeCoursePlotter::drawTimeLine(TimeLine &tl, QPen *pen)
{
    QPen *myPen;
    if(pen == NULL)

    {
        myPen = new QPen(colors.getColor(tl.colorID));
        myPen->setWidth(this->lineWidth);
    }
    else
    {
        myPen = pen;
    }
    QwtPlotCurve *tc = new QwtPlotCurve();
    tc->setRenderHint(QwtPlotItem::RenderAntialiased);
    //tc->setLegendAttribute(QwtPlotCurve::LegendShowLine, true);
    tc->setPen(*myPen);

    tc->setSamples(tl.x,tl.y);
    tc->attach(this);
    if(pen == NULL) delete myPen;
}



void TimeCoursePlotter::setTimeExtents(const double &min, const double &max)
{    
    this->minActivityExtent = std::min(min,this->minActivityExtent);
    this->maxActivityExtent = std::max(max,this->maxActivityExtent);
}
void TimeCoursePlotter::setActivityExtents(const double &min, const double &max)
{    
    this->minActivityExtent = std::min(min,this->minActivityExtent);
    this->maxActivityExtent = std::max(max,this->maxActivityExtent);
}
void TimeCoursePlotter::initExtents(double &tmin, double &tmax, double &amin, double &amax)
{
    this->minTimeExtent = tmin;
    this->maxTimeExtent = tmax;
    this->minActivityExtent = amin;
    this->maxActivityExtent = amax;
}
void TimeCoursePlotter::getTimeExtents(double &min, double &max) const
{
    min = this->minTimeExtent;
    max = this->maxTimeExtent;
}
void TimeCoursePlotter::getActivityExtents(double &min, double &max) const
{
    min = this->minActivityExtent;
    max = this->maxActivityExtent;
}

void TimeCoursePlotter::calculateAndDisplayAverage(QList<TimeLine> &tlV)
{
    int64_t xmax =0;
    for(int i =0;i<tlV.size();i++)
    {
        xmax = std::max<int64_t>(tlV[i].x.size(),xmax);
    }
    averageTimeLine.x.clear();
    averageTimeLine.y.clear();
    for(int i =0;i<xmax;i++)
    {
        averageTimeLine.x.push_back(0.0);
        averageTimeLine.y.push_back(0.0);
        uint64_t divisor = 0.0;

        for(int j = 0;j<tlV.size();j++)
        {
            if(i>=tlV[j].x.size()) continue;
            divisor++;
            averageTimeLine.x[i] += tlV[j].x[i];
            averageTimeLine.y[i] += tlV[j].y[i];

        }
        averageTimeLine.x[i] /= divisor;
        averageTimeLine.y[i] /= divisor;
    }
    QPen pen(colors.getColor(averageTimeLine.colorID));
    pen.setDashOffset(4);
    pen.setWidth(2);
    drawTimeLine(averageTimeLine,&pen);
}

void TimeCoursePlotter::setDisplayAverage(bool checked)
{
    displayAverage = checked;
    if(checked)
    {
        averageTimeLine.colorID = colors.getNewColor();
    }
    else
    {
        colors.removeColor(averageTimeLine.colorID);
    }
}

void TimeCoursePlotter::resizeEvent( QResizeEvent *event )
{
    QwtPlot::resizeEvent( event );
}

void TimeCoursePlotter::setTimeLineWidth(int width)
{
    this->lineWidth = width;    
}

void TimeCoursePlotter::setAutoScaleEnabled(bool checked)
{
    this->autoScaleEnabled = checked;
}

bool TimeCoursePlotter::getAutoScale()
{
    if(autoScaleOnce || autoScaleEnabled)
    {
        autoScaleOnce = false;
        return true;
    }
    else
        return false;
}

void TimeCoursePlotter::resetView()
{
    autoScaleOnce = true;
}

void TimeCoursePlotter::replot()
{
    QwtPlot::replot();
    const QwtScaleDiv *sd = this->axisScaleDiv(QwtPlot::xBottom);
    //double bound = sd->lowerBound();
    //double ubound = sd->upperBound();
    emit this->timeStartValueChanged(sd->lowerBound());
    emit this->timeEndValueChanged(sd->upperBound()); 
    sd = this->axisScaleDiv(QwtPlot::yLeft);
    //bound = sd->lowerBound();
    //ubound = sd->upperBound();
    emit this->activityMinValueChanged(sd->lowerBound());
    emit this->activityMaxValueChanged(sd->upperBound());
    //this->axisMaxMajor
}

