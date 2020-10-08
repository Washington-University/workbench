#ifndef __SURFACE_PROPERTIES_EDITOR_DIALOG__H_
#define __SURFACE_PROPERTIES_EDITOR_DIALOG__H_

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

#include <array>

#include "EventListenerInterface.h"
#include "SceneableInterface.h"
#include "WuQDialogNonModal.h"

class QCheckBox;
class QComboBox;
class QDoubleSpinBox;

namespace caret {
    class EnumComboBoxTemplate;
    class WuQTrueFalseComboBox;
    
    class SurfacePropertiesEditorDialog : public WuQDialogNonModal, public EventListenerInterface, public SceneableInterface {
        Q_OBJECT
        
    public:
        SurfacePropertiesEditorDialog(QWidget* parent = 0);
        
        virtual ~SurfacePropertiesEditorDialog();
        
        void receiveEvent(Event* event);
        
        void updateDialog();
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
    private slots:
        void surfaceSetColorToolButtonClicked();
        
        void surfaceResetColorToolButtonClicked();

        void backfaceCullingCheckBoxClicked(bool checked);
        
    private:
        SurfacePropertiesEditorDialog(const SurfacePropertiesEditorDialog&);

        SurfacePropertiesEditorDialog& operator=(const SurfacePropertiesEditorDialog&);
        
        void updateDefaultSurfaceColorWidget();
        
        QCheckBox* m_displayNormalVectorsCheckBox;
        
        QCheckBox* m_backfaceCullingCheckBox;
        
        QDoubleSpinBox* m_linkSizeSpinBox;
        
        QDoubleSpinBox* m_nodeSizeSpinBox;
        
        QComboBox* m_surfaceDrawingTypeComboBox;
        
        QDoubleSpinBox* m_opacitySpinBox;
        
        QWidget* m_surfaceColorWidget;
        
        bool m_updateInProgress;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __SURFACE_PROPERTIES_EDITOR_DIALOG_DECLARE__
#endif // __SURFACE_PROPERTIES_EDITOR_DIALOG_DECLARE__

} // namespace
#endif  //__SURFACE_PROPERTIES_EDITOR_DIALOG__H_
