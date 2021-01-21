#ifndef __WU_Q_TOOL_TIP_HELPER_H__
#define __WU_Q_TOOL_TIP_HELPER_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2021 Washington University School of Medicine
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

#include <QObject>

class QAction;
class QMenu;

namespace caret {

    class WuQToolTipHelper : public QObject {
        
        Q_OBJECT

    public:
        static WuQToolTipHelper* newInstanceForMenu(QMenu* menu);
        
        virtual ~WuQToolTipHelper();
        
        WuQToolTipHelper(const WuQToolTipHelper&) = delete;

        WuQToolTipHelper& operator=(const WuQToolTipHelper&) = delete;
        

        // ADD_NEW_METHODS_HERE

    private slots:
        void menuActionHovered(QAction* action);
        
        void hideToolTip();
        
    protected:
        virtual bool event(QEvent* event) override;
        
    private:
        enum class Mode {
            MENU_TOOLTIPS
        };
        
        WuQToolTipHelper(const Mode mode,
                         QObject* parent);
        
        const Mode m_mode;
        
        QMenu* m_menu = NULL;
        
        bool m_useWhatsThisFlag = false;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WU_Q_TOOL_TIP_HELPER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_TOOL_TIP_HELPER_DECLARE__

} // namespace
#endif  //__WU_Q_TOOL_TIP_HELPER_H__
