
/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

#define __USER_INPUT_MODE_VOLUME_EDIT_DECLARE__
#include "UserInputModeVolumeEdit.h"
#undef __USER_INPUT_MODE_VOLUME_EDIT_DECLARE__

#include "Brain.h"
#include "BrainOpenGLWidget.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EventBrowserWindowContentGet.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "MouseEvent.h"
#include "Overlay.h"
#include "OverlaySet.h"
#include "SelectionItemVoxelEditing.h"
#include "SelectionManager.h"
#include "UserInputModeVolumeEditWidget.h"
#include "VolumeFile.h"
#include "VolumeFileEditorDelegate.h"
#include "VolumeSliceViewPlaneEnum.h"
#include "WuQMessageBox.h"

using namespace caret;


    
/**
 * \class caret::UserInputModeVolumeEdit 
 * \brief User input processor for editing volume voxels
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * Index of window using this volume editor input handler.
 */
UserInputModeVolumeEdit::UserInputModeVolumeEdit(const int32_t windowIndex)
: UserInputModeView(),
m_windowIndex(windowIndex)
{
    m_inputModeVolumeEditWidget = new UserInputModeVolumeEditWidget(this,
                                                                    windowIndex);
}

/**
 * Destructor.
 */
UserInputModeVolumeEdit::~UserInputModeVolumeEdit()
{
}

/**
 * Called when 'this' user input receiver is set
 * to receive events.
 */
void
UserInputModeVolumeEdit::initialize()
{
    m_inputModeVolumeEditWidget->updateWidget();
}

/**
 * Called when 'this' user input receiver is no
 * longer set to receive events.
 */
void
UserInputModeVolumeEdit::finish()
{
}

/**
 * Called to update the input receiver for various events.
 */
void
UserInputModeVolumeEdit::update()
{
    m_inputModeVolumeEditWidget->updateWidget();
}


/**
 * @return A widget for display at the bottom of the
 * Browser Window Toolbar when this mode is active.
 * If no user-interface controls are needed, return NULL.
 * The toolbar will take ownership of the widget and
 * delete it so derived class MUST NOT delete the widget.
 */
QWidget*
UserInputModeVolumeEdit::getWidgetForToolBar()
{
    return m_inputModeVolumeEditWidget;
}

/**
 * @return The input mode enumerated type.
 */
UserInputModeVolumeEdit::UserInputMode
UserInputModeVolumeEdit::getUserInputMode() const
{
    
    return UserInputReceiverInterface::VOLUME_EDIT;
}

/**
 * Process a mouse left click event.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeVolumeEdit::mouseLeftClick(const MouseEvent& mouseEvent)
{
    if (mouseEvent.getViewportContent() == NULL) {
        return;
    }
    
    VolumeEditInfo volumeEditInfo;
    if ( ! getVolumeEditInfo(volumeEditInfo)) {
        return;
    }
    
    BrainOpenGLWidget* openGLWidget = mouseEvent.getOpenGLWidget();
    const int mouseX = mouseEvent.getX();
    const int mouseY = mouseEvent.getY();
    
    SelectionManager* idManager = GuiManager::get()->getBrain()->getSelectionManager();
    SelectionItemVoxelEditing* idEditVoxel = idManager->getVoxelEditingIdentification();
    idEditVoxel->setVolumeFileForEditing(volumeEditInfo.m_volumeFile);
    idManager = openGLWidget->performIdentificationVoxelEditing(volumeEditInfo.m_volumeFile,
                                                                mouseX,
                                                                mouseY);
    if (idEditVoxel->isValid()) {
        const VolumeFile* vf = dynamic_cast<const VolumeFile*>(idEditVoxel->getVolumeFile());
        if (vf != NULL) {
            std::cout << "Selected " << qPrintable(vf->getFileNameNoPath()) << std::endl;
            
            int64_t ijk[3];
            idEditVoxel->getVoxelIJK(ijk);
            
            std::cout << "   Voxel: " << qPrintable(AString::fromNumbers(ijk, 3, ",")) << std::endl;
            VolumeEditingModeEnum::Enum editMode = VolumeEditingModeEnum::VOLUME_EDITING_MODE_ON;
            int32_t brushSizes[3] = { 0, 0, 0 };
            float voxelValue = 0;
            m_inputModeVolumeEditWidget->getEditingParameters(editMode,
                                                              brushSizes,
                                                              voxelValue);
            const int64_t brushSizesInt64[3] = {
                brushSizes[0],
                brushSizes[1],
                brushSizes[2]
            };
            
            VolumeFileEditorDelegate* editor = volumeEditInfo.m_volumeFileEditorDelegate;
            CaretAssert(editor);
            
            VolumeSliceViewPlaneEnum::Enum slicePlane = volumeEditInfo.m_sliceViewPlane;
            
            AString errorMessage;
            if ( ! editor->performEditingOperation(volumeEditInfo.m_mapIndex,
                                                   editMode,
                                                   slicePlane,
                                                   ijk,
                                                   brushSizesInt64,
                                                   voxelValue,
                                                   errorMessage)) {
                WuQMessageBox::errorOk(m_inputModeVolumeEditWidget,
                                       errorMessage);
            }
        }
    }
    
    updateGraphicsAfterEditing();
}

/**
 * @return The cursor for display in the OpenGL widget.
 */
