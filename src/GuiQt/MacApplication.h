#ifndef __MAC_APPLICATION_H__
#define __MAC_APPLICATION_H__

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

/*
 * Based upon: http://www.qtcentre.org/wiki/index.php?title=Opening_documents_in_the_Mac_OS_X_Finder&printable=yes&useskin=vector
 */

#include <QApplication>

namespace caret {

    class MacApplication : public QApplication {
        
        Q_OBJECT

    public:
        MacApplication(int& argc, char** argv);
        
        virtual ~MacApplication();
        
    private:
        MacApplication(const MacApplication&);

        MacApplication& operator=(const MacApplication&);
        
    public:

        // ADD_NEW_METHODS_HERE

    protected:
        virtual bool event(QEvent *event);
        
    private:

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __MAC_APPLICATION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __MAC_APPLICATION_DECLARE__

} // namespace
#endif  //__MAC_APPLICATION_H__
