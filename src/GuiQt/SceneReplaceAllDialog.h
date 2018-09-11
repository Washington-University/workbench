#ifndef __SCENE_REPLACE_ALL_DIALOG_H__
#define __SCENE_REPLACE_ALL_DIALOG_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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



#include <memory>

#include "WuQDialogModal.h"

class QCheckBox;

namespace caret {

    class SceneReplaceAllDialog : public WuQDialogModal {
        
        Q_OBJECT

    public:
        SceneReplaceAllDialog(const AString& replaceDescription,
                              QWidget* parent = 0);
        
        virtual ~SceneReplaceAllDialog();
        
        SceneReplaceAllDialog(const SceneReplaceAllDialog&) = delete;

        SceneReplaceAllDialog& operator=(const SceneReplaceAllDialog&) = delete;

        bool isChangeSurfaceAnnotationOffsetToOffset() const;
        
    protected:
        virtual void okButtonClicked();

        // ADD_NEW_METHODS_HERE

    private:
        QCheckBox* m_changeSurfaceAnntotationOffsetCheckBox;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __SCENE_REPLACE_ALL_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SCENE_REPLACE_ALL_DIALOG_DECLARE__

} // namespace
#endif  //__SCENE_REPLACE_ALL_DIALOG_H__
