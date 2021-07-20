
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
    QLabel* pyramidLayerLabel = new QLabel("Pyramid Layer");
    
    const QString resModeTT("<html><body>"
                            "<ul>"
                            "<li>Auto - Workbench automatically selects level of image resolution"
                            "<li>Manual - User selects level of image resolution"
                            "</ul>"
                            "</body></html>");
    m_resolutionModeComboBox = new EnumComboBoxTemplate(this);
    m_resolutionModeComboBox->setup<CziImageResolutionChangeModeEnum, CziImageResolutionChangeModeEnum::Enum>();
    QObject::connect(m_resolutionModeComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &BrainBrowserWindowToolBarCziImage::resolutionModeComboBoxActivated);
    m_resolutionModeComboBox->getWidget()->setToolTip(resModeTT);
    m_resolutionModeComboBox->getWidget()->setObjectName(parentObjectName
                                                         + "BrainBrowserWindowToolBarCziImage::ResolutionModeComboBox");
    WuQMacroManager::instance()->addMacroSupportToObject(m_resolutionModeComboBox->getWidget(),
                                                         "Change CZI Resolution Mode");
    
    const QString pyrTT("Increase/decrease pyramid layer to show higher/lower resolution image.  "
                        "Higher resolution images cover a smaller spatial region.");
    m_pyramidLayerSpinBox = new QSpinBox();
    QObject::connect(m_pyramidLayerSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                     this, &BrainBrowserWindowToolBarCziImage::pyramidLayerChanged);
    m_pyramidLayerSpinBox->setObjectName(parentObjectName
                                            + ":BrainBrowserWindowToolBarCziImage:PyramidLayerSpinBox");
    WuQMacroManager::instance()->addMacroSupportToObject(m_pyramidLayerSpinBox,
                                                         "Select pyramid layer of CZI Image in Tab");
    WuQtUtilities::setWordWrappedToolTip(m_pyramidLayerSpinBox, pyrTT);

    const QString reloadTT("Reload the current pyramid layer.  This may be useful when user pans the "
                           "image so that much of the image is off the screen.");
    m_reloadAction = new QAction(this);
    m_reloadAction->setText("Reload");
    QObject::connect(m_reloadAction, &QAction::triggered,
                     this, &BrainBrowserWindowToolBarCziImage::reloadActionTriggered);
    WuQtUtilities::setWordWrappedToolTip(m_reloadAction,
                                         reloadTT);
    m_reloadAction->setObjectName(parentObjectName
                                            + ":BrainBrowserWindowToolBarCziImage:ReloadCziImageAction");
    WuQMacroManager::instance()->addMacroSupportToObject(m_reloadAction,
                                                         "Reload CZI Image");
    
    
    QToolButton* reloadToolButton = new QToolButton();
    reloadToolButton->setDefaultAction(m_reloadAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(reloadToolButton);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 4, 5);
    layout->addWidget(pyramidLayerLabel);
    layout->addWidget(m_resolutionModeComboBox->getWidget());
    layout->addWidget(m_pyramidLayerSpinBox);
    layout->addWidget(reloadToolButton);
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
                MediaFile* mediaFile(NULL);
                int32_t frameIndex(-1);
                underlay->getSelectionData(mediaFile, frameIndex);
                
                cziImageFile = mediaFile->castToCziImageFile();
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
        const int32_t tabIndex = browserTabContent->getTabNumber();
        QSignalBlocker spinBlocker(m_pyramidLayerSpinBox);
        int32_t minLayerIndex(0), maxLayerIndex(0);
        cziImageFile->getPyramidLayerRange(minLayerIndex, maxLayerIndex);
        m_pyramidLayerSpinBox->setRange(minLayerIndex, maxLayerIndex);
        m_pyramidLayerSpinBox->setValue(cziImageFile->getPyramidLayerIndexForTab(tabIndex));
        
        const DisplayPropertiesCziImages* dsc = GuiManager::get()->getBrain()->getDisplayPropertiesCziImages();
        const CziImageResolutionChangeModeEnum::Enum resolutionChangeMode(dsc->getResolutionChangeMode(tabIndex));
        m_resolutionModeComboBox->setSelectedItem<CziImageResolutionChangeModeEnum, CziImageResolutionChangeModeEnum::Enum>(resolutionChangeMode);
        const bool manualModeFlag(resolutionChangeMode == CziImageResolutionChangeModeEnum::MANUAL);
        
        m_pyramidLayerSpinBox->setEnabled(manualModeFlag);
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
 * Called when pyramid layer spin box is changed
 * @param value
 *    New value
 */
void
BrainBrowserWindowToolBarCziImage::pyramidLayerChanged(int value)
{
    CziImageFile* cziImageFile = getCziImageFile(m_browserTabContent);
    if (cziImageFile != NULL) {
        const int32_t tabIndex = m_browserTabContent->getTabNumber();
        cziImageFile->setPyramidLayerIndexForTab(tabIndex,
                                                 value);
    }

    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    updateContent(m_browserTabContent);
}

/**
 * Called when reload button is clicked
 */
void
BrainBrowserWindowToolBarCziImage::reloadActionTriggered()
{
    CziImageFile* cziImageFile = getCziImageFile(m_browserTabContent);
    if (cziImageFile != NULL) {
        const int32_t tabIndex = m_browserTabContent->getTabNumber();
        cziImageFile->reloadPyramidLayerInTab(tabIndex);
    }
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    updateContent(m_browserTabContent);
}

/**
 * Called when user changes resolution mode
 */
void
BrainBrowserWindowToolBarCziImage::resolutionModeComboBoxActivated()
{
    const int32_t tabIndex = m_browserTabContent->getTabNumber();
    DisplayPropertiesCziImages* dsc = GuiManager::get()->getBrain()->getDisplayPropertiesCziImages();
    const CziImageResolutionChangeModeEnum::Enum mode = m_resolutionModeComboBox->getSelectedItem<CziImageResolutionChangeModeEnum, CziImageResolutionChangeModeEnum::Enum>();
    dsc->setResolutionChangeMode(tabIndex, mode);
    
    switch (mode) {
        case CziImageResolutionChangeModeEnum::AUTO:
        {
            /*
             * AUTO needs repaint since it may try to load a new image
             * of a different resolution.
             */
            const bool doRepaintFlag(true);
            EventGraphicsUpdateAllWindows graphicsEvent(doRepaintFlag);
            EventManager::get()->sendEvent(graphicsEvent.getPointer());
        }
            break;
        case CziImageResolutionChangeModeEnum::MANUAL:
            EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
            break;
    }
    updateContent(m_browserTabContent);
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
        
        updateContent(m_browserTabContent);
    }
}
