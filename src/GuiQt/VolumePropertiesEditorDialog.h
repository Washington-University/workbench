#ifndef __VOLUME_PROPERTIES_EDITOR_DIALOG__H_
#define __VOLUME_PROPERTIES_EDITOR_DIALOG__H_

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

#include "EventListenerInterface.h"
#include "SceneableInterface.h"
#include "WuQDialogNonModal.h"

class QDoubleSpinBox;

namespace caret {
    class VolumePropertiesEditorDialog : public WuQDialogNonModal, public EventListenerInterface, public SceneableInterface {
        Q_OBJECT
        
    public:
        VolumePropertiesEditorDialog(QWidget* parent = 0);
        
        virtual ~VolumePropertiesEditorDialog();
        
        void receiveEvent(Event* event);
        
        void updateDialog();
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
    private slots:
        void displayPropertyChanged();
        
    private:
        VolumePropertiesEditorDialog(const VolumePropertiesEditorDialog&);

        VolumePropertiesEditorDialog& operator=(const VolumePropertiesEditorDialog&);
        
        QDoubleSpinBox* m_opacitySpinBox;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __VOLUME_PROPERTIES_EDITOR_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __VOLUME_PROPERTIES_EDITOR_DIALOG_DECLARE__

} // namespace
#endif  //__VOLUME_PROPERTIES_EDITOR_DIALOG__H_
