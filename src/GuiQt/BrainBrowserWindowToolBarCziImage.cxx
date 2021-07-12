
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

#include "BrowserTabContent.h"
#include "BrainBrowserWindow.h"
#include "BrainBrowserWindowToolBar.h"
#include "BrainOpenGLWidget.h"
#include "CaretAssert.h"
#include "CaretUndoStack.h"
#include "CziImageFile.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "GraphicsObjectToWindowTransform.h"
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
    m_pyramidLayerSpinBox = new QSpinBox();
    QObject::connect(m_pyramidLayerSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                     this, &BrainBrowserWindowToolBarCziImage::pyramidLayerChanged);
    m_pyramidLayerSpinBox->setObjectName(parentObjectName
                                            + ":BrainBrowserWindowToolBarCziImage:PyramidLayerSpinBox");
    WuQMacroManager::instance()->addMacroSupportToObject(m_pyramidLayerSpinBox,
                                                         "Select pyramid layer of CZI Image in Tab");

    m_reloadAction = new QAction(this);
    m_reloadAction->setText("Reload");
    QObject::connect(m_reloadAction, &QAction::triggered,
                     this, &BrainBrowserWindowToolBarCziImage::reloadActionTriggered);
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
    layout->addWidget(m_pyramidLayerSpinBox);
    layout->addSpacing(10);
    layout->addWidget(reloadToolButton);
    layout->addStretch();
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
    
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    updateContent(m_browserTabContent);
}

