#ifndef __FILE_PATH_NAME_PREFIX_COMPACTOR_H__
#define __FILE_PATH_NAME_PREFIX_COMPACTOR_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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
    class CaretDataFile;
    class CaretMappableDataFile;

    class FilePathNamePrefixCompactor : public CaretObject {
        
    public:

        static void removeMatchingPathPrefixFromCaretDataFiles(const std::vector<CaretMappableDataFile*>& caretMappableDataFiles,
                                                               std::vector<AString>& prefixRemovedNamesOut);
        
        static void removeMatchingPathPrefixFromCaretDataFiles(const std::vector<CaretDataFile*>& caretDataFiles,
                                                               std::vector<AString>& prefixRemovedNamesOut);
        
        static void removeMatchingPathPrefixFromCaretDataFile(const CaretDataFile* caretDataFile,
                                                              AString& prefixRemovedNameOut);
        
    private:
        FilePathNamePrefixCompactor();
        
        virtual ~FilePathNamePrefixCompactor();
        
        FilePathNamePrefixCompactor(const FilePathNamePrefixCompactor&);

        FilePathNamePrefixCompactor& operator=(const FilePathNamePrefixCompactor&);
        
        static void removeMatchingPathPrefixFromFileNames(const std::vector<AString>& fileNames,
                                                          std::vector<AString>& prefixRemovedNamesOut);
        
    };
    
#ifdef __FILE_PATH_NAME_PREFIX_COMPACTOR_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __FILE_PATH_NAME_PREFIX_COMPACTOR_DECLARE__

} // namespace
#endif  //__FILE_PATH_NAME_PREFIX_COMPACTOR_H__
