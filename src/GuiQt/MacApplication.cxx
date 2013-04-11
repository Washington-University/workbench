
/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
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
            QFileOpenEvent* openFileEvent = dynamic_cast<QFileOpenEvent*>(event);
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

