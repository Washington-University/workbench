#ifndef __ANNOTATION_TEXT_SUBSTITUTION_VIEW_CONTROLLER_H__
#define __ANNOTATION_TEXT_SUBSTITUTION_VIEW_CONTROLLER_H__

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

#include "DisplayGroupEnum.h"
#include "EventListenerInterface.h"
#include "SceneableInterface.h"

class QCheckBox;
class QLabel;
class QRadioButton;
class QSpinBox;

namespace caret {
    class AnnotationTextSubstitutionFile;
    class MapYokingGroupComboBox;
    class SceneClassAssistant;

    class AnnotationTextSubstitutionViewController : public QWidget, public EventListenerInterface, public SceneableInterface {
        Q_OBJECT
        
    public:
        AnnotationTextSubstitutionViewController(const int32_t browserWindowIndex,
                                          QWidget* parent);
        
        virtual ~AnnotationTextSubstitutionViewController();
        

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
        void enableCheckBoxClicked(bool);
        
        void fileRadioButtonClicked(int32_t fileIndex);
        
        void valueIndexSpinBoxChanged(int32_t fileIndex);
        
        void fileMapYokingGroupComboBoxChanged(int32_t fileIndex);
        
    private:
        class FileGuiControls {
        public:
            FileGuiControls(const int32_t index);
            
            FileGuiControls(QRadioButton* selectionRadioButton,
                            QSpinBox* valueIndexSpinBox,
                            MapYokingGroupComboBox* mapYokingGroupSpinBox,
                            QLabel* fileNameLabel)
            : m_selectionRadioButton(selectionRadioButton),
            m_valueIndexSpinBox(valueIndexSpinBox),
            m_mapYokingGroupComboBox(mapYokingGroupSpinBox),
            m_fileNameLabel(fileNameLabel),
            m_dataFile(NULL) { }
            
            QRadioButton* m_selectionRadioButton;
            
            QSpinBox* m_valueIndexSpinBox;
            
            MapYokingGroupComboBox* m_mapYokingGroupComboBox;
            
            QLabel* m_fileNameLabel;
            
            AnnotationTextSubstitutionFile* m_dataFile;
        };
        
        AnnotationTextSubstitutionViewController(const AnnotationTextSubstitutionViewController&);

        AnnotationTextSubstitutionViewController& operator=(const AnnotationTextSubstitutionViewController&);
        
        void updateSelections();
        
        void updateOtherViewControllersAndGraphics(const int32_t fileIndex);
        
        SceneClassAssistant* m_sceneAssistant;

        QCheckBox* m_enableSubstitutionsCheckBox;
        
        int32_t m_browserWindowIndex;
        
        std::vector<FileGuiControls*> m_fileGuiControls;

        static std::set<AnnotationTextSubstitutionViewController*> s_allAnnotationTextSubstitutionViewControllers;
        // ADD_NEW_MEMBERS_HERE
        
        static const int32_t MAXIMUM_NUMBER_OF_FILES = 25;

    };
    
#ifdef __ANNOTATION_TEXT_SUBSTITUTION_VIEW_CONTROLLER_DECLARE__
    std::set<AnnotationTextSubstitutionViewController*> AnnotationTextSubstitutionViewController::s_allAnnotationTextSubstitutionViewControllers;
#endif // __ANNOTATION_TEXT_SUBSTITUTION_VIEW_CONTROLLER_DECLARE__

} // namespace
#endif  //__ANNOTATION_TEXT_SUBSTITUTION_VIEW_CONTROLLER_H__
