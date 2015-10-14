
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

#define __MAC_APPLICATION_DECLARE__
#include "MacApplication.h"
#undef __MAC_APPLICATION_DECLARE__

#include <iostream>

#include <QFileOpenEvent>
#include <QUrl>

#include "CaretAssert.h"
#include "EventManager.h"
#include "EventOperatingSystemRequestOpenDataFile.h"

using namespace caret;


    
/**
 * \class caret::MacApplication 
 * \brief Subclass of QApplication so that Macs can receive file open events
 * \ingroup GuiQt
 *
 * Allows user to open a spec file via Mac Finder.
 *
 * Based upon: http://www.qtcentre.org/wiki/index.php?title=Opening_documents_in_the_Mac_OS_X_Finder&printable=yes&useskin=vector
 */

/**
 * Constructor.
 */
MacApplication::MacApplication(int& argc, char** argv)
: QApplication(argc, argv)
{
    
}

/**
 * Destructor.
 */
MacApplication::~MacApplication()
{
    
}

/**
 * Receive events and processes them.
 *
 * @param event
 *    The event.
 * @return true if the event was processed, else false.
 */
bool
MacApplication::event(QEvent *event)
{
    bool eventWasProcessed = false;
    
    switch (event->type()) {
        case QEvent::FileOpen:
        {
            QFileOpenEvent* openFileEvent = static_cast<QFileOpenEvent*>(event);
            CaretAssert(openFileEvent);
            
            const QString filename = openFileEvent->file();
            
            EventManager::get()->sendEvent(EventOperatingSystemRequestOpenDataFile(filename).getPointer());
            
            eventWasProcessed = true;
        }
            break;
        default:
            eventWasProcessed = QApplication::event(event);
            break;
    }
    
    return eventWasProcessed;
}

