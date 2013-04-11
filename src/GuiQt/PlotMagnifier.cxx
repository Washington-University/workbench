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

/**
 * \class caret::PlotMagnifier 
 * \brief Helper class for magnifying Qwt plots
 * \ingroup GuiQt
 */

#include "PlotMagnifier.h"
using namespace caret;
PlotMagnifier::PlotMagnifier(QwtPlotCanvas * canvas) : QwtPlotMagnifier(canvas)
{    
    this->setEnabled(true);  
    //The three function calls below reverse the default behavior of zooming so that it is consistent with
    //brainview's zooming
    this->setMouseFactor(1.05);//inverse of default value, 0.95
    this->setWheelFactor(1.11);//inverse of default value, 0.9
    this->setMouseButton(Qt::LeftButton);
}


bool PlotMagnifier::eventFilter(QObject * object,QEvent *event)
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
            case QEvent::Wheel:
            {
                widgetWheelEvent( ( QWheelEvent * )event );
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
void PlotMagnifier::widgetMousePressEvent( QMouseEvent *mouseEvent )
{
    if(mouseEvent->modifiers() != Qt::ControlModifier || mouseEvent->modifiers() == Qt::ShiftModifier) 
    {
        return;        
    }
    mouseEvent->setModifiers(Qt::NoModifier); // remove modifier so base class accepts event, we ignore events when shift key is pressed so panning works
    QwtPlotMagnifier::widgetMousePressEvent( mouseEvent);    
}

/*!
  Handle a mouse release event for the observed widget.

  \param mouseEvent Mouse event

  \sa eventFilter(), widgetMousePressEvent(), widgetMouseMoveEvent(),
*/
void PlotMagnifier::widgetMouseReleaseEvent( QMouseEvent *mouseEvent )
{
    /*if(mouseEvent->modifiers() != Qt::ControlModifier || mouseEvent->modifiers() == Qt::ShiftModifier) 
    {
        return;        
    }*/
    mouseEvent->setModifiers(Qt::NoModifier); // remove modifier so base class accepts event, we ignore events when shift key is pressed so panning works
    QwtPlotMagnifier::widgetMouseReleaseEvent( mouseEvent); 
    
}

/*!
  Handle a mouse move event for the observed widget.

  \param mouseEvent Mouse event
  \sa eventFilter(), widgetMousePressEvent(), widgetMouseReleaseEvent(),
*/
void PlotMagnifier::widgetMouseMoveEvent( QMouseEvent *mouseEvent )
{
    if(mouseEvent->modifiers() != Qt::ControlModifier || mouseEvent->modifiers() == Qt::ShiftModifier) 
    {
        return;        
    }
    mouseEvent->setModifiers(Qt::NoModifier);// remove modifier so base class accepts event, we ignore events when shift key is pressed so panning works
    QwtPlotMagnifier::widgetMouseMoveEvent( mouseEvent); 
}

/*!
  Handle a wheel event for the observed widget.

  \param wheelEvent Wheel event
  \sa eventFilter()
*/
void PlotMagnifier::widgetWheelEvent( QWheelEvent *wheelEvent )
{
    // remove modifier so base class accepts event
    wheelEvent->setModifiers(Qt::NoModifier);    
    QwtPlotMagnifier::widgetWheelEvent( wheelEvent);
}