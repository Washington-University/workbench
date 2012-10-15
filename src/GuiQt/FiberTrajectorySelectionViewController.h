#ifndef __FIBER_TRAJECTORY_SELECTION_VIEW_CONTROLLER__H_
#define __FIBER_TRAJECTORY_SELECTION_VIEW_CONTROLLER__H_

/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#include <set>

#include <QWidget>

#include "DisplayGroupEnum.h"
#include "EnumComboBoxTemplate.h"
#include "EventListenerInterface.h"

class QCheckBox;
class QDoubleSpinBox;
class QSpinBox;
class QVBoxLayout;

namespace caret {
    class DisplayGroupEnumComboBox;
    class WuQTrueFalseComboBox;

    class FiberTrajectorySelectionViewController : public QWidget, public EventListenerInterface {
        
        Q_OBJECT

    public:
        FiberTrajectorySelectionViewController(const int32_t browserWindowIndex,
                                                QWidget* parent = 0);
        
        virtual ~FiberTrajectorySelectionViewController();
        
        void receiveEvent(Event* event);
        
        // ADD_NEW_METHODS_HERE
        
    private slots:
        void processSelectionChanges();
        
        void displayGroupSelected(const DisplayGroupEnum::Enum);
        
        void processAttributesChanges();
        
    private:
        FiberTrajectorySelectionViewController(const FiberTrajectorySelectionViewController&);

        FiberTrajectorySelectionViewController& operator=(const FiberTrajectorySelectionViewController&);
        
        void updateViewController();
        
        void updateOtherViewControllers();
        
        QWidget* createSelectionWidget();
        
        QWidget* createAttributesWidget();
        
        const int32_t m_browserWindowIndex;
        
        DisplayGroupEnumComboBox* m_displayGroupComboBox;
        
        QCheckBox* m_displayFibersCheckBox;
        
        std::vector<QCheckBox*> m_fileSelectionCheckBoxes;
        
        QSpinBox* m_proportionStreamlineSpinBox;
        
        QDoubleSpinBox* m_proportionMinimumSpinBox;
        
        QDoubleSpinBox* m_proportionMaximumSpinBox;
        
        QVBoxLayout* m_selectionWidgetLayout;
        
        bool m_updateInProgress;
        
        // ADD_NEW_MEMBERS_HERE

        static std::set<FiberTrajectorySelectionViewController*> s_allViewControllers;
        
    };
    
#ifdef __FIBER_ORIENTATION_TRAJECTORY_VIEW_CONTROLLER_DECLARE__
    std::set<FiberTrajectorySelectionViewController*> FiberTrajectorySelectionViewController::s_allViewControllers;
#endif // __FIBER_ORIENTATION_TRAJECTORY_VIEW_CONTROLLER_DECLARE__

} // namespace
#endif  //__FIBER_TRAJECTORY_SELECTION_VIEW_CONTROLLER__H_
