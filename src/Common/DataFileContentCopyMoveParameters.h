#ifndef __DATA_FILE_CONTENT_COPY_MOVE_PARAMETERS_H__
#define __DATA_FILE_CONTENT_COPY_MOVE_PARAMETERS_H__

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



namespace caret {

    class DataFileContentCopyMoveInterface;
    
    class DataFileContentCopyMoveParameters : public CaretObject {
        
    public:
        DataFileContentCopyMoveParameters(const DataFileContentCopyMoveInterface* dataFileCopyMoveInterface,
                                          const int32_t windowIndex);
        
        virtual ~DataFileContentCopyMoveParameters();
        
        const DataFileContentCopyMoveInterface* getDataFileCopyMoveInterfaceToCopy() const;
        
        int32_t getWindowIndex() const;
        
        bool isOptionSelectedItems() const;
        
        void setOptionSelectedItems(const bool optionSelectedItems);

        // ADD_NEW_METHODS_HERE
        
    private:
        DataFileContentCopyMoveParameters(const DataFileContentCopyMoveParameters&);

        DataFileContentCopyMoveParameters& operator=(const DataFileContentCopyMoveParameters&);
        
        const DataFileContentCopyMoveInterface* m_dataFileCopyMoveInterface;
        
        const int32_t m_windowIndex;
        
        bool m_optionSelectedItems;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __DATA_FILE_CONTENT_COPY_MOVE_PARAMETERS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DATA_FILE_CONTENT_COPY_MOVE_PARAMETERS_DECLARE__

} // namespace
#endif  //__DATA_FILE_CONTENT_COPY_MOVE_PARAMETERS_H__
