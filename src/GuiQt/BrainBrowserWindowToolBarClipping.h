#ifndef __BRAIN_BROWSER_WINDOW_TOOL_BAR_CLIPPING_H__
#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_CLIPPING_H__

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

#include <stdint.h>
#include <QCheckBox>

#include "BrainBrowserWindowToolBarComponent.h"



namespace caret {

    class BrainBrowserWindowToolBarClipping : public BrainBrowserWindowToolBarComponent {
    Q_OBJECT
        
    public:
        BrainBrowserWindowToolBarClipping(const int32_t browserWindowIndex,
                                          BrainBrowserWindowToolBar* parentToolBar,
                                          const QString& objectNamePrefix);
        
        virtual ~BrainBrowserWindowToolBarClipping();
        
        virtual void updateContent(BrowserTabContent* browserTabContent);
        
        // ADD_NEW_METHODS_HERE

    private slots:
        void clippingCheckBoxCheckStatusChanged();
    
        void setupClippingPushButtonClicked();
        
    private:
        BrainBrowserWindowToolBarClipping(const BrainBrowserWindowToolBarClipping&);

        BrainBrowserWindowToolBarClipping& operator=(const BrainBrowserWindowToolBarClipping&);
        
        const int32_t m_browserWindowIndex;
        
        BrainBrowserWindowToolBar* m_parentToolBar;
        
        QCheckBox* m_xClippingEnabledCheckBox;
        
        QCheckBox* m_yClippingEnabledCheckBox;
        
        QCheckBox* m_zClippingEnabledCheckBox;
        
        QCheckBox* m_surfaceClippingEnabledCheckBox;
    
        QCheckBox* m_volumeClippingEnabledCheckBox;
        
        QCheckBox* m_featuresClippingEnabledCheckBox;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __BRAIN_BROWSER_WINDOW_TOOL_BAR_CLIPPING_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_BROWSER_WINDOW_TOOL_BAR_CLIPPING_DECLARE__

} // namespace
#endif  //__BRAIN_BROWSER_WINDOW_TOOL_BAR_CLIPPING_H__
