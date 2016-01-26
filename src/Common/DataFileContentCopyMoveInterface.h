#ifndef __DATA_FILE_CONTENT_COPY_MOVE_INTERFACE_H__
#define __DATA_FILE_CONTENT_COPY_MOVE_INTERFACE_H__

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


#include "CaretObject.h"


/**
 * \class caret::DataFileContentCopyMoveInterface
 * \brief Interface for data file that can copy their content to other files of same type
 * \ingroup Common
 */

namespace caret {

    class DataFile;
    
    class DataFileContentCopyMoveInterface : public CaretObject {
        
    public:
        DataFileContentCopyMoveInterface() { }
        
        virtual ~DataFileContentCopyMoveInterface() { }
        
        /**
         * @return Pointer to DataFile that implements this interface
         */
        virtual DataFile* getAsDataFile() = 0;

        /**
         * Append content from the given data file copy/move interface to this instance
         *
         * @param dataFileCopyMoveInterface
         *     From which content is copied.
         * @throws DataFileException
         *     If there is an error.
         */
        virtual void appendContentFromDataFile(const DataFileContentCopyMoveInterface* dataFileCopyMoveInterface) = 0;
        
        /**
         * @return A new instance of the same file type.  File is empty.
         */
        virtual DataFileContentCopyMoveInterface* newInstanceOfDataFile() const = 0;
        
    private:
        DataFileContentCopyMoveInterface(const DataFileContentCopyMoveInterface&);

        DataFileContentCopyMoveInterface& operator=(const DataFileContentCopyMoveInterface&);
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __DATA_FILE_CONTENT_COPY_MOVE_INTERFACE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DATA_FILE_CONTENT_COPY_MOVE_INTERFACE_DECLARE__

} // namespace
#endif  //__DATA_FILE_CONTENT_COPY_MOVE_INTERFACE_H__
