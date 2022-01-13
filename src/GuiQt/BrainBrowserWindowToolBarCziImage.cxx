
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

#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_CZI_IMAGE_DECLARE__
#include "BrainBrowserWindowToolBarCziImage.h"
#undef __BRAIN_BROWSER_WINDOW_TOOL_BAR_CZI_IMAGE_DECLARE__

#include <QAction>
#include <QLabel>
#include <QSpinBox>
#include <QToolButton>
#include <QVBoxLayout>

#include "Brain.h"
#include "BrowserTabContent.h"
#include "BrainBrowserWindow.h"
#include "BrainBrowserWindowToolBar.h"
#include "BrainOpenGLWidget.h"
#include "CaretAssert.h"
#include "CaretUndoStack.h"
#include "CziImageFile.h"
#include "DisplayPropertiesCziImages.h"
#include "EnumComboBoxTemplate.h"
#include "EventBrowserWindowGraphicsRedrawn.h"
#include "EventBrowserTabValidate.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "GraphicsObjectToWindowTransform.h"
#include "GuiManager.h"
#include "MediaOverlay.h"
#include "MediaOverlaySet.h"
#include "ModelMedia.h"
#include "WuQMacroManager.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::BrainBrowserWindowToolBarCziImage
 * \brief CZI Image Component of Brain Browser Window ToolBar
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param parentToolBar
 *    parent toolbar.
 */
BrainBrowserWindowToolBarCziImage::BrainBrowserWindowToolBarCziImage(BrainBrowserWindowToolBar* parentToolBar,
                                                                                   const QString& parentObjectName)
: BrainBrowserWindowToolBarComponent(parentToolBar),
m_parentToolBar(parentToolBar)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 4, 5);
    layout->addStretch();
    
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_BROWSER_WINDOW_GRAPHICS_HAVE_BEEN_REDRAWN);

}

/**
 * Destructor.
 */
BrainBrowserWindowToolBarCziImage::~BrainBrowserWindowToolBarCziImage()
{
}


/**
 * @return The selected CZI image file (NULL if none selected)
 */
CziImageFile*
BrainBrowserWindowToolBarCziImage::getCziImageFile(BrowserTabContent* browserTabContent)
{
    CziImageFile* cziImageFile(NULL);
    
    if (browserTabContent != NULL) {
        ModelMedia* mediaModel = browserTabContent->getDisplayedMediaModel();
        if (mediaModel != NULL) {
            MediaOverlaySet* mediaOverlaySet = browserTabContent->getMediaOverlaySet();
            MediaOverlay* underlay = mediaOverlaySet->getBottomMostEnabledOverlay();
            if (underlay != NULL) {
                const MediaOverlay::SelectionData selectionData(underlay->getSelectionData());

                if (selectionData.m_selectedMediaFile != NULL) {
                    cziImageFile = selectionData.m_selectedMediaFile->castToCziImageFile();
                }
            }
        }
    }
    
    return cziImageFile;
}


/**
 * Update content of this tool bar component.
 *
 * @param browserTabContent
 *     Content of the browser tab.
 */
void
BrainBrowserWindowToolBarCziImage::updateContent(BrowserTabContent* browserTabContent)
{
    m_browserTabContent = browserTabContent;

    CziImageFile* cziImageFile = getCziImageFile(browserTabContent);
    if (cziImageFile != NULL) {
    }
    
    setEnabled(cziImageFile != NULL);
}

/**
 * @return Viewport content for the selected tab (NULL if not available)
 */
const BrainOpenGLViewportContent*
BrainBrowserWindowToolBarCziImage::getBrainOpenGLViewportContent()
{
    std::vector<const BrainOpenGLViewportContent*> viewportContent;
    getParentToolBar()->m_parentBrainBrowserWindow->getAllBrainOpenGLViewportContent(viewportContent);
    for (auto v : viewportContent) {
        if (v->getBrowserTabContent() == m_browserTabContent) {
            return v;
        }
    }
    
    return NULL;
}

/**
 * Receive events from the event manager.
 *
 * @param event
 *   Event sent by event manager.
 */
void
BrainBrowserWindowToolBarCziImage::receiveEvent(Event* event)
{
    BrainBrowserWindowToolBarComponent::receiveEvent(event);
    
    if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_WINDOW_GRAPHICS_HAVE_BEEN_REDRAWN) {
        EventBrowserWindowGraphicsRedrawn* redrawnEvent =
        dynamic_cast<EventBrowserWindowGraphicsRedrawn*>(event);
        CaretAssert(redrawnEvent);
        redrawnEvent->setEventProcessed();
        
        EventBrowserTabValidate tabEvent(m_browserTabContent);
        EventManager::get()->sendEvent(tabEvent.getPointer());
        
        if ( ! tabEvent.isValid()) {
            m_browserTabContent = NULL;
        }

        updateContent(m_browserTabContent);
    }
}
