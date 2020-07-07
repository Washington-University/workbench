#ifndef __DYN_CONN_VIEW_CONTROLLER_H__
#define __DYN_CONN_VIEW_CONTROLLER_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2020 Washington University School of Medicine
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

#include <QTreeView>
#include <QWidget>

#include "EventListenerInterface.h"


namespace caret {

    class DynConnViewController : public QWidget, public EventListenerInterface {
        
        Q_OBJECT

    public:
        DynConnViewController();
        
        virtual ~DynConnViewController();
        
        DynConnViewController(const DynConnViewController&) = delete;

        DynConnViewController& operator=(const DynConnViewController&) = delete;
        

        // ADD_NEW_METHODS_HERE

        virtual void receiveEvent(Event* event);

    private:
        
        QTreeView* m_treeView;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __DYN_CONN_VIEW_CONTROLLER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DYN_CONN_VIEW_CONTROLLER_DECLARE__

} // namespace
#endif  //__DYN_CONN_VIEW_CONTROLLER_H__
