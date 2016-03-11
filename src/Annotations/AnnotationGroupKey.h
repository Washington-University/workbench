#ifndef __ANNOTATION_GROUP_KEY_H__
#define __ANNOTATION_GROUP_KEY_H__

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


#include "AnnotationGroupTypeEnum.h"
#include "CaretObject.h"

namespace caret {

    class AnnotationFile;
    
    class AnnotationGroupKey : public CaretObject {
        
    public:
        AnnotationGroupKey();
        
        virtual ~AnnotationGroupKey();
        
        AnnotationGroupKey(const AnnotationGroupKey& obj);

        AnnotationGroupKey& operator=(const AnnotationGroupKey& obj);
        
        bool operator==(const AnnotationGroupKey& groupKey) const;
        
        bool operator<(const AnnotationGroupKey& groupKey) const;
        
        void reset();
        
        AnnotationFile* getAnnotationFile() const;
        
        AnnotationGroupTypeEnum::Enum getGroupType() const;
        
        int32_t getSpaceGroupUniqueKey() const;
        
        int32_t getUserGroupUniqueKey() const;
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void copyHelperAnnotationGroupKey(const AnnotationGroupKey& obj);

        /* set method private so that only friend classes can access them */
        
        void setAnnotationFile(AnnotationFile* annotationFile);
        
        void setGroupType(const AnnotationGroupTypeEnum::Enum groupType);
        
        void setSpaceGroupUniqueKey(const int32_t spaceGroupUniqueKey);
        
        void setUserGroupUniqueKey(const int32_t userGroupUniqueKey);
        
        AnnotationFile* m_annotationFile;
        
        AnnotationGroupTypeEnum::Enum m_groupType;
        
        int32_t m_spaceGroupUniqueKey;
        
        int32_t m_userGroupUniqueKey;
        
        friend class AnnotationFile;
        
        friend class AnnotationGroup;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_GROUP_KEY_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_GROUP_KEY_DECLARE__

} // namespace
#endif  //__ANNOTATION_GROUP_KEY_H__
