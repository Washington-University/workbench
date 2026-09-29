#ifndef __QT_FILE_HELPER_H__
#define __QT_FILE_HELPER_H__

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



#include <QIODevice>

#include "CaretObject.h"



namespace caret {

    class QtFileHelper : public CaretObject {
        
    public:
        static qint64 safeSkip(QIODevice *device, qint64 maxSize);
        
        virtual ~QtFileHelper();
        
        QtFileHelper(const QtFileHelper&) = delete;

        QtFileHelper& operator=(const QtFileHelper&) = delete;
        
    private:
        QtFileHelper();
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __QT_FILE_HELPER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __QT_FILE_HELPER_DECLARE__

} // namespace
#endif  //__QT_FILE_HELPER_H__
