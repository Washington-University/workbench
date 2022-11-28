
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
#include "BrainOpenGLViewportContent.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EventBrowserWindowDrawingContent.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
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
: UserInputModeView(windowIndex,
                    UserInputModeEnum::Enum::VOLUME_EDIT),
m_windowIndex(windowIndex)
{
    m_inputModeVolumeEditWidget = new UserInputModeVolumeEditWidget(this,
                                                                    windowIndex);
    setWidgetForToolBar(m_inputModeVolumeEditWidget);
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
 * Process a mouse event for editing the voxels.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeVolumeEdit::processEditCommandFromMouse(const MouseEvent& mouseEvent)
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
    if ((volumeEditInfo.m_volumeFile == idEditVoxel->getVolumeFile())
        && idEditVoxel->isValid()) {
        int64_t ijk[3];
        idEditVoxel->getVoxelIJK(ijk);
        
        VolumeEditingModeEnum::Enum editMode = VolumeEditingModeEnum::VOLUME_EDITING_MODE_ON;
        int32_t brushSizes[3] = { 0, 0, 0 };
        float paletteMappedValue = 0;
        AString labelMappedName;
        m_inputModeVolumeEditWidget->getEditingParameters(editMode,
                                                          brushSizes,
                                                          paletteMappedValue,
                                                          labelMappedName);
        const int64_t brushSizesInt64[3] = {
            brushSizes[0],
            brushSizes[1],
            brushSizes[2]
        };
        
        VolumeFileEditorDelegate* editor = volumeEditInfo.m_volumeFileEditorDelegate;
        CaretAssert(editor);
        
        const VolumeSliceViewPlaneEnum::Enum slicePlane = volumeEditInfo.m_sliceViewPlane;
        
        const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType = volumeEditInfo.m_sliceProjectionType;
        const Matrix4x4 obliqueRotationMatrix = volumeEditInfo.m_obliqueRotationMatrix;
        
        bool successFlag = true;
        AString errorMessage;
        
        float voxelValueOn = paletteMappedValue;
        float voxelValueOff = 0.0;
        
        float voxelDiffXYZ[3];
        idEditVoxel->getVoxelDiffXYZ(voxelDiffXYZ);
        
        if (volumeEditInfo.m_volumeFile->isMappedWithLabelTable()) {
            const GiftiLabelTable* labelTable = volumeEditInfo.m_volumeFile->getMapLabelTable(volumeEditInfo.m_mapIndex);
            const GiftiLabel* label = labelTable->getLabel(labelMappedName);
            if (label != NULL) {
                voxelValueOn = label->getKey();
            }
            else {
                errorMessage = ("Label name "
                                + labelMappedName
                                + " is not in label table.");
                successFlag = false;
            }
            
            const GiftiLabel* unassignedLabel = labelTable->getLabel(labelTable->getUnassignedLabelKey());
            if (unassignedLabel != NULL) {
                voxelValueOff = unassignedLabel->getKey();
            }
        }
        
        if (successFlag) {
            successFlag = editor->performEditingOperation(volumeEditInfo.m_mapIndex,
                                                          editMode,
                                                          slicePlane,
                                                          sliceProjectionType,
                                                          obliqueRotationMatrix,
                                                          voxelDiffXYZ,
                                                          ijk,
                                                          brushSizesInt64,
                                                          voxelValueOn,
                                                          voxelValueOff,
                                                          errorMessage);
        }
        
        if ( ! successFlag) {
            WuQMessageBox::errorOk(m_inputModeVolumeEditWidget,
                                   errorMessage);
        }
        
        updateGraphicsAfterEditing(volumeEditInfo.m_volumeFile,
                                   volumeEditInfo.m_mapIndex);
    }
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
    processEditCommandFromMouse(mouseEvent);
}

