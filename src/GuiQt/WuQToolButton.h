#ifndef __WU_Q_TOOL_BUTTON_H__
#define __WU_Q_TOOL_BUTTON_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2026 Washington University School of Medicine
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



//#include <memory>

#include <QMenu> /* without this get a compilation error in QToolButton */
#include <QToolButton>



namespace caret {

    class WuQToolButton : public QToolButton {
        
        Q_OBJECT

    public:
        WuQToolButton(QWidget* parent = 0);
        
        virtual ~WuQToolButton();
        
        WuQToolButton(const WuQToolButton&) = delete;

        WuQToolButton& operator=(const WuQToolButton&) = delete;

        void setLightIcon(const QIcon& icon);

        // ADD_NEW_METHODS_HERE

    protected:
        virtual void changeEvent(QEvent *event) override;
        
        virtual void showEvent(QShowEvent *event) override;
        
    private:
        void setStyleOnMacOS();
        
        void setStyleOnMacOSDark();
        
        void setStyleOnMacOSLight();
        
        QIcon m_lightIcon;
        
        QIcon m_darkIcon;
        
        bool m_iconsCreatedFlag = false;
        
        bool m_blockChangeEventFlag = false;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WU_Q_TOOL_BUTTON_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_TOOL_BUTTON_DECLARE__

} // namespace
#endif  //__WU_Q_TOOL_BUTTON_H__
