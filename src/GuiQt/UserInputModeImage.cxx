
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

#define __USER_INPUT_MODE_IMAGE_DECLARE__
#include "UserInputModeImage.h"
#undef __USER_INPUT_MODE_IMAGE_DECLARE__

#include <QToolTip>

#include "Brain.h"
#include "BrainOpenGLViewportContent.h"
#include "BrainOpenGLWidget.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "ControlPoint3D.h"
#include "ControlPointFile.h"
#include "DisplayPropertiesImages.h"
#include "EventBrowserWindowDrawingContent.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "ImageFile.h"
#include "SelectionItemMedia.h"
#include "SelectionItemImageControlPoint.h"
#include "SelectionItemVoxel.h"
#include "SelectionManager.h"
#include "MouseEvent.h"
#include "UserInputModeImageWidget.h"
#include "UserInputModeView.h"
#include "VolumeFile.h"
#include "WuQTimedMessageDisplay.h"

using namespace caret;


    
/**
 * \class caret::UserInputModeImage 
 * \brief Processes user input for images.
 */

/**
 * Constructor.
 */
UserInputModeImage::UserInputModeImage(const int32_t windowIndex)
: UserInputModeView(windowIndex,
                    UserInputModeEnum::Enum::IMAGE),
  m_windowIndex(windowIndex)
{
    m_inputModeImageWidget = new UserInputModeImageWidget(this,
                                                        windowIndex);
    m_editOperation = EDIT_OPERATION_ADD;
    setWidgetForToolBar(m_inputModeImageWidget);
}

/**
 * Destructor.
 */
UserInputModeImage::~UserInputModeImage()
{
    
}

/**
 * @return The edit operation.
 */
UserInputModeImage::EditOperation
UserInputModeImage::getEditOperation() const
{
    return m_editOperation;
}

/**
 * Set the edit operation.
 * @param editOperation
 *   New edit operation.
 */
void
UserInputModeImage::setEditOperation(const EditOperation editOperation)
{
    m_editOperation = editOperation;
}

/**
 * Called when 'this' user input receiver is set
 * to receive events.
 */
void
UserInputModeImage::initialize()
{
    m_inputModeImageWidget->updateWidget();
}

/**
 * Called when 'this' user input receiver is no
 * longer set to receive events.
 */
void 
UserInputModeImage::finish()
{
}

/**
 * Called to update the input receiver for various events.
 */
void
UserInputModeImage::update()
{
    
}

/**
 * @return The cursor for display in the OpenGL widget.
 */
CursorEnum::Enum
UserInputModeImage::getCursor() const
{
    
    CursorEnum::Enum cursor = CursorEnum::CURSOR_DEFAULT;
    
    cursor = CursorEnum::CURSOR_POINTING_HAND;
    switch (m_editOperation) {
        case EDIT_OPERATION_ADD:
            cursor = CursorEnum::CURSOR_DEFAULT;
            break;
        case EDIT_OPERATION_DELETE:
            cursor = CursorEnum::CURSOR_CROSS;
            break;
    }
    
    return cursor;
}

/**
 * Updates after any changes to control points
 */
void
UserInputModeImage::updateAfterControlPointsChanged()
{
    /*
     * Need to update all graphics windows and all border controllers.
     */
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
}

/**
 * Process a mouse left click event.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeImage::mouseLeftClick(const MouseEvent& mouseEvent)
{
    BrainOpenGLViewportContent* viewportContent = mouseEvent.getViewportContent();
    if (viewportContent == NULL) {
        return;
    }
    
    BrainOpenGLWidget* openGLWidget = mouseEvent.getOpenGLWidget();
    SelectionManager* idManager =
    openGLWidget->performIdentification(mouseEvent.getX(),
                                        mouseEvent.getY(),
                                        true);
    
    SelectionItemMedia* idMedia = idManager->getMediaIdentification();
    CaretAssert(idMedia);
    
    SelectionItemVoxel* idVoxel = idManager->getVoxelIdentification();
    CaretAssert(idVoxel);
    
    SelectionItemImageControlPoint* idImageControlPoint = idManager->getImageControlPointIdentification();
    CaretAssert(idImageControlPoint);
    
    AString toolTipMessage;
    
    switch (m_editOperation) {
        case EDIT_OPERATION_ADD:
            if (idMedia->isValid()
                && idVoxel->isValid()) {
                addControlPoint(idMedia,
                                idVoxel);
            }
            else if (idMedia->isValid()) {
                toolTipMessage = "Mouse click is over image but must also be over volume slice";
            }
            else if (idVoxel->isValid()) {
                toolTipMessage = "Mouse click is over volume slice but must also be over an image";
            }
            else {
                toolTipMessage = "Mouse click must be over both an image and a volume slice";
            }
            break;
        case EDIT_OPERATION_DELETE:
            if (idImageControlPoint->isValid()) {
                deleteControlPoint(idImageControlPoint);
            }
            else {
                toolTipMessage = "Mouse click must be over an image control point";
            }
            break;
    }
    
    updateAfterControlPointsChanged();
    
    if ( ! toolTipMessage.isEmpty()) {
        WuQTimedMessageDisplay::showModal(openGLWidget,
                                          mouseEvent.getX(),
                                          mouseEvent.getY(),
                                          2,
                                          toolTipMessage);
    }
}

/**
 * Create a control point for the given image and voxel coordinates.
 *
 * @param mediaSelection
 *     The image selection.
 * @param voxelSelection
 *     The voxel selection.
 */
