#ifndef __ANNOTATION_SELECTION_VIEW_CONTROLLER__H_
#define __ANNOTATION_SELECTION_VIEW_CONTROLLER__H_

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

    class GroupAndNameHierarchyViewController;
    class DisplayGroupEnumComboBox;
    class EnumComboBoxTemplate;
    class WuQTabWidget;
    
    class AnnotationSelectionViewController : public QWidget, public EventListenerInterface, public SceneableInterface {
        
        Q_OBJECT

    public:
        AnnotationSelectionViewController(const int32_t browserWindowIndex,
                              QWidget* parent = 0);
        
        virtual ~AnnotationSelectionViewController();
        
        void receiveEvent(Event* event);
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
    private slots:
        void processAnnotationSelectionChanges();
        
        void processSelectionChanges();
        
        void annotationDisplayGroupSelected(const DisplayGroupEnum::Enum);
        
        void processAttributesChanges();
        
    private:
        AnnotationSelectionViewController(const AnnotationSelectionViewController&);

        AnnotationSelectionViewController& operator=(const AnnotationSelectionViewController&);

        void updateAnnotationViewController();
        
        void updateOtherAnnotationViewControllers();
        
        QWidget* createSelectionWidget();
        
        QWidget* createAttributesWidget();
        
        int32_t m_browserWindowIndex;
        
        GroupAndNameHierarchyViewController* m_annotationClassNameHierarchyViewController;
        
        QCheckBox* m_annotationsDisplayCheckBox;
        
        DisplayGroupEnumComboBox* m_annotationsDisplayGroupComboBox;

        WuQTabWidget* m_tabWidget;
        
        static std::set<AnnotationSelectionViewController*> allAnnotationSelectionViewControllers;
    };
    
#ifdef __ANNOTATION_SELECTION_VIEW_CONTROLLER_DECLARE__
    std::set<AnnotationSelectionViewController*> AnnotationSelectionViewController::allAnnotationSelectionViewControllers;
#endif // __ANNOTATION_SELECTION_VIEW_CONTROLLER_DECLARE__

} // namespace
#endif  //__ANNOTATION_SELECTION_VIEW_CONTROLLER__H_
