#ifndef __BRAIN_BROWSER_WINDOW_TOOL_BAR_VIEW_H__
#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_VIEW_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

#include "BrainBrowserWindowToolBarComponent.h"
#include "ModelTypeEnum.h"

class QAbstractButton;
class QRadioButton;

namespace caret {

    class BrainBrowserWindowToolBarView : public BrainBrowserWindowToolBarComponent {
        Q_OBJECT
        
    public:
        BrainBrowserWindowToolBarView(const QString& parentObjectName,
                                      BrainBrowserWindowToolBar* parentToolBar);
        
        virtual ~BrainBrowserWindowToolBarView();
        
        BrainBrowserWindowToolBarView(const BrainBrowserWindowToolBarView&) = delete;

        BrainBrowserWindowToolBarView& operator=(const BrainBrowserWindowToolBarView&) = delete;
        
        virtual void updateContent(BrowserTabContent* browserTabContent) override;
        
        // ADD_NEW_METHODS_HERE

    private slots:
        void viewModeRadioButtonClicked(QAbstractButton*);
    
    private:
        QRadioButton* viewModeSurfaceRadioButton;
        QRadioButton* viewModeSurfaceMontageRadioButton;
        QRadioButton* viewModeVolumeRadioButton;
        QRadioButton* viewModeWholeBrainRadioButton;
        QRadioButton* viewModeChartOneRadioButton;
        QRadioButton* viewModeChartTwoRadioButton;
        QRadioButton* viewModeMediaRadioButton;
        QRadioButton* viewModeHistologyRadioButton;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __BRAIN_BROWSER_WINDOW_TOOL_BAR_VIEW_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_BROWSER_WINDOW_TOOL_BAR_VIEW_DECLARE__

} // namespace
#endif  //__BRAIN_BROWSER_WINDOW_TOOL_BAR_VIEW_H__
