#ifndef __SAMPLES_SELECTION_VIEW_CONTROLLER_H__
#define __SAMPLES_SELECTION_VIEW_CONTROLLER_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#include "ContextSensitiveMenuItemsEnum.h"
#include "DisplayGroupEnum.h"
#include "EventListenerInterface.h"
#include "SceneableInterface.h"

class QCheckBox;
class QTreeWidgetItem;

namespace caret {
    class AnnotationPolyhedron;
    class DisplayGroupAndTabItemViewController;
    class DisplayGroupEnumComboBox;
    class EnumComboBoxTemplate;
    class SceneClassAssistant;

    class SamplesSelectionViewController : public QWidget, public EventListenerInterface, public SceneableInterface {
        Q_OBJECT
        
    public:
        SamplesSelectionViewController(const int32_t browserWindowIndex,
                                          const QString& parentObjectName,
                                          QWidget* parent);
        
        virtual ~SamplesSelectionViewController();
        

        // ADD_NEW_METHODS_HERE

        virtual void receiveEvent(Event* event);

        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

          
          
          
          
          
// If there will be sub-classes of this class that need to save
// and restore data from scenes, these pure virtual methods can
// be uncommented to force their implemetation by sub-classes.
//    protected: 
//        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
//                                             SceneClass* sceneClass) = 0;
//
//        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
//                                                  const SceneClass* sceneClass) = 0;

    private slots:
        void samplesColorModeEnumComboBoxItemActivated();
        
        void checkBoxToggled();
        
        void displayGroupSelected(const DisplayGroupEnum::Enum);
        
        void contextMenuItemSelected(QList<QTreeWidgetItem*>& itemsSelected,
                                     const ContextSensitiveMenuItemsEnum::Enum contextMenuItem);
        
    private:
        SamplesSelectionViewController(const SamplesSelectionViewController&);

        SamplesSelectionViewController& operator=(const SamplesSelectionViewController&);
        
        QWidget* createSelectionWidget(const AString& objectNamePrefix);
        
        void updateSampleSelections();
        
        void updateOtherSampleViewControllers();
        
        std::vector<AnnotationPolyhedron*> getPolyhedronsFromTreeWidgetItems(QList<QTreeWidgetItem*>& treeWidgetItems) const;
        
        SceneClassAssistant* m_sceneAssistant;

        int32_t m_browserWindowIndex;
        
        DisplayGroupEnumComboBox* m_displayGroupComboBox;
        
        DisplayGroupAndTabItemViewController* m_selectionViewController;
        
        QCheckBox* m_displaySamplesCheckBox;
        
        QCheckBox* m_displaySampleNamesCheckBox;
        
        QCheckBox* m_displaySamplesNumberCheckBox;
        
        QCheckBox* m_displaySamplesActualDesiredSuffixCheckBox;
        
        EnumComboBoxTemplate* m_samplesColorModeEnumComboBox;;
        
        static std::set<SamplesSelectionViewController*> s_allSamplesSelectionViewControllers;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __SAMPLES_SELECTION_VIEW_CONTROLLER_DECLARE__
    std::set<SamplesSelectionViewController*> SamplesSelectionViewController::s_allSamplesSelectionViewControllers;
#endif // __SAMPLES_SELECTION_VIEW_CONTROLLER_DECLARE__

} // namespace
#endif  //__SAMPLES_SELECTION_VIEW_CONTROLLER_H__
