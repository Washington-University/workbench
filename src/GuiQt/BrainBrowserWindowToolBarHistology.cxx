
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

#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_HISTOLOGY_DECLARE__
#include "BrainBrowserWindowToolBarHistology.h"
#undef __BRAIN_BROWSER_WINDOW_TOOL_BAR_HISTOLOGY_DECLARE__

#include <QAction>
#include <QLabel>
#include <QToolButton>
#include <QVBoxLayout>

#include "Brain.h"
#include "BrowserTabContent.h"
#include "BrainBrowserWindow.h"
#include "BrainBrowserWindowToolBar.h"
#include "BrainOpenGLWidget.h"
#include "CaretAssert.h"
#include "CaretUndoStack.h"
#include "DisplayPropertiesCziImages.h"
#include "EnumComboBoxTemplate.h"
#include "EventBrowserWindowGraphicsRedrawn.h"
#include "EventBrowserTabValidate.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "GraphicsObjectToWindowTransform.h"
#include "GuiManager.h"
#include "HistologySlicesFile.h"
#include "HistologyOverlay.h"
#include "HistologyOverlaySet.h"
#include "ModelHistology.h"
#include "WuQMacroManager.h"
#include "WuQMessageBox.h"
#include "WuQSpinBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::BrainBrowserWindowToolBarHistology
 * \brief Histology Component of Brain Browser Window ToolBar
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param parentToolBar
 *    parent toolbar.
 */
BrainBrowserWindowToolBarHistology::BrainBrowserWindowToolBarHistology(BrainBrowserWindowToolBar* parentToolBar,
                                                                       const QString& /*parentObjectName*/)
: BrainBrowserWindowToolBarComponent(parentToolBar),
m_parentToolBar(parentToolBar)
{
    QLabel* sliceIndexLabel(new QLabel("Slice Index: "));
    m_sliceIndexSpinBox = new WuQSpinBox();
    m_sliceIndexSpinBox->setSingleStep(1);
    QObject::connect(m_sliceIndexSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                     this, &BrainBrowserWindowToolBarHistology::sliceIndexValueChanged);
    
    QLabel* sliceNumberLabel(new QLabel("Slice Number: "));
    m_sliceNumberSpinBox = new WuQSpinBox();
    m_sliceNumberSpinBox->setSingleStep(1);
    QObject::connect(m_sliceNumberSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                     this, &BrainBrowserWindowToolBarHistology::sliceNumberValueChanged);

    QGridLayout* gridLayout(new QGridLayout());
    int row(0);
    gridLayout->addWidget(sliceIndexLabel,
                          row, 0);
    gridLayout->addWidget(m_sliceIndexSpinBox,
                          row, 1);
    ++row;
    gridLayout->addWidget(sliceNumberLabel,
                          row, 0);
    gridLayout->addWidget(m_sliceNumberSpinBox,
                          row, 1);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addLayout(gridLayout);
    
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 4, 5);
    layout->addStretch();
    
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_BROWSER_WINDOW_GRAPHICS_HAVE_BEEN_REDRAWN);

}

/**
 * Destructor.
 */
BrainBrowserWindowToolBarHistology::~BrainBrowserWindowToolBarHistology()
{
}


/**
 * @return The selected histology file (NULL if none selected)
 */
HistologySlicesFile*
BrainBrowserWindowToolBarHistology::getHistologySlicesFile(BrowserTabContent* browserTabContent)
{
    HistologySlicesFile* histologySlicesFile(NULL);
    
    if (browserTabContent != NULL) {
        ModelHistology* histologyModel = browserTabContent->getDisplayedHistologyModel();
        if (histologyModel != NULL) {
            HistologyOverlaySet* histologyOverlaySet = browserTabContent->getHistologyOverlaySet();
            HistologyOverlay* underlay = histologyOverlaySet->getBottomMostEnabledOverlay();
            if (underlay != NULL) {
                const HistologyOverlay::SelectionData selectionData(underlay->getSelectionData());

                if (selectionData.m_selectedFile != NULL) {
                    histologySlicesFile = selectionData.m_selectedFile->castToHistologySlicesFile();
                }
            }
        }
    }
    
    return histologySlicesFile;
}

/**
 * Update content of this tool bar component.
 *
 * @param browserTabContent
 *     Content of the browser tab.
 */
void
BrainBrowserWindowToolBarHistology::updateContent(BrowserTabContent* browserTabContent)
{
    m_browserTabContent = browserTabContent;

    HistologySlicesFile* histologySlicesFile = getHistologySlicesFile(browserTabContent);
    if (histologySlicesFile != NULL) {
        QSignalBlocker indexBlocker(m_sliceIndexSpinBox);
        m_sliceIndexSpinBox->setRange(0, histologySlicesFile->getNumberOfHistologySlices() - 1);
        m_sliceIndexSpinBox->setValue(m_browserTabContent->getHistologySelectedSliceIndex(histologySlicesFile));

        QSignalBlocker numberBlocker(m_sliceIndexSpinBox);
        m_sliceNumberSpinBox->setRange(0, 100000);
        m_sliceNumberSpinBox->setValue(m_browserTabContent->getHistologySelectedSliceNumber(histologySlicesFile));
    }
    
    setEnabled(histologySlicesFile != NULL);
}

/**
 * @return Viewport content for the selected tab (NULL if not available)
 */
const BrainOpenGLViewportContent*
BrainBrowserWindowToolBarHistology::getBrainOpenGLViewportContent()
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
BrainBrowserWindowToolBarHistology::receiveEvent(Event* event)
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

/**
 * Called when slice index is changed
 * @param sliceIndex
 *    New slice index
 */
void
BrainBrowserWindowToolBarHistology::sliceIndexValueChanged(int sliceIndex)
{
    if (m_browserTabContent != NULL) {
        HistologySlicesFile* histologySlicesFile = getHistologySlicesFile(m_browserTabContent);
        if (histologySlicesFile != NULL) {
            m_browserTabContent->setHistologySelectedSliceIndex(histologySlicesFile,
                                                                sliceIndex);
            updateUserInterface();
            updateGraphicsWindowAndYokedWindows();
        }
    }
}

/**
 * Called when slice number is changed
 * @param sliceNumber
 *    New slice number
 */
void
BrainBrowserWindowToolBarHistology::sliceNumberValueChanged(int sliceNumber)
{
    
}
