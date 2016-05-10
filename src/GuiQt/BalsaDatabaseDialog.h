#ifndef __BALSA_DATABASE_DIALOG_H__
#define __BALSA_DATABASE_DIALOG_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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


class QLineEdit;

namespace caret {

    class BalsaDatabaseDialog : public WuQDialogNonModal, public EventListenerInterface {
        
        Q_OBJECT

    public:
        BalsaDatabaseDialog(QWidget* parent);
        
        virtual ~BalsaDatabaseDialog();
        
        void updateDialog();
        

        // ADD_NEW_METHODS_HERE

        virtual void receiveEvent(Event* event);

    private slots:
        void labelHtmlLinkClicked(const QString&);
        
    private:
        QWidget* createUsernamePasswordWidget();
        
        BalsaDatabaseDialog(const BalsaDatabaseDialog&);

        BalsaDatabaseDialog& operator=(const BalsaDatabaseDialog&);
        
        QLineEdit* m_usernameLineEdit;
        
        QLineEdit* m_passwordLineEdit;

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __BALSA_DATABASE_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BALSA_DATABASE_DIALOG_DECLARE__

} // namespace
#endif  //__BALSA_DATABASE_DIALOG_H__
