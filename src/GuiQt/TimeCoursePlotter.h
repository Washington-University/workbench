#ifndef __TIME_COURSE_PLOTTER__
#define __TIME_COURSE_PLOTTER__
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
#include "TimeLine.h"
#include "ColorManager.h"
#include "QList"
#include <stdio.h>
#include "PlotMagnifier.h"

namespace caret {

    class TimeCoursePlotter : public QwtPlot
    {
        Q_OBJECT
    public:
        TimeCoursePlotter( QWidget *parent = NULL);
        void populate(QList<TimeLine> &tlVIn, const bool &forceDisableAutoScale = false);
        void sortByColorId(QList<TimeLine> &tlV);
        void setDisplayAverage(bool checked);
        void calculateAndDisplayAverage(QList<TimeLine> &tlV);
        void clear(QList<TimeLine> &tlV);
        void setMaxTimeLines(int maxIn) { max = maxIn; }
        int getMaxTimeLines() { return max; }
        void setAutoScaleEnabled(bool checked);
        void setTimeLineWidth(int width);
        PlotMagnifier * magnifier;
        void resetView();
        bool getAutoScale();
        ColorManager colors;     
        void initExtents(double &tmin, double &tmax, double &amin, double &amax);
        void setTimeExtents(const double &min, const double &max);
        void setActivityExtents(const double &min, const double &max);
        void getTimeExtents(double &min, double &max) const;
        void getActivityExtents(double &min, double &max) const;
        public slots:
            virtual void replot();
signals:
            void timeStartValueChanged(double value);
            void timeEndValueChanged(double value);
            void activityMinValueChanged(double value);
            void activityMaxValueChanged(double value);

    protected:
        bool ctrlKeyDown;
        bool shiftKeyDown;
        //virtual bool eventFilter(QObject * object,QEvent *event);
        void drawTimeLine(TimeLine &tl, QPen *pen=NULL);
        virtual void resizeEvent( QResizeEvent * );
        QList<QwtPlotCurve *> plotV;

        int max;
        bool displayAverage;
        TimeLine averageTimeLine;
        bool autoScaleEnabled;
        bool autoScaleOnce;
        int lineWidth;
        double minTimeExtent;
        double maxTimeExtent;
        double minActivityExtent;
        double maxActivityExtent;

    };

}

#endif //__TIME_COURSE_PLOTTER__