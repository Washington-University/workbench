#ifndef __TAB_MARGINS_DIALOG_H__
#define __TAB_MARGINS_DIALOG_H__

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

class QLabel;
class QSignalMapper;
class QSpinBox;

namespace caret {

    class WuQGridLayoutGroup;
    
    class TabMarginsDialog : public WuQDialogNonModal, public EventListenerInterface {
        
        Q_OBJECT

    public:
        TabMarginsDialog(QWidget* parent = 0);
        
        virtual ~TabMarginsDialog();
        
        virtual void updateDialog();

        // ADD_NEW_METHODS_HERE

        virtual void receiveEvent(Event* event);

    private slots:
        void tabMarginChanged(int tabIndex);
        
    private:
        TabMarginsDialog(const TabMarginsDialog&);

        TabMarginsDialog& operator=(const TabMarginsDialog&);
        
        WuQGridLayoutGroup* m_gridLayoutGroup;
        
        std::vector<QLabel*> m_tabNumberLabels;
        
        std::vector<QSpinBox*> m_leftMarginSpinBoxes;
        
        std::vector<QSpinBox*> m_rightMarginSpinBoxes;
        
        std::vector<QSpinBox*> m_bottomMarginSpinBoxes;
        
        std::vector<QSpinBox*> m_topMarginSpinBoxes;
        
        QSignalMapper* m_tabIndexSignalMapper;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __TAB_MARGINS_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __TAB_MARGINS_DIALOG_DECLARE__

} // namespace
#endif  //__TAB_MARGINS_DIALOG_H__
