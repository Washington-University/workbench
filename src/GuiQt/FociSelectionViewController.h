#ifndef __FOCI_SELECTION_VIEW_CONTROLLER__H_
#define __FOCI_SELECTION_VIEW_CONTROLLER__H_

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

#include <stdint.h>
#include <set>

#include <QWidget>

#include "DisplayGroupEnum.h"
#include "EventListenerInterface.h"
#include "SceneableInterface.h"

class QCheckBox;
class QComboBox;
class QDoubleSpinBox;

namespace caret {

    class CaretColorEnumComboBox;
    class GroupAndNameHierarchyViewController;
    class DisplayGroupEnumComboBox;
    class EnumComboBoxTemplate;
    class WuQTabWidget;
    
    class FociSelectionViewController : public QWidget, public EventListenerInterface, public SceneableInterface {
        
        Q_OBJECT

    public:
        FociSelectionViewController(const int32_t browserWindowIndex,
                              QWidget* parent = 0);
        
        virtual ~FociSelectionViewController();
        
        void receiveEvent(Event* event);
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
    private slots:
        void processFociSelectionChanges();
        
        void processSelectionChanges();
        
        void fociDisplayGroupSelected(const DisplayGroupEnum::Enum);
        
        void processAttributesChanges();
        
    private:
        FociSelectionViewController(const FociSelectionViewController&);

        FociSelectionViewController& operator=(const FociSelectionViewController&);

        void updateFociViewController();
        
        void updateOtherFociViewControllers();
        
        QWidget* createSelectionWidget();
        
        QWidget* createAttributesWidget();
        
        int32_t m_browserWindowIndex;
        
        GroupAndNameHierarchyViewController* m_fociClassNameHierarchyViewController;
        
        QCheckBox* m_fociDisplayCheckBox;
        
        QCheckBox* m_fociContralateralCheckBox;
        
        QCheckBox* m_pasteOntoSurfaceCheckBox;
        
        DisplayGroupEnumComboBox* m_fociDisplayGroupComboBox;

        EnumComboBoxTemplate* m_coloringTypeComboBox;
        
        CaretColorEnumComboBox* m_standardColorComboBox;
        
        QDoubleSpinBox* m_lineWidthSpinBox;
        
        QDoubleSpinBox* m_sizeSpinBox;
        
        QComboBox* m_drawTypeComboBox;
        
        WuQTabWidget* m_tabWidget;
        
        static std::set<FociSelectionViewController*> allFociSelectionViewControllers;
    };
    
#ifdef __FOCI_SELECTION_VIEW_CONTROLLER_DECLARE__
    std::set<FociSelectionViewController*> FociSelectionViewController::allFociSelectionViewControllers;
#endif // __FOCI_SELECTION_VIEW_CONTROLLER_DECLARE__

} // namespace
#endif  //__FOCI_SELECTION_VIEW_CONTROLLER__H_
