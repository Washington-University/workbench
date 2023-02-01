
/*LICENSE_START*/
/*
 *  Copyright (C) 2023 Washington University School of Medicine
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

#define __WUQ_HYPERLINK_TOOL_TIP_DECLARE__
#include "WuQHyperlinkToolTip.h"
#undef __WUQ_HYPERLINK_TOOL_TIP_DECLARE__

#include <iostream>

#include <QAction>
#include <QCoreApplication>
#include <QHelpEvent>
#include <QWhatsThis>
#include <QWhatsThisClickedEvent>
#include <QWidget>

#include "CaretAssert.h"

using namespace caret;

/*
 * Follow QT coding guidelines
 * Option to turn off
 * Widgets with multiple actions?
 * May want to install in notify method in application by overwriting QCoreApplication::notify
 * Should I just call the QWhatsThis::show() function OR
 * Should I create a custom event
 * Warn if what this or tooltip text contains <html>
 * Method override for processing link or signal
 */

/**
 * \class caret::WuQHyperlinkToolTip
 * \brief Display tooltip using QWhatsThis so that hyperlinks can be used
 * \ingroup GuiQt
 *
 * Qt provides two ways to show help information for a widget, QToolTip
 * and QWhatsThis.  A Tool Tip is displayed when the mouse is over a widget
 * and a small text window pops up.  More extensive text, that also supports
 * hyperlinks, is available with QWhatsThis.  For QWhatsThis, the user must expliclty enter a
 * "What's This" mode and then click the widget of interest.  This can be a bit cumbersome
 * and there is also the possibility that no What's This information is available
 * for the widget.  In addition, if What's This is desired for mutliple items, the
 * user must explicitly enter What's This mode for each item.
 *
 * This class extends QObject and overrides the eventFilter() method.
 * In the eventFilter() method, two events are of interest,
 * QEvent::ToolTip and QEvent::WhatsThisClicked.
 *
 * When QEvent::ToolTip is received, the tooltip text for the widget (qobject)
 * is examined.  If the tool tip text contains a hyperlink (a href), the tool tip
 * is displayed using QWhatsThis:show().  Otherwise, no action is taken
 * and the tool tip is displayed using the usual Qt functionality.
 *
 * When QEvent::WhatsThisClicked is received, it is the hyperlink is
 * processed TBD.
 *
 * To use this functionality on a widget, simply call the static method
 * add().  
 */

/**
 * Constructor.
 * @parent
 *    Parent object of this instance.
 */
WuQHyperlinkToolTip::WuQHyperlinkToolTip(QObject* parent)
: QObject(parent)
{
    
}

/**
 * Destructor.
 */
WuQHyperlinkToolTip::~WuQHyperlinkToolTip()
{
}

/**
 * @return Pointer to the one instance of the hyper link tooltip
 * When an instance is created, it parent is QCoreApplication::instance().
 */
WuQHyperlinkToolTip*
WuQHyperlinkToolTip::instance()
{
    if (s_instance == NULL) {
        s_instance = new WuQHyperlinkToolTip(QCoreApplication::instance());
    }
    CaretAssert(s_instance);
    return s_instance;
}

/**
 * Add a widget for tooltip filtering.
 * @param widget
 *    Widget that will have its tool tips filtered for hyperlinks.
 */
void
WuQHyperlinkToolTip::add(QWidget* widget)
{
    CaretAssert(widget);
    
    widget->installEventFilter(instance());
}

/**
 * Filters the object for ToolTip aned WhatsThisClicked events.
 * @param object
 *    The object
 * @param event
 *    The event.
 */
