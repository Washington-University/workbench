#ifndef __BORDER_SELECTION_VIEW_CONTROLLER__H_
#define __BORDER_SELECTION_VIEW_CONTROLLER__H_

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
    
    class BorderSelectionViewController : public QWidget, public EventListenerInterface, public SceneableInterface {
        
        Q_OBJECT

    public:
        BorderSelectionViewController(const int32_t browserWindowIndex,
                                      const QString& parentObjectName,
                                      QWidget* parent = 0);
        
        virtual ~BorderSelectionViewController();
        
        void receiveEvent(Event* event);
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
    private slots:
        void processBorderSelectionChanges();
        
        void processSelectionChanges();
        
        void borderDisplayGroupSelected(const DisplayGroupEnum::Enum);
        
        void processAttributesChanges();
        
    private:
        BorderSelectionViewController(const BorderSelectionViewController&);

        BorderSelectionViewController& operator=(const BorderSelectionViewController&);

        void updateBorderViewController();
        
        void updateOtherBorderViewControllers();
        
        QWidget* createSelectionWidget();
        
        QWidget* createAttributesWidget();
        
        const QString m_objectNamePrefix;
        
        int32_t m_browserWindowIndex;
        
        GroupAndNameHierarchyViewController* m_borderClassNameHierarchyViewController;
        
        QCheckBox* m_bordersDisplayCheckBox;
        
        QCheckBox* m_bordersContralateralCheckBox;
        
        DisplayGroupEnumComboBox* m_bordersDisplayGroupComboBox;

        QComboBox* m_drawTypeComboBox;
        
        EnumComboBoxTemplate* m_coloringTypeComboBox;
        
        CaretColorEnumComboBox* m_standardColorComboBox;
        
        QDoubleSpinBox* m_lineWidthSpinBox;
        
        QDoubleSpinBox* m_pointSizeSpinBox;
        
        QCheckBox* m_enableUnstretchedLinesCheckBox;
        
        QDoubleSpinBox* m_unstretchedLinesLengthSpinBox;
        
        QDoubleSpinBox* m_aboveSurfaceOffsetSpinBox;
        
        WuQTabWidget* m_tabWidget;
        
        static std::set<BorderSelectionViewController*> allBorderSelectionViewControllers;
    };
    
#ifdef __BORDER_SELECTION_VIEW_CONTROLLER_DECLARE__
    std::set<BorderSelectionViewController*> BorderSelectionViewController::allBorderSelectionViewControllers;
#endif // __BORDER_SELECTION_VIEW_CONTROLLER_DECLARE__

} // namespace
#endif  //__BORDER_SELECTION_VIEW_CONTROLLER__H_
