#ifndef __MAC_DUPLICATE_MENU_BAR_H__
#define __MAC_DUPLICATE_MENU_BAR_H__

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

#include <QWidget>

class QMainWindow;
class QMenu;
namespace caret {

    class MacDuplicateMenuBar : public QWidget {
        
        Q_OBJECT

    public:
        MacDuplicateMenuBar(QMainWindow* mainWindow,
                            QWidget* parent = 0);
        
        virtual ~MacDuplicateMenuBar();
        
        MacDuplicateMenuBar(const MacDuplicateMenuBar&) = delete;

        MacDuplicateMenuBar& operator=(const MacDuplicateMenuBar&) = delete;
        

        // ADD_NEW_METHODS_HERE

    private:
        QMenu* duplicateMenu(QMenu* menu);
        
        QString m_indentText;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __MAC_DUPLICATE_MENU_BAR_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __MAC_DUPLICATE_MENU_BAR_DECLARE__

} // namespace
#endif  //__MAC_DUPLICATE_MENU_BAR_H__
