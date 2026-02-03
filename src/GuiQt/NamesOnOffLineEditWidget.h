#ifndef __NAMES_ON_OFF_LINE_EDIT_WIDGET_H__
#define __NAMES_ON_OFF_LINE_EDIT_WIDGET_H__

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



#include <memory>
#include <vector>
#include <QWidget>

#include "AString.h"

class QLineEdit;
class QToolButton;

namespace caret {

    class NamesOnOffLineEditWidget : public QWidget {
        
        Q_OBJECT

    public:
        NamesOnOffLineEditWidget(QWidget* parent = 0);
        
        virtual ~NamesOnOffLineEditWidget();
        
        NamesOnOffLineEditWidget(const NamesOnOffLineEditWidget&) = delete;

        NamesOnOffLineEditWidget& operator=(const NamesOnOffLineEditWidget&) = delete;
        
    signals:
        /* Issued when turn names on is selected */
        void namesOnTriggered(const std::vector<AString>& names);
        
        /* Issued when turn names off is selected */
        void namesOffTriggered(const std::vector<AString>& names);
        
    private slots:
        void onButtonClicked();
        
        void offButtonClicked();

        // ADD_NEW_METHODS_HERE

    private:
        std::vector<AString> getNames() const;
        
        QAction* m_onAction;
        
        QAction* m_offAction;
        
        QLineEdit* m_namesLineEdit;
        
        QLineEdit* m_separatorLineEdit;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __NAMES_ON_OFF_LINE_EDIT_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __NAMES_ON_OFF_LINE_EDIT_WIDGET_DECLARE__

} // namespace
#endif  //__NAMES_ON_OFF_LINE_EDIT_WIDGET_H__
