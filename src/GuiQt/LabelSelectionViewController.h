#ifndef __LABEL_SELECTION_VIEW_CONTROLLER__H_
#define __LABEL_SELECTION_VIEW_CONTROLLER__H_

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

#include <cstdint>
#include <set>

#include <QWidget>

#include "SceneableInterface.h"

class QCheckBox;

namespace caret {

    class GroupAndNameHierarchyViewController;
    
    class LabelSelectionViewController : public QWidget, public SceneableInterface {
        
        Q_OBJECT

    public:
        LabelSelectionViewController(const int32_t browserWindowIndex,
                                     const QString& parentObjectName,
                                     QWidget* parent = 0);
        
        virtual ~LabelSelectionViewController();
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
        void updateLabelViewController();
        
    private slots:
        void processLabelSelectionChanges();
        
        void processSelectionChanges();
        
    private:
        LabelSelectionViewController(const LabelSelectionViewController&);

        LabelSelectionViewController& operator=(const LabelSelectionViewController&);

        void updateOtherLabelViewControllers();
        
        QWidget* createSelectionWidget();
        
        const QString m_objectNamePrefix;
        
        int32_t m_browserWindowIndex;
        
        GroupAndNameHierarchyViewController* m_labelClassNameHierarchyViewController;
        
        QCheckBox* m_labelsDisplayCheckBox;
        
        QCheckBox* m_labelsContralateralCheckBox;
        
        static std::set<LabelSelectionViewController*> allLabelSelectionViewControllers;
    };
    
#ifdef __LABEL_SELECTION_VIEW_CONTROLLER_DECLARE__
    std::set<LabelSelectionViewController*> LabelSelectionViewController::allLabelSelectionViewControllers;
#endif // __LABEL_SELECTION_VIEW_CONTROLLER_DECLARE__

} // namespace
#endif  //__LABEL_SELECTION_VIEW_CONTROLLER__H_