CursorEnum::Enum
UserInputModeVolumeEdit::getCursor() const
{
    
    CursorEnum::Enum cursor = CursorEnum::CURSOR_DEFAULT;
    
//    switch (m_mode) {
//        case MODE_CREATE:
//            break;
//        case MODE_EDIT:
//            cursor = CursorEnum::CURSOR_POINTING_HAND;
//            switch (m_editOperation) {
//                case EDIT_OPERATION_DELETE:
//                    cursor = CursorEnum::CURSOR_CROSS;
//                    break;
//                case EDIT_OPERATION_PROPERTIES:
//                    cursor = CursorEnum::CURSOR_WHATS_THIS;
//                    break;
//            }
//            break;
//        case MODE_OPERATIONS:
//            cursor = CursorEnum::CURSOR_POINTING_HAND;
//            break;
//    }
    
    return cursor;
}

/**
 * Get information about volume data being edited.
 *
 * @param volumeEditInfo
 *    Loaded with editing information upon sucess.
 * @return
 *    True if volumeEditInfo is valid, else false.
 */
bool
UserInputModeVolumeEdit::getVolumeEditInfo(VolumeEditInfo& volumeEditInfo)
{
    volumeEditInfo.m_volumeFile     = NULL;
    volumeEditInfo.m_mapIndex       = -1;
    volumeEditInfo.m_sliceViewPlane = VolumeSliceViewPlaneEnum::ALL;
    volumeEditInfo.m_modelVolume    = NULL;
    
    EventBrowserWindowContentGet windowEvent(m_windowIndex);
    EventManager::get()->sendEvent(windowEvent.getPointer());
    
    BrowserTabContent* tabContent = windowEvent.getSelectedBrowserTabContent();
    if (tabContent != NULL) {
        ModelVolume* modelVolume = tabContent->getDisplayedVolumeModel();
        if (modelVolume != NULL) {
            OverlaySet* overlaySet = tabContent->getOverlaySet();
            const int32_t numOverlays = overlaySet->getNumberOfDisplayedOverlays();
            for (int32_t i = 0; i < numOverlays; i++) {
                Overlay* overlay = overlaySet->getOverlay(i);
                CaretMappableDataFile* mapFile = NULL;
                int32_t mapIndex;
                overlay->getSelectionData(mapFile,
                                          mapIndex);
                if (mapFile != NULL) {
                    VolumeFile* vf = dynamic_cast<VolumeFile*>(mapFile);
                    if (vf != NULL) {
                        volumeEditInfo.m_volumeFile     = vf;
                        volumeEditInfo.m_mapIndex       = mapIndex;
                        volumeEditInfo.m_sliceViewPlane = tabContent->getSliceViewPlane();
                        volumeEditInfo.m_modelVolume    = modelVolume;
                        volumeEditInfo.m_volumeFileEditorDelegate = NULL;
                        return true;
                    }
                }
            }
        }
    }
    
    return false;
}


/**
 * Update graphics after editing.
 */
void
UserInputModeVolumeEdit::updateGraphicsAfterEditing()
{
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}



