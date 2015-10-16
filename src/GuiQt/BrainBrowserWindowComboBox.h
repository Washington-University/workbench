#ifndef __BRAIN_BROWSER_WINDOW_COMBO_BOX_H__
#define __BRAIN_BROWSER_WINDOW_COMBO_BOX_H__

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

#include <stdint.h>

#include "WuQWidget.h"

class QComboBox;

namespace caret {

    class BrainBrowserWindow;
    
    class BrainBrowserWindowComboBox : public WuQWidget {
        
        Q_OBJECT

    public:
        /**
         * Style of combo box content
         */
        enum Style {
            /** Name and number: "Window 3" */
            STYLE_NAME_AND_NUMBER,
            /** Number only:  "3" */
            STYLE_NUMBER
        };
        
        BrainBrowserWindowComboBox(const Style style,
                                   QObject* parent);
        
        virtual ~BrainBrowserWindowComboBox();
        
        void updateComboBox();
        
        virtual QWidget* getWidget();
        
        void setBrowserWindowByIndex(const int32_t browserWindowIndex);
        
        void setBrowserWindow(BrainBrowserWindow* browserWindow);
        
        int32_t getSelectedBrowserWindowIndex() const;
        
        BrainBrowserWindow* getSelectedBrowserWindow() const;
        
    signals:
        void browserWindowIndexSelected(const int32_t browserWindowIndex);
        
        void browserWindowSelected(BrainBrowserWindow* browserWindow);
        
    private slots:
        void comboBoxIndexSelected(int indx);
        
    private:
        BrainBrowserWindowComboBox(const BrainBrowserWindowComboBox&);

        BrainBrowserWindowComboBox& operator=(const BrainBrowserWindowComboBox&);
        
    public:

        // ADD_NEW_METHODS_HERE

    private:
        const Style m_style;
        
        QComboBox* m_comboBox;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __BRAIN_BROWSER_WINDOW_COMBO_BOX_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_BROWSER_WINDOW_COMBO_BOX_DECLARE__

} // namespace
#endif  //__BRAIN_BROWSER_WINDOW_COMBO_BOX_H__
