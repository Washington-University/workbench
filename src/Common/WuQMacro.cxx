
/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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

#define __WU_Q_MACRO_DECLARE__
#include "WuQMacro.h"
#undef __WU_Q_MACRO_DECLARE__

//#include <cmath>
//
//#include <QApplication>
//#include <QWidget>

#include "CaretAssert.h"
#include "WuQMacroCommand.h"
//#include "WuQMacroSignalEmitter.h"

using namespace caret;


    
/**
 * \class caret::WuQMacro 
 * \brief Contains a sequence of WuQMacroCommand's
 * \ingroup WuQMacro
 *
 */

/**
 * Constructor.
 */
WuQMacro::WuQMacro()
: CaretObjectTracksModification()
{
    
}

/**
 * Destructor.
 */
WuQMacro::~WuQMacro()
{
    for (auto mc : m_macroCommands) {
        delete mc;
    }
    m_macroCommands.clear();
}

void
WuQMacro::addMacroCommand(WuQMacroCommand* macroCommand)
{
    m_macroCommands.push_back(macroCommand);
    setModified();
}

/**
 * @return Name of macro
 */
QString
WuQMacro::getName() const
{
    return m_name;
}

/**
 * Set name of macro
 *
 * @param name
 *    New name
 */
void
WuQMacro::setName(const QString& name)
{
    if (m_name != name) {
        m_name = name;
        setModified();
    }
}

/**
 * @return Description of macro
 */
QString
WuQMacro::getDescription() const
{
    return m_description;
}

/**
 * Set description of macro
 *
 * @param description
 *    New description
 */
void
WuQMacro::setDescription(const QString& description)
{
    if (m_description != description) {
        m_description = description;
        setModified();
    }
}

/**
 * @return The number of macro commands in this macro
 */
int32_t
WuQMacro::getNumberOfMacroCommands() const
{
    return m_macroCommands.size();
}

/**
 * @return
 *     The macro command at the given index
 * @param index
 *     Index of the macro command
 */
const WuQMacroCommand*
WuQMacro::getMacroCommandAtIndex(const int32_t index) const
{
    CaretAssertVectorIndex(m_macroCommands, index);
    return m_macroCommands[index];
}

/**
 * @return
 *     The macro command at the given index
 * @param index
 *     Index of the macro command
 */
WuQMacroCommand*
WuQMacro::getMacroCommandAtIndex(const int32_t index)
{
    CaretAssertVectorIndex(m_macroCommands, index);
    return m_macroCommands[index];
}


//void
//WuQMacro::moveMouse(QWidget* widget,
//                        const bool highlightFlag)
//{
//    CaretAssert(widget);
//    
//    const QPoint widgetCenter = widget->rect().center();
//    const QPoint windowPoint = widget->mapToGlobal(widgetCenter);
//    
//    if (highlightFlag) {
//        const float radius = 15.0;
//        for (float angle = 0.0; angle < 6.28; angle += 0.314) {
//            const float x = windowPoint.x() + (std::cos(angle) * radius);
//            const float y = windowPoint.y() + (std::sin(angle) * radius);
//            QCursor::setPos(x, y);
//            //SystemUtilities::sleepSeconds(0.025);
//        }
//    }
//    
//    QCursor::setPos(windowPoint);
//}
//
///**
// * Run the commands in this macro.
// *
// * @param window
// *    The window from which macro was launched
// * @param stopOnErrorFlag
// *    If true, stop running the commands if there is an error
// * @param errorMessageOut
// *    Output containing any error messages
// * @return
// *    True if the macro completed without errors, else false.
// */
//bool
//WuQMacro::runMacro(QObject* window,
//                   const bool stopOnErrorFlag,
//                   QString& errorMessageOut)
//{
//    errorMessageOut.clear();
//    
//    for (auto mc : m_macroCommands) {
//        CaretAssert(mc);
//        
//        const QString objectName(mc->getObjectName());
//        QObject* object = window->findChild<QObject*>(objectName);
//        if (object == NULL) {
//            errorMessageOut.append("Unable to find object named "
//                                   + objectName
//                                   + "\n");
//            continue;
//        }
//        
//        if (object->signalsBlocked()) {
//            errorMessageOut.append("Object named "
//                                   + objectName
//                                   + " has signals blocked");
//            continue;
//        }
//        
//        QWidget* widgetToMoveMouse = qobject_cast<QWidget*>(object);
//        
//        if (widgetToMoveMouse == NULL) {
//            QObject* object = window->findChild<QObject*>(objectName);
//            if (object != NULL) {
//                QObject* parent = object->parent();
//                if (parent != NULL) {
//                    widgetToMoveMouse = qobject_cast<QWidget*>(parent);
//                }
//            }
//        }
//        
//        const WuQMacroObjectTypeEnum::Enum objectType = mc->getObjectType();
//        
//        const bool mouseEventFlag = (objectType == WuQMacroObjectTypeEnum::MOUSE_USER_EVENT);
//        if (widgetToMoveMouse != NULL) {
//            if ( ! mouseEventFlag) {
//                const bool highlightFlag = ( ! mouseEventFlag);
//                moveMouse(widgetToMoveMouse,
//                          highlightFlag);
//            }
//        }
//        
//        QString commandErrorMessage;
//        if ( ! mc->runMacro(object,
//                            commandErrorMessage)) {
//            errorMessageOut.append(commandErrorMessage + "\n");
//            if (stopOnErrorFlag) {
//                return false;
//            }
//        }
//        
//        QGuiApplication::processEvents();
//        if ( ! mouseEventFlag) {
//            SystemUtilities::sleepSeconds(1);
//        }
//        QGuiApplication::processEvents();
//    }
//    
//    return (errorMessageOut.isEmpty());
//    
//    
//}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
WuQMacro::toString() const
{
    QString s("WuQMacro\n");
    
    s.append("Name=" + m_name + "\n");
    s.append("Description=" + m_description + "\n");
    
    for (auto mc : m_macroCommands) {
        s.append(mc->toString() + "\n");
    }
    
    return s;
}

