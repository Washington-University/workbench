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


#include "UserInputModeView.h"
#include "VolumeSliceViewPlaneEnum.h"

namespace caret {

    class ModelVolume;
    class VolumeFile;
    class VolumeFileEditorDelegate;
    
    class UserInputModeVolumeEditWidget;
    
    class UserInputModeVolumeEdit : public UserInputModeView {
        
    public:
        struct VolumeEditInfo {
            ModelVolume* m_modelVolume;
            VolumeFile* m_volumeFile;
            int32_t m_mapIndex;
            VolumeSliceViewPlaneEnum::Enum m_sliceViewPlane;
            VolumeFileEditorDelegate* m_volumeFileEditorDelegate;
        };
        
        UserInputModeVolumeEdit(const int32_t windowIndex);
        
        virtual ~UserInputModeVolumeEdit();
        
        virtual UserInputMode getUserInputMode() const;
        
        virtual void initialize();
        
        virtual void finish();
        
        virtual void update();
        
        virtual QWidget* getWidgetForToolBar();
        
        virtual CursorEnum::Enum getCursor() const;
        
        virtual void mouseLeftClick(const MouseEvent& mouseEvent);
        
        bool getVolumeEditInfo(VolumeEditInfo& volumeEditInfo);
        
        void updateGraphicsAfterEditing(VolumeFile* volumeFile,
                                        const int32_t mapIndex);
        
        // ADD_NEW_METHODS_HERE

    private:
        UserInputModeVolumeEdit(const UserInputModeVolumeEdit&);

        UserInputModeVolumeEdit& operator=(const UserInputModeVolumeEdit&);
        
        const int32_t m_windowIndex;
        
        UserInputModeVolumeEditWidget* m_inputModeVolumeEditWidget;
        
        friend class UserInputModeVolumeEditWidget;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __USER_INPUT_MODE_VOLUME_EDIT_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __USER_INPUT_MODE_VOLUME_EDIT_DECLARE__

} // namespace
#endif  //__USER_INPUT_MODE_VOLUME_EDIT_H__
