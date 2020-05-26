#ifndef __FIBER_ORIENTATION_SELECTION_VIEW_CONTROLLER__H_
#define __FIBER_ORIENTATION_SELECTION_VIEW_CONTROLLER__H_

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

#include <set>

#include <QWidget>

#include "DisplayGroupEnum.h"
#include "EnumComboBoxTemplate.h"
#include "EventListenerInterface.h"
#include "SceneableInterface.h"

class QCheckBox;
class QDoubleSpinBox;
class QVBoxLayout;

namespace caret {
    class DisplayGroupEnumComboBox;
    class FiberSamplesOpenGLWidget;
    class WuQTabWidget;
    class WuQTrueFalseComboBox;

    class FiberOrientationSelectionViewController : public QWidget, public EventListenerInterface, public SceneableInterface {
        
        Q_OBJECT

    public:
        FiberOrientationSelectionViewController(const int32_t browserWindowIndex,
                                                QWidget* parent = 0);
        
        virtual ~FiberOrientationSelectionViewController();
        
        void receiveEvent(Event* event);
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
        // ADD_NEW_METHODS_HERE
        
    private slots:
        void processSelectionChanges();
        
        void displayGroupSelected(const DisplayGroupEnum::Enum);
        
        void processAttributesChanges();
        
    private:
        FiberOrientationSelectionViewController(const FiberOrientationSelectionViewController&);

        FiberOrientationSelectionViewController& operator=(const FiberOrientationSelectionViewController&);
        
        void updateViewController();
        
        void updateOtherViewControllers();
        
        QWidget* createSelectionWidget();
        
        QWidget* createAttributesWidget();
        
        QWidget* createSamplesWidget();
        
        const int32_t m_browserWindowIndex;
        
        DisplayGroupEnumComboBox* m_displayGroupComboBox;
        
        QCheckBox* m_displayFibersCheckBox;
        
        std::vector<QCheckBox*> m_fileSelectionCheckBoxes;
        
        QDoubleSpinBox* m_aboveLimitSpinBox;
        
        QDoubleSpinBox* m_belowLimitSpinBox;
        
        QDoubleSpinBox* m_minimumMagnitudeSpinBox;
        
        QDoubleSpinBox* m_maximumUncertaintySpinBox;
        
        QDoubleSpinBox* m_lengthMultiplierSpinBox;
        
        QDoubleSpinBox* m_fanMultiplierSpinBox;
        
        EnumComboBoxTemplate* m_coloringTypeComboBox;
        
        EnumComboBoxTemplate* m_symbolTypeComboBox;
        
        QCheckBox* m_drawWithMagnitudeCheckBox;
        
        QVBoxLayout* m_selectionWidgetLayout;
        
        FiberSamplesOpenGLWidget* m_samplesOpenGLWidget;
        
        QCheckBox* m_displaySphereOrientationsCheckBox;
        
        WuQTabWidget* m_tabWidget;
        
        bool m_updateInProgress;
        
        // ADD_NEW_MEMBERS_HERE

        static std::set<FiberOrientationSelectionViewController*> s_allViewControllers;
        
    };
    
#ifdef __FIBER_ORIENTATION_SELECTION_VIEW_CONTROLLER_DECLARE__
    std::set<FiberOrientationSelectionViewController*> FiberOrientationSelectionViewController::s_allViewControllers;
#endif // __FIBER_ORIENTATION_SELECTION_VIEW_CONTROLLER_DECLARE__

} // namespace
#endif  //__FIBER_ORIENTATION_SELECTION_VIEW_CONTROLLER__H_
