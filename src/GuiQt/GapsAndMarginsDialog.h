#ifndef __GAPS_AND_MARGINS_DIALOG_H__
#define __GAPS_AND_MARGINS_DIALOG_H__

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



#include "EventListenerInterface.h"
#include "WuQDialogNonModal.h"

class QCheckBox;
class QLabel;
class QSignalMapper;
class QDoubleSpinBox;

namespace caret {

    class WuQGridLayoutGroup;
    
    class GapsAndMarginsDialog : public WuQDialogNonModal, public EventListenerInterface {
        
        Q_OBJECT

    public:
        GapsAndMarginsDialog(QWidget* parent = 0);
        
        virtual ~GapsAndMarginsDialog();
        
        virtual void updateDialog();

        // ADD_NEW_METHODS_HERE

        virtual void receiveEvent(Event* event);

    private slots:
        void tabMarginChanged(int tabIndex);
        
        void surfaceMontageGapChanged();
        
        void volumeMontageGapChanged();
        
        void tabMarginCheckBoxClicked();
        
    private:
        GapsAndMarginsDialog(const GapsAndMarginsDialog&);

        GapsAndMarginsDialog& operator=(const GapsAndMarginsDialog&);
        
        QWidget* createGapsWidget();
        
        QWidget* createMarginsWidget();
        
        QDoubleSpinBox* createPercentageSpinBox();
        
        WuQGridLayoutGroup* m_gridLayoutGroup;
        
        std::vector<QLabel*> m_tabNumberLabels;
        
        std::vector<QDoubleSpinBox*> m_leftMarginSpinBoxes;
        
        std::vector<QDoubleSpinBox*> m_rightMarginSpinBoxes;
        
        std::vector<QDoubleSpinBox*> m_bottomMarginSpinBoxes;
        
        std::vector<QDoubleSpinBox*> m_topMarginSpinBoxes;
        
        QCheckBox* m_leftMarginCheckBox;
        
        QCheckBox* m_rightMarginCheckBox;
        
        QCheckBox* m_bottomMarginCheckBox;
        
        QCheckBox* m_topMarginCheckBox;
        
        QDoubleSpinBox* m_surfaceMontageHorizontalGapSpinBox;
        
        QDoubleSpinBox* m_surfaceMontageVerticalGapSpinBox;
        
        QDoubleSpinBox* m_volumeMontageHorizontalGapSpinBox;
        
        QDoubleSpinBox* m_volumeMontageVerticalGapSpinBox;
        
        QSignalMapper* m_tabIndexSignalMapper;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __GAPS_AND_MARGINS_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __GAPS_AND_MARGINS_DIALOG_DECLARE__

} // namespace
#endif  //__GAPS_AND_MARGINS_DIALOG_H__