bool
WuQHyperlinkToolTip::eventFilter(QObject *object, QEvent *event)
{
    CaretAssert(object);
    CaretAssert(event);
    
    if (event->type() == QEvent::WhatsThisClicked) {
        QWhatsThisClickedEvent *whatsThisEvent = dynamic_cast<QWhatsThisClickedEvent *>(event);
        CaretAssert(whatsThisEvent);
        
//        if (s_hyperlinkReceiver != NULL) {
//            s_hyperlinkReceiver->hyperlinkClicked(whatsThisEvent->href());
//        }
        

        /*
         * May want to create a subclass of QEvent() and send it
         * to the GuiManager() rather than connecting to a function
         * which could cause Qt prolbems
         *
         * static QCoreApplication::postEvent(QObject*, QEvent*, int priority);
         * Probably do not want sendEvent()
         *
         * const QEvent::Type MyEvent::MyEventType =
         * static_cast<QEvent::Type>(QEvent::registerEventType());
         * or static event
         * Create subclass of QEvent()
         * Call QEvent::registerEventType() for event type
         * new WuQHyerlinkClickedEvent() : QEvent(MyEvent::MyEventType)
         *
         * Maybe just forward this QWhatsThisClickedEvent() to an object
         * such as the GuiManager().
         *
         * May want interface for widgets to return tooltip for position
         * in the widget.
         */
        /*
         * Emit signal indicating a hyperlink has been clicked
         */
        emit hyperlinkClicked(whatsThisEvent->href());
        
        /*
         * Event has been filtered
         */
        return true;
    }
    else if (event->type() == QEvent::ToolTip) {
        if (object->isWidgetType()) {
            QWidget* widget(qobject_cast<QWidget*>(object));
            if (widget != NULL) {
                const QString text(widget->toolTip());
                if (text.toLower().contains("a href=")) {
                    std::cout << "Show text: " << text.toStdString() << std::endl;
                    QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
                    CaretAssert(helpEvent);
                    QWhatsThis::showText(helpEvent->pos(), text, widget);
                    
                    /*
                     * Event has been filtered
                     */
                    return true;
                }
            }
        }
    }
    
    /*
     * In QObject, an example shows returning QObject::eventFilter()
     * to do "standard event processing".
     * https://doc-snapshots.qt.io/qt6-dev/qobject.html#installEventFilter
     *
     * In "Events and Filters", an example shows returning false
     * so that events are sent to the target widget.
     * https://doc.qt.io/qt-6/eventsandfilters.html
     *
     * I think we want to return false so that events are handled by
     * the target widget and not this instance of QObject.
     */
    return false;
}

///**
// * Set the hyperlink receiver.  Any previous hyperlink receiver is destructed.
// * @param hyperlinkReciver
// *    New hyperlink receiver.  Ownership is taken and after this method is called, callers
// *    should never reference the instance.
// */
//void
//WuQHyperlinkToolTip::setHyperlinkReceiver(WuQHyperlinkReceiver* hyperlinkReciver)
//{
//    if (s_hyperlinkReceiver != NULL) {
//        delete s_hyperlinkReceiver;
//        s_hyperlinkReceiver = NULL;
//    }
//    s_hyperlinkReceiver = hyperlinkReciver;
//}

///**
// * \class caret::WuQHyperLinkReceiver
// * \brief Handles hyperlink request from WuQHyperlinkToolTip
// * \ingroup GuiQt
// *
// * Handles hyperlink request from WuQHyperlinkToolTip.
// * User should extend this class and override hyperlinkClicked()
// * to process the hyperlink.
// */
//
///**
// * Constructor.
// */
//WuQHyperlinkReceiver::WuQHyperlinkReceiver()
//{
//}
//
///**
// * Destructor.
// */
//WuQHyperlinkReceiver::~WuQHyperlinkReceiver()
//{
//}
//
///**
// * Called by WuQHyperlinkToolTip when a tooltip hyperlink is clicked.
// * @param hyperlink
// *    Hyperlink that was clicked by user.
// */
//void
//WuQHyperlinkReceiver::hyperlinkClicked(const QString& hyperlink)
//{
//    std::cout << "Clicked: " << hyperlink.toStdString() << std::endl;
//}
