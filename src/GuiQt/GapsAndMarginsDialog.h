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


#include "BrainConstants.h"
#include "EventListenerInterface.h"
#include "WuQDialogNonModal.h"

class QLabel;
class QSignalMapper;
class QDoubleSpinBox;
class QToolButton;

namespace caret {

    class BrainBrowserWindowComboBox;
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
        void browserWindowIndexChanged(const int32_t browserWindowIndex);
        
        void tabMarginChanged(int rowIndex);
        
        void surfaceMontageGapChanged();
        
        void volumeMontageGapChanged();
        
        void applyFirstTabToAllButtonClicked();
        
        void surfaceMontageScaleProportionatelyToolButtonClicked();
        
        void volumeMontageScaleProportionatelyToolButtonClicked();
        
        void tabMarginMatchPixelButtonClicked(int rowIndex);
        
    private:
        GapsAndMarginsDialog(const GapsAndMarginsDialog&);

        GapsAndMarginsDialog& operator=(const GapsAndMarginsDialog&);
        
        QWidget* createGapsWidget();
        
        QWidget* createMarginsWidget();
        
        QDoubleSpinBox* createPercentageSpinBox();
        
        void updateGapsSpinBoxes(const int32_t windowIndex);
        
        void updateMarginSpinBoxes(const int32_t windowIndex);
        
        float matchLeftRightMarginPercentageFromTop(const float topMarginPercentage,
                                                    const float viewportWidth,
                                                    const float viewportHeight) const;
        
        BrainBrowserWindowComboBox* m_browserWindowComboBox;
        
        WuQGridLayoutGroup* m_gridLayoutGroup;
        
        std::vector<QLabel*> m_tabNumberLabels;
        
        std::vector<QDoubleSpinBox*> m_leftMarginSpinBoxes;
        
        std::vector<QDoubleSpinBox*> m_rightMarginSpinBoxes;
        
        std::vector<QDoubleSpinBox*> m_bottomMarginSpinBoxes;
        
        std::vector<QDoubleSpinBox*> m_topMarginSpinBoxes;
        
        QToolButton* m_applyFirstTabToAllToolButton;
        
        QDoubleSpinBox* m_surfaceMontageHorizontalGapSpinBox;
        
        QDoubleSpinBox* m_surfaceMontageVerticalGapSpinBox;
        
        QDoubleSpinBox* m_volumeMontageHorizontalGapSpinBox;
        
        QDoubleSpinBox* m_volumeMontageVerticalGapSpinBox;
        
        QToolButton* m_surfaceMontageMatchPixelToolButton;
        
        QToolButton* m_volumeMontageMatchPixelToolButton;
        
        std::vector<QToolButton*> m_tabMarginMatchPixelToolButtons;
        
        QSignalMapper* m_tabIndexSignalMapper;
        
        QSignalMapper* m_tabMarginMatchPixelsToolButtonSignalMapper;
        
        int32_t m_tabIndexInTabMarginRow[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __GAPS_AND_MARGINS_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __GAPS_AND_MARGINS_DIALOG_DECLARE__

} // namespace
#endif  //__GAPS_AND_MARGINS_DIALOG_H__
