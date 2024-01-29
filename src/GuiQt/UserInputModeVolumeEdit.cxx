
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
#include "DrawingViewportContent.h"
#include "EventBrowserWindowDrawingContent.h"
#include "EventDrawingViewportContentGet.h"
#include "EventGraphicsPaintNowAllWindows.h"
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
 * @param browserIndexIndex
 * Index of window using this volume editor input handler.
 */
UserInputModeVolumeEdit::UserInputModeVolumeEdit(const int32_t browserIndexIndex)
: UserInputModeView(browserIndexIndex,
                    UserInputModeEnum::Enum::VOLUME_EDIT)
{
    m_inputModeVolumeEditWidget = new UserInputModeVolumeEditWidget(this,
                                                                    browserIndexIndex);
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
    
    std::unique_ptr<EventDrawingViewportContentGet> viewportContent(
               EventDrawingViewportContentGet::newInstanceGetTopModelViewport(getBrowserWindowIndex(),
                                                                         mouseEvent.getXY()));
    EventManager::get()->sendEvent(viewportContent.get());
    if ( ! viewportContent) {
        WuQMessageBox::errorOk(m_inputModeVolumeEditWidget,
                               "Failed to find volume slice under the mouse");
        return;

    }
    std::shared_ptr<DrawingViewportContent> drawContent(viewportContent->getDrawingViewportContent());
    const DrawingViewportContentVolumeSlice sliceVP(drawContent->getVolumeSlice());
    const VolumeSliceViewPlaneEnum::Enum sliceViewPlane(sliceVP.getVolumeSliceViewPlane());
    if (sliceViewPlane == VolumeSliceViewPlaneEnum::ALL) {
        WuQMessageBox::errorOk(m_inputModeVolumeEditWidget,
                               "Mouse was not clicked over a volume slice");
        return;
    }
    VolumeEditInfo volumeEditInfo;
    if ( ! getVolumeEditInfoForEditOperation(volumeEditInfo,
                                             sliceViewPlane)) {
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
        const VoxelIJK ijk(idEditVoxel->getVoxelIJK());
        
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
        const Matrix4x4 obliqueMprRotationMatrix = volumeEditInfo.m_obliqueMprRotationMatrix;
        
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
                                                          obliqueMprRotationMatrix,
                                                          voxelDiffXYZ,
                                                          ijk.m_ijk,
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
    /*
     * Note: Coloring is updated in VolumeFileEditorDelegate
     * We want to repaint the graphics as immediately
     */
    EventManager::get()->sendEvent(EventGraphicsPaintNowAllWindows().getPointer());
}

/**
 * @return The cursor for display in the OpenGL widget.
 */
CursorEnum::Enum
UserInputModeVolumeEdit::getCursor() const
{
    
    CursorEnum::Enum cursor = CursorEnum::CURSOR_DEFAULT;
    
    return cursor;
}

/**
 * Get information about volume data being edited.
 *
 * @param volumeEditInfo
 *    Loaded with editing information upon sucess BUT oblique / MPR matrix is NOT set
 *    There may be instance where the slice plane is "ALL" but the matrix is not avaliable
 *    for ALL, only the individual planes (Axial, Coronal, Parasagittal)
 * @return
 *    True if all ofvolumeEditInfo is valid, else false.  Even
 *    if false the overlay and model volume MAY be valid (non-NULL).
 */
bool
UserInputModeVolumeEdit::getVolumeEditInfoForStatus(VolumeEditInfo& volumeEditInfo)
{
    const bool setObliqueMprMatrixFlag(false);
    return getVolumeEditInfo(volumeEditInfo,
                             VolumeSliceViewPlaneEnum::ALL,
                             setObliqueMprMatrixFlag);
}

/**
 * Get information about volume data being edited for the given slice plane
 *
 * @param volumeEditInfo
 *    Loaded with editing information upon sucess.
 * @param sliceViewPlane
 *    The slice view plane
 * @return
 *    True if all ofvolumeEditInfo is valid, else false.  Even
 *    if false the overlay and model volume MAY be valid (non-NULL).
 */
bool
UserInputModeVolumeEdit::getVolumeEditInfoForEditOperation(VolumeEditInfo& volumeEditInfo,
                                                           const VolumeSliceViewPlaneEnum::Enum sliceViewPlane)
{
    const bool setObliqueMprMatrixFlag(true);
    return getVolumeEditInfo(volumeEditInfo,
                             sliceViewPlane,
                             setObliqueMprMatrixFlag);
}


/**
 * Get information about volume data being edited.
 *
 * @param volumeEditInfo
 *    Loaded with editing information upon sucess.
 * @param sliceViewPlane
 *    The slice view plane
 * @param setObliqueMprMatrixFlag
 *    If true, set the oblique/MPR matrix.  Turn this off when the plane is ALL as there is no matrix for ALL
 * @return
 *    True if all ofvolumeEditInfo is valid, else false.  Even
 *    if false the overlay and model volume MAY be valid (non-NULL).
 */
bool
UserInputModeVolumeEdit::getVolumeEditInfo(VolumeEditInfo& volumeEditInfo,
                                           const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                           const bool setObliqueMprMatrixFlag)
{
    volumeEditInfo.m_overlaySet     = NULL;
    volumeEditInfo.m_underlayVolume = NULL;
    volumeEditInfo.m_topOverlay     = NULL;
    volumeEditInfo.m_volumeOverlay  = NULL;
    volumeEditInfo.m_volumeFile     = NULL;
    volumeEditInfo.m_mapIndex       = -1;
    volumeEditInfo.m_sliceViewPlane = VolumeSliceViewPlaneEnum::ALL;
    volumeEditInfo.m_sliceProjectionType = VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL;
    volumeEditInfo.m_obliqueMprRotationMatrix.identity();
    
    EventBrowserWindowDrawingContent windowEvent(getBrowserWindowIndex());
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
                            volumeEditInfo.m_sliceViewPlane = sliceViewPlane;
                            volumeEditInfo.m_sliceProjectionType = tabContent->getVolumeSliceProjectionType();
                            volumeEditInfo.m_volumeFileEditorDelegate = vf->getVolumeFileEditorDelegate();
                            volumeEditInfo.m_obliqueMprRotationMatrix = Matrix4x4();
                            
                            if (setObliqueMprMatrixFlag) {
                                switch (tabContent->getVolumeSliceProjectionType()) {
                                    case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR:
                                        volumeEditInfo.m_obliqueMprRotationMatrix = tabContent->getMprRotationMatrix4x4ForSlicePlane(ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES,
                                                                                                                                     sliceViewPlane);
                                        break;
                                    case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR_THREE:
                                        volumeEditInfo.m_obliqueMprRotationMatrix = tabContent->getMprThreeRotationMatrixForSlicePlane(sliceViewPlane);
                                        break;
                                    case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
                                        volumeEditInfo.m_obliqueMprRotationMatrix = tabContent->getObliqueVolumeRotationMatrix();
                                        break;
                                    case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
                                        break;
                                }
                            }
                            
                            return true;
                        }
                    }
                }
            }
        }
    }
    
    return false;
}

