#ifndef __USER_INPUT_MODE_VOLUME_EDIT_H__
#define __USER_INPUT_MODE_VOLUME_EDIT_H__

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

#include "Matrix4x4.h"
#include "UserInputModeView.h"
#include "VolumeSliceViewPlaneEnum.h"
#include "VolumeSliceProjectionTypeEnum.h"

namespace caret {

    class ModelVolume;
    class Overlay;
    class OverlaySet;
    class VolumeFile;
    class VolumeFileEditorDelegate;
    class VolumeMappableInterface;
    
    class UserInputModeVolumeEditWidget;
    
    class UserInputModeVolumeEdit : public UserInputModeView {
        
    public:
        /**
         * Contains information regarding the volume file that is
         * being edited.
         */
        struct VolumeEditInfo {
            /** The overlay set in the tab */
            OverlaySet* m_overlaySet = NULL;
            
            /** The underlay volume */
            VolumeMappableInterface* m_underlayVolume = NULL;
            
            /** The top-most overlay in the tab */
            Overlay* m_topOverlay = NULL;
            
            /** The overlay containing the volume file */
            Overlay* m_volumeOverlay = NULL;
            
            /** The volume file being edited */
            VolumeFile* m_volumeFile = NULL;
            
            /** Index of the map in the volume file being edited */
            int32_t m_mapIndex;
            
            /** The current slice view plane */
            VolumeSliceViewPlaneEnum::Enum m_sliceViewPlane;
            
            /** Slice projection type (orthogonal/oblique) */
            VolumeSliceProjectionTypeEnum::Enum m_sliceProjectionType;
            
            /** The volume's editor delegate */
            VolumeFileEditorDelegate* m_volumeFileEditorDelegate;
            
            /** The oblique or MPR projection rotation matrix */
            Matrix4x4 m_obliqueMprRotationMatrix;
        };
        
        UserInputModeVolumeEdit(const int32_t browserIndexIndex);
        
        virtual ~UserInputModeVolumeEdit();
        
        virtual void initialize();
        
        virtual void finish();
        
        virtual void update();
        
        virtual CursorEnum::Enum getCursor() const;
        
        virtual void mouseLeftClick(const MouseEvent& mouseEvent);
        
        virtual void mouseLeftDragWithCtrlShift(const MouseEvent& mouseEvent);

        bool getVolumeEditInfoForStatus(VolumeEditInfo& volumeEditInfo);
        
        bool getVolumeEditInfoForEditOperation(VolumeEditInfo& volumeEditInfo,
                                               const VolumeSliceViewPlaneEnum::Enum sliceViewPlane);
        
        virtual void showContextMenu(const MouseEvent& mouseEvent,
                                     const QPoint& menuPosition,
                                     BrainOpenGLWidget* openGLWidget);
        
        void updateGraphicsAfterEditing();
        
        // ADD_NEW_METHODS_HERE

    private:
        UserInputModeVolumeEdit(const UserInputModeVolumeEdit&);

        UserInputModeVolumeEdit& operator=(const UserInputModeVolumeEdit&);
        
        bool getVolumeEditInfo(VolumeEditInfo& volumeEditInfo,
                               const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                               const bool setObliqueMprMatrixFlag);
        
        void processEditCommandFromMouse(const MouseEvent& mouseEvent);
        
        UserInputModeVolumeEditWidget* m_inputModeVolumeEditWidget;
        
        friend class UserInputModeVolumeEditWidget;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __USER_INPUT_MODE_VOLUME_EDIT_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __USER_INPUT_MODE_VOLUME_EDIT_DECLARE__

} // namespace
#endif  //__USER_INPUT_MODE_VOLUME_EDIT_H__
