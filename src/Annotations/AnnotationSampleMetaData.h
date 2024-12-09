#ifndef __ANNOTATION_SAMPLE_META_DATA_H__
#define __ANNOTATION_SAMPLE_META_DATA_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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

#include "CaretObject.h"



namespace caret {

    class AnnotationSampleMetaData : public CaretObject {
        
    public:
        AnnotationSampleMetaData();
        
        virtual ~AnnotationSampleMetaData();
        
        static std::vector<AString> getAllValidAllenTissueTypeValues();
        
        static std::vector<AString> getAllValidHemisphereValues();
        
        static std::vector<AString> getAllValidLocationValues();
        
        static std::vector<AString> getAllValidSampleTypeValues();

        static std::vector<AString> getAllValidSlabFaceValues();
        
        AnnotationSampleMetaData(const AnnotationSampleMetaData& obj);

        AnnotationSampleMetaData& operator=(const AnnotationSampleMetaData& obj);
        

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void copyHelperAnnotationSampleMetaData(const AnnotationSampleMetaData& obj);

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_SAMPLE_META_DATA_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_SAMPLE_META_DATA_DECLARE__

} // namespace
#endif  //__ANNOTATION_SAMPLE_META_DATA_H__