/**
 * Process a mouse left drag with ctrl and shift keys down event.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeVolumeEdit::mouseLeftDragWithCtrlShift(const MouseEvent& mouseEvent)
{
    processEditCommandFromMouse(mouseEvent);
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
UserInputModeVolumeEdit::showContextMenu(const MouseEvent& /*mouseEvent*/,
                                   const QPoint& /*menuPosition*/,
                                   BrainOpenGLWidget* /*openGLWidget*/)
{
    /* no context menu */
}
    
/**
 * Update the graphics after editing.
 *
 * @param volumeFile
 *    Volume file that needs coloring update.
 * @param mapIndex
 *    Index of the map.
 */
void
UserInputModeVolumeEdit::updateGraphicsAfterEditing(VolumeFile* volumeFile,
                                                    const int32_t mapIndex)
{
    CaretAssert(volumeFile);
    CaretAssert((mapIndex >= 0) && (mapIndex < volumeFile->getNumberOfMaps()));

    volumeFile->clearVoxelColoringForMap(mapIndex);
    volumeFile->updateScalarColoringForMap(mapIndex);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
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
 *    True if all ofvolumeEditInfo is valid, else false.  Even
 *    if false the overlay and model volume MAY be valid (non-NULL).
 */
bool
UserInputModeVolumeEdit::getVolumeEditInfo(VolumeEditInfo& volumeEditInfo)
{
    volumeEditInfo.m_overlaySet     = NULL;
    volumeEditInfo.m_underlayVolume = NULL;
    volumeEditInfo.m_topOverlay     = NULL;
    volumeEditInfo.m_volumeOverlay  = NULL;
    volumeEditInfo.m_volumeFile     = NULL;
    volumeEditInfo.m_mapIndex       = -1;
    volumeEditInfo.m_sliceViewPlane = VolumeSliceViewPlaneEnum::ALL;
    volumeEditInfo.m_sliceProjectionType = VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL;
    volumeEditInfo.m_obliqueRotationMatrix.identity();
    
    EventBrowserWindowDrawingContent windowEvent(m_windowIndex);
    EventManager::get()->sendEvent(windowEvent.getPointer());
    
    BrowserTabContent* tabContent = windowEvent.getSelectedBrowserTabContent();
    if (tabContent != NULL) {
        ModelVolume* modelVolume = tabContent->getDisplayedVolumeModel();
        ModelWholeBrain* modelWholeBrain = tabContent->getDisplayedWholeBrainModel();
        if ((modelVolume != NULL)
            || (modelWholeBrain != NULL)) {
            OverlaySet* overlaySet = tabContent->getOverlaySet();
            const int32_t numOverlays = overlaySet->getNumberOfDisplayedOverlays();
            for (int32_t i = 0; i < numOverlays; i++) {
                Overlay* overlay = overlaySet->getOverlay(i);
                if (i == 0) {
                    volumeEditInfo.m_topOverlay = overlay;
                }
                
                if (overlay->isEnabled()) {
                    CaretMappableDataFile* mapFile = NULL;
                    int32_t mapIndex;
                    overlay->getSelectionData(mapFile,
                                              mapIndex);
                    if (mapFile != NULL) {
                        VolumeFile* vf = dynamic_cast<VolumeFile*>(mapFile);
                        if (vf != NULL) {
                            volumeEditInfo.m_overlaySet     = overlaySet;
                            volumeEditInfo.m_underlayVolume = overlaySet->getUnderlayVolume();
                            volumeEditInfo.m_volumeOverlay  = overlay;
                            volumeEditInfo.m_volumeFile     = vf;
                            volumeEditInfo.m_mapIndex       = mapIndex;
                            volumeEditInfo.m_sliceViewPlane = tabContent->getVolumeSliceViewPlane();
                            volumeEditInfo.m_sliceProjectionType = tabContent->getVolumeSliceProjectionType();
                            volumeEditInfo.m_volumeFileEditorDelegate = vf->getVolumeFileEditorDelegate();
                            volumeEditInfo.m_obliqueRotationMatrix = tabContent->getObliqueVolumeRotationMatrix();
                            return true;
                        }
                    }
                }
            }
        }
    }
    
    return false;
}

