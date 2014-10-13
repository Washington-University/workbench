
/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

#define __WU_QWT_PLOT_DECLARE__
#include "WuQwtPlot.h"
#undef __WU_QWT_PLOT_DECLARE__

#include <iostream>

#include <QContextMenuEvent>

#include "qwt_plot_canvas.h"

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::WuQwtPlot 
 * \brief Extends QwtPlot
 * \ingroup GuiQt
 *
 * Extends QwtPlot by adding a context menu with graph coordinates
 */

/**
 * Constructor.
 */
WuQwtPlot::WuQwtPlot(QWidget* w)
: QwtPlot(w)
{
    
}

/**
 * Destructor.
 */
WuQwtPlot::~WuQwtPlot()
{
}

/**
 * Receives the context menu event.
 * 
 * @param event
 *    The context menu event.
 */
void
WuQwtPlot::contextMenuEvent(QContextMenuEvent* event)
{
    QPoint canvasPos = canvas()->mapFromGlobal(event->globalPos());
    const QPointF plotPos = inverseTransform(canvasPos);
    
    emit contextMenuDisplay(event,
                            plotPos.x(),
                            plotPos.y());
}

/*!
 Translate a point from pixel into plot coordinates
 \return Point in plot coordinates
 \sa transform()
 */
QPointF
WuQwtPlot::inverseTransform( const QPoint &pos ) const
{
    QwtScaleMap xMap = canvasMap( QwtPlot::xBottom );
    QwtScaleMap yMap = canvasMap( QwtPlot::yLeft );
    
    return QPointF(
                   xMap.invTransform( pos.x() ),
                   yMap.invTransform( pos.y() )
                   );
}