void
UserInputModeImage::addControlPoint(SelectionItemMedia* mediaSelection,
                                    const SelectionItemVoxel* voxelSelection)
{
    MediaFile* mediaFile = mediaSelection->getMediaFile();
    CaretAssert(mediaFile);
    ImageFile* imageFile = mediaFile->castToImageFile();
    if (imageFile == NULL) {
        CaretLogWarning("File for adding control point must be an image file but is: "
                        + DataFileTypeEnum::toName(mediaFile->getDataFileType()));
        return;
    }
    ControlPointFile* controlPointFile = imageFile->getControlPointFile();
    CaretAssert(controlPointFile);
    
    const PixelIndex pixelIndex(mediaSelection->getPixelIndexOriginAtBottom());
    const float pixelX = pixelIndex.getI();
    const float pixelY = pixelIndex.getJ();
    const float pixelZ = 0.0;
    
    double voxelXYZ[3] = { 0.0, 0.0, 0.0 };
    voxelSelection->getModelXYZ(voxelXYZ);
    
    controlPointFile->addControlPoint(ControlPoint3D(pixelX, pixelY, pixelZ,
                                                     voxelXYZ[0], voxelXYZ[1], voxelXYZ[2]));
}

/**
 * Delete the selection control point.
 *
 * @param idImageControlPoint
 *      Control point identification.
 */
void
UserInputModeImage::deleteControlPoint(SelectionItemImageControlPoint* idImageControlPoint)
{
    ControlPointFile* controlPointFile = idImageControlPoint->getControlPointFile();
    CaretAssert(controlPointFile);
    const int32_t controlPointIndex = idImageControlPoint->getControlPointIndexInFile();
    CaretAssert(controlPointIndex >= 0);
    
    controlPointFile->removeControlPointAtIndex(controlPointIndex);
}

/**
 * Delete all control points
 */
void
UserInputModeImage::deleteAllControlPoints()
{
    ImageFile* imageFile = getImageFile();
    if (imageFile != NULL) {
        imageFile->getControlPointFile()->removeAllControlPoints();
    }
    updateAfterControlPointsChanged();
}

/**
 * @return The selected image file in the window (NULL if not valid)
 */
ImageFile*
UserInputModeImage::getImageFile() const
{
    EventBrowserWindowDrawingContent windowGet(m_windowIndex);
    EventManager::get()->sendEvent(windowGet.getPointer());
    
    DisplayPropertiesImages* dpi = GuiManager::get()->getBrain()->getDisplayPropertiesImages();
    BrowserTabContent* tabContent = windowGet.getSelectedBrowserTabContent();
    if (tabContent == NULL) {
        return NULL;
    }
    
    const DisplayGroupEnum::Enum displayGroup = dpi->getDisplayGroupForTab(tabContent->getTabNumber());
    
    ImageFile* imageFile = dpi->getSelectedImageFile(displayGroup,
                                                     tabContent->getTabNumber());
    
    return imageFile;
}

/**
 * @return The tab index (negative if invalid).
 */
int32_t
UserInputModeImage::getTabIndex() const
{
    int32_t tabIndex = -1;
    
    EventBrowserWindowDrawingContent windowGet(m_windowIndex);
    EventManager::get()->sendEvent(windowGet.getPointer());
    
    BrowserTabContent* tabContent = windowGet.getSelectedBrowserTabContent();
    if (tabContent != NULL) {
        tabIndex = tabContent->getTabNumber();
    }
    
    return tabIndex;
}


/**
 * Show a context menu (pop-up menu at mouse location)
 *
 * @param mouseEvent
 *     Mouse event information.
 * @param menuPosition
 *     Point at which menu is displayed (passed to QMenu::exec())
 * @param openGLWidget
 *     OpenGL widget in which context menu is requested
 */
void
UserInputModeImage::showContextMenu(const MouseEvent& /*mouseEvent*/,
                                         const QPoint& /*menuPosition*/,
                                         BrainOpenGLWidget* /*openGLWidget*/)
{
    /* no context menu */
}



