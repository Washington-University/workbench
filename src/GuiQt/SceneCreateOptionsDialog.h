#ifndef __SCENE_CREATE_OPTIONS_DIALOG_H__
#define __SCENE_CREATE_OPTIONS_DIALOG_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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
class QToolButton;

namespace caret {

    class SceneCreateOptionsDialog : public WuQDialogModal {
        
        Q_OBJECT

    public:
        /**
         * Options for display in dialog
         */
        class Options {
        public:
            /**
             * Constructor
             *
             * @param useSceneColorsSelected
             *     Status of use scene colors
             */
            Options(const bool useSceneColorsSelected)
            : m_useSceneColorsSelected(useSceneColorsSelected) { }
            
            /** @return Status of use scene colors */
            bool isUseSceneColorsSelected() const { return m_useSceneColorsSelected; }
        
        private:
            bool m_useSceneColorsSelected = false;
        };
        
        
        SceneCreateOptionsDialog(const Options& options,
                               QWidget* parent = 0);
        
        virtual ~SceneCreateOptionsDialog();
        
        Options getOptions() const;
        
        // ADD_NEW_METHODS_HERE

    protected:
        virtual void okButtonClicked();
        
        void useSceneColorsInfoButtonClicked();
        
    private:
        SceneCreateOptionsDialog(const SceneCreateOptionsDialog&);

        SceneCreateOptionsDialog& operator=(const SceneCreateOptionsDialog&);
        
        QCheckBox* m_useSceneColorsCheckBox;
        
        QToolButton* m_useSceneColorsToolButton;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __SCENE_CREATE_OPTIONS_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SCENE_CREATE_OPTIONS_DIALOG_DECLARE__

} // namespace
#endif  //__SCENE_CREATE_OPTIONS_DIALOG_H__
