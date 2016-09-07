#ifndef __IMAGE_SELECTION_VIEW_CONTROLLER_H__
#define __IMAGE_SELECTION_VIEW_CONTROLLER_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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
#include <vector>

#include <QWidget>

#include "DisplayGroupEnum.h"
#include "EventListenerInterface.h"
#include "SceneableInterface.h"

class QButtonGroup;
class QCheckBox;
class QDoubleSpinBox;
class QGridLayout;
class QRadioButton;
class QSignalMapper;

namespace caret {
    class DisplayGroupEnumComboBox;
    class EnumComboBoxTemplate;
    class SceneClassAssistant;
    class WuQTabWidget;

    class ImageSelectionViewController : public QWidget, public EventListenerInterface, public SceneableInterface {
        
        Q_OBJECT

    public:
        ImageSelectionViewController(const int32_t browserWindowIndex,
                                     QWidget* parent = 0);
        
        virtual ~ImageSelectionViewController();
        

        // ADD_NEW_METHODS_HERE

        virtual void receiveEvent(Event* event);

        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

          
    private slots:
        void processAttributesChanges();
        
        void imageDisplayGroupSelected(const DisplayGroupEnum::Enum);

        void imageRadioButtonClicked(int);
          
// If there will be sub-classes of this class that need to save
// and restore data from scenes, these pure virtual methods can
// be uncommented to force their implemetation by sub-classes.
//    protected: 
//        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
//                                             SceneClass* sceneClass) = 0;
//
//        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
//                                                  const SceneClass* sceneClass) = 0;

    private:
        ImageSelectionViewController(const ImageSelectionViewController&);

        ImageSelectionViewController& operator=(const ImageSelectionViewController&);
        
        void updateOtherImageViewControllers();
        
        void updateImageViewController();
        
        QWidget* createSelectionWidget();
        
        QWidget* createAttributesWidget();
        
        const int32_t m_browserWindowIndex;
        
        WuQTabWidget* m_tabWidget;
        
        SceneClassAssistant* m_sceneAssistant;

        DisplayGroupEnumComboBox* m_imagesDisplayGroupComboBox;
        
        QCheckBox* m_imageDisplayCheckBox;
        
        std::vector<QRadioButton*> m_imageRadioButtons;
        
        QButtonGroup* m_imageRadioButtonGroup;
        
        QGridLayout* m_imageRadioButtonLayout;
        
        EnumComboBoxTemplate* m_depthComboBox;
        
        QDoubleSpinBox* m_thresholdMinimumSpinBox;
        
        QDoubleSpinBox* m_thresholdMaximumSpinBox;
        
        QDoubleSpinBox* m_opacitySpinBox;
        
        static std::set<ImageSelectionViewController*> s_allImageSelectionViewControllers;
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __IMAGE_SELECTION_VIEW_CONTROLLER_DECLARE__
    std::set<ImageSelectionViewController*> ImageSelectionViewController::s_allImageSelectionViewControllers;
#endif // __IMAGE_SELECTION_VIEW_CONTROLLER_DECLARE__

} // namespace
#endif  //__IMAGE_SELECTION_VIEW_CONTROLLER_H__
