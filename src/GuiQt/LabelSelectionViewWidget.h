#ifndef __LABEL_SELECTION_VIEW_WIDGET_H__
#define __LABEL_SELECTION_VIEW_WIDGET_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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



#include <memory>

#include <QWidget>

#include "DisplayGroupEnum.h"
#include "EventListenerInterface.h"
#include "SceneableInterface.h"

class QStackedWidget;

namespace caret {

    class DisplayGroupEnumComboBox;
    class EnumComboBoxTemplate;
    class LabelSelectionViewController;
    class LabelSelectionViewHierarchyController;
    
    class LabelSelectionViewWidget : public QWidget, public EventListenerInterface, public SceneableInterface {
        
        Q_OBJECT

    public:
        LabelSelectionViewWidget(const int32_t browserWindowIndex,
                                 const QString& parentObjectName,
                                 QWidget* parent);
        
        virtual ~LabelSelectionViewWidget();
        
        LabelSelectionViewWidget(const LabelSelectionViewWidget&) = delete;

        LabelSelectionViewWidget& operator=(const LabelSelectionViewWidget&) = delete;
        
        void receiveEvent(Event* event);
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        

        // ADD_NEW_METHODS_HERE

    private slots:
        void labelViewModeEnumComboBoxItemActivated();

        void labelDisplayGroupSelected(const DisplayGroupEnum::Enum);
        
    private:
        void updateAfterSelectionChanges();
        
        const int32_t m_browserWindowIndex;
        
        EnumComboBoxTemplate* m_labelViewModeEnumComboBox;
        
        QStackedWidget* m_stackedWidget;
        
        LabelSelectionViewController* m_labelViewController;
        
        LabelSelectionViewHierarchyController* m_labelViewHierarchyController;
        
        int32_t m_labelViewControllerTabIndex = -1;
        
        int32_t m_labelViewHierarchyControllerTabIndex = -1;
        
        DisplayGroupEnumComboBox* m_labelsDisplayGroupComboBox;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __LABEL_SELECTION_VIEW_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __LABEL_SELECTION_VIEW_WIDGET_DECLARE__

} // namespace
#endif  //__LABEL_SELECTION_VIEW_WIDGET_H__
