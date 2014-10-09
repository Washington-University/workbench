#ifndef __CARET_TEMPORARY_FILE_H__
#define __CARET_TEMPORARY_FILE_H__

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

#include "DataFile.h"

class QTemporaryFile;

namespace caret {

    class CaretTemporaryFile : public DataFile {
        
    public:
        CaretTemporaryFile();
        
        virtual ~CaretTemporaryFile();
        
        virtual void clear();
        
        virtual bool isEmpty() const;
        
        virtual AString getFileName() const;
                
        virtual AString getFileNameNoPath() const;
        
        virtual void setFileName(const AString& filename);
        
        virtual void readFile(const AString& filename);
        
        virtual void writeFile(const AString& filename);

        // ADD_NEW_METHODS_HERE

    private:
        CaretTemporaryFile(const CaretTemporaryFile&);
        
        CaretTemporaryFile& operator=(const CaretTemporaryFile&);
        
    private:

        void initializeCaretTemporaryFile();
        
        QTemporaryFile* m_temporaryFile;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CARET_TEMPORARY_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CARET_TEMPORARY_FILE_DECLARE__

} // namespace
#endif  //__CARET_TEMPORARY_FILE_H__
