#ifndef __NEUROGLANCER_ANNOTATIONS_SELECTION_VIEW_CONTROLLER__H_
#define __NEUROGLANCER_ANNOTATIONS_SELECTION_VIEW_CONTROLLER__H_

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
    class DisplayGroupEnumComboBox;
    class EnumComboBoxTemplate;
    class WuQTabWidget;
    
    class NeuroglancerAnnotationsSelectionViewController : public QWidget, public EventListenerInterface, public SceneableInterface {
        
        Q_OBJECT

    public:
        NeuroglancerAnnotationsSelectionViewController(const int32_t browserWindowIndex,
                                    const QString& parentObjectName,
                                    QWidget* parent = 0);
        
        virtual ~NeuroglancerAnnotationsSelectionViewController();
        
        void receiveEvent(Event* event);
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
    private slots:
        void processNeuroAnnSelectionChanges();
        
        void processSelectionChanges();
        
        void displayGroupSelected(const DisplayGroupEnum::Enum);
        
        void processAttributesChanges();
        
    private:
        NeuroglancerAnnotationsSelectionViewController(const NeuroglancerAnnotationsSelectionViewController&);

        NeuroglancerAnnotationsSelectionViewController& operator=(const NeuroglancerAnnotationsSelectionViewController&);

        void updateNeuroAnnViewController();
        
        void updateOtherNeuroAnnViewControllers();
        
        QWidget* createSelectionWidget();
        
        QWidget* createAttributesWidget();
        
        const QString m_objectNamePrefix;
        
        int32_t m_browserWindowIndex;
        
        QCheckBox* m_displayCheckBox;
        
        DisplayGroupEnumComboBox* m_displayGroupComboBox;

        QDoubleSpinBox* m_symbolScaleSpinBox;
        
        WuQTabWidget* m_tabWidget;
        
        static std::set<NeuroglancerAnnotationsSelectionViewController*> allNeuroglancerAnnotationsSelectionViewControllers;
    };
    
#ifdef __NEUROGLANCER_ANNOTATIONS_SELECTION_VIEW_CONTROLLER_DECLARE__
    std::set<NeuroglancerAnnotationsSelectionViewController*> NeuroglancerAnnotationsSelectionViewController::allNeuroglancerAnnotationsSelectionViewControllers;
#endif // __NEUROGLANCER_ANNOTATIONS_SELECTION_VIEW_CONTROLLER_DECLARE__

} // namespace
#endif  //__NEUROGLANCER_ANNOTATIONS_SELECTION_VIEW_CONTROLLER__H_
