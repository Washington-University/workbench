#ifndef __USER_INPUT_MODE_IMAGE__H_
#define __USER_INPUT_MODE_IMAGE__H_

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


#include "CaretObject.h"
#include "UserInputModeView.h"

namespace caret {

    class ImageFile;
    class SelectionItemImage;
    class SelectionItemImageControlPoint;
    class SelectionItemVoxel;
    class UserInputModeImageWidget;
    
    class UserInputModeImage : public UserInputModeView {
        
    public:
        enum EditOperation {
            EDIT_OPERATION_ADD,
            EDIT_OPERATION_DELETE
        };
        
        UserInputModeImage(const int32_t windowIndex);
        
        virtual ~UserInputModeImage();
        
        virtual void initialize();
        
        virtual void finish();
        
        virtual void update();
        
        virtual CursorEnum::Enum getCursor() const;
        
        virtual void mouseLeftClick(const MouseEvent& mouseEvent);
        
        virtual void showContextMenu(const MouseEvent& mouseEvent,
                                     const QPoint& menuPosition,
                                     BrainOpenGLWidget* openGLWidget);
        
    private:
        /*
         * Note some private methods are accessed by the 
         * friend UserInputModeImageWidget.
         */
        friend class UserInputModeImageWidget;
        
        
        UserInputModeImage(const UserInputModeImage&);

        UserInputModeImage& operator=(const UserInputModeImage&);
        
        EditOperation getEditOperation() const;
        
        void setEditOperation(const EditOperation editOperation);
        
        void updateAfterControlPointsChanged();
        
        void addControlPoint(SelectionItemImage* imageSelection,
                             const SelectionItemVoxel* voxelSelection);
        
        void deleteControlPoint(SelectionItemImageControlPoint* idImageControlPoint);
        
        void deleteAllControlPoints();
        
        ImageFile* getImageFile() const;
        
        int32_t getTabIndex() const;
        
        // ADD_NEW_MEMBERS_HERE
        
        const int32_t m_windowIndex;

        UserInputModeImageWidget* m_inputModeImageWidget;
        
        EditOperation m_editOperation;
    };
    
#ifdef __USER_INPUT_MODE_IMAGE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __USER_INPUT_MODE_IMAGE_DECLARE__

} // namespace
#endif  //__USER_INPUT_MODE_IMAGE__H_
