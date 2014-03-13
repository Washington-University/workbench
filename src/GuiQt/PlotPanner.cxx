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

/**
 * \class caret::PlotPanner
 * \brief Helper class for panning Qwt plots
 * \ingroup GuiQt
 */

#include "PlotPanner.h"
using namespace caret;
PlotPanner::PlotPanner(QwtPlotCanvas * canvas) : QwtPlotPanner(canvas)
{    
    this->setEnabled(true);    
    this->setMouseButton(Qt::LeftButton);
}


bool PlotPanner::eventFilter(QObject * object,QEvent *event)
{
    if ( object && (object == parent() || object == parent()->parent()) )
    {
        switch ( event->type() )
        {
            case QEvent::MouseButtonPress:
            {
                widgetMousePressEvent( ( QMouseEvent * )event );
                break;
            }
            case QEvent::MouseMove:
            {
                widgetMouseMoveEvent( ( QMouseEvent * )event );
                break;
            }
            case QEvent::MouseButtonRelease:
            {
                widgetMouseReleaseEvent( ( QMouseEvent * )event );
                break;
            }
            case QEvent::KeyPress:
            {
                widgetKeyPressEvent( ( QKeyEvent * )event );
                break;
            }
            case QEvent::KeyRelease:
            {
                widgetKeyReleaseEvent( ( QKeyEvent * )event );
                break;
            }
            default:;
        }
    }
    return QObject::eventFilter( object, event );
}

/*!
  Handle a mouse press event for the observed widget.

  \param mouseEvent Mouse event
  \sa eventFilter(), widgetMouseReleaseEvent(), widgetMouseMoveEvent()
*/
void PlotPanner::widgetMousePressEvent( QMouseEvent *mouseEvent )
{
    if(mouseEvent->modifiers() != Qt::ShiftModifier || mouseEvent->modifiers() == Qt::ControlModifier) 
    {
        return;        
    }
    mouseEvent->setModifiers(Qt::NoModifier); // remove modifier so base class accepts event, we ignore events when shift key is pressed so panning works
    QwtPlotPanner::widgetMousePressEvent( mouseEvent);    
}

/*!
  Handle a mouse release event for the observed widget.

  \param mouseEvent Mouse event

  \sa eventFilter(), widgetMousePressEvent(), widgetMouseMoveEvent(),
*/
void PlotPanner::widgetMouseReleaseEvent( QMouseEvent *mouseEvent )
{
    /*if(mouseEvent->modifiers() != Qt::ShiftModifier || mouseEvent->modifiers() == Qt::ControlModifier) 
    {
        return;        
    }*/
    mouseEvent->setModifiers(Qt::NoModifier); // remove modifier so base class accepts event, we ignore events when shift key is pressed so panning works
    QwtPlotPanner::widgetMouseReleaseEvent( mouseEvent); 
    
}

/*!
  Handle a mouse move event for the observed widget.

  \param mouseEvent Mouse event
  \sa eventFilter(), widgetMousePressEvent(), widgetMouseReleaseEvent(),
*/
void PlotPanner::widgetMouseMoveEvent( QMouseEvent *mouseEvent )
{
    if(mouseEvent->modifiers() != Qt::ShiftModifier || mouseEvent->modifiers() == Qt::ControlModifier) 
    {
        return;        
    }
    mouseEvent->setModifiers(Qt::NoModifier);// remove modifier so base class accepts event, we ignore events when shift key is pressed so panning works
    QwtPlotPanner::widgetMouseMoveEvent( mouseEvent); 
}
