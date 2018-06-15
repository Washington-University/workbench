#ifndef __CIFTI_BRAINORDINATE_LABEL_DYNAMIC_FILE_H__
#define __CIFTI_BRAINORDINATE_LABEL_DYNAMIC_FILE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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

#include "CiftiBrainordinateLabelFile.h"



namespace caret {
    class CiftiMappableDataFile;

    class CiftiBrainordinateLabelDynamicFile : public CiftiBrainordinateLabelFile {
        
    public:
        static CiftiBrainordinateLabelDynamicFile* newInstance(CiftiMappableDataFile* ciftiMapFile,
                                                               AString& errorMessageOut);
        
        virtual ~CiftiBrainordinateLabelDynamicFile();
        
        CiftiBrainordinateLabelDynamicFile(const CiftiBrainordinateLabelDynamicFile&) = delete;

        CiftiBrainordinateLabelDynamicFile& operator=(const CiftiBrainordinateLabelDynamicFile&) = delete;
        
        static bool isFileTypeSupported(const DataFileTypeEnum::Enum dataFileType);
        
        virtual bool supportsWriting() const;
        
        CaretMappableDataFile* getParentMappableDataFile();
        
        const CaretMappableDataFile* getParentMappableDataFile() const;
        

        // ADD_NEW_METHODS_HERE

    private:
        CiftiBrainordinateLabelDynamicFile(CaretMappableDataFile* parentMappableDataFile);
        
        CaretMappableDataFile* m_parentMappableDataFile;
        
        CaretPointer<SceneClassAssistant> m_sceneAssistant;
        
        static const int32_t LABEL_KEY_OFF = 0;
        
        static const int32_t LABEL_KEY_OUTLINE = 1;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CIFTI_BRAINORDINATE_LABEL_DYNAMIC_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CIFTI_BRAINORDINATE_LABEL_DYNAMIC_FILE_DECLARE__

} // namespace
#endif  //__CIFTI_BRAINORDINATE_LABEL_DYNAMIC_FILE_H__
