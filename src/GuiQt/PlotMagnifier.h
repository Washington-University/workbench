#ifndef __PLOT_MAGNIFIER__
#define __PLOT_MAGNIFIER__
/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*LICENSE_END*/
#include <QWidget>
#include <QDialog>
#include "AString.h"
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
#include <QKeyEvent>

namespace caret {
class PlotMagnifier : public QwtPlotMagnifier
{
    Q_OBJECT
public:
    PlotMagnifier(QwtPlotCanvas * canvas = NULL);
protected:
    virtual bool eventFilter(QObject * object,QEvent *event);
    virtual void widgetWheelEvent( QWheelEvent *wheelEvent );
    virtual void widgetMouseMoveEvent( QMouseEvent *mouseEvent );
    virtual void widgetMousePressEvent( QMouseEvent *mouseEvent );
    virtual void widgetMouseReleaseEvent( QMouseEvent *mouseEvent );
private:   

};
}
#endif //__PLOT_MAGNIFIER__
