#ifndef __FEATURE_KEY_H__
#define __FEATURE_KEY_H__

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



#include <memory>

#include "CaretObject.h"



namespace caret {

    class FeatureKey : public CaretObject {
        
    public:
        static FeatureKey fromSceneKeyString(const AString& sceneKeyString);
        
        FeatureKey(const uint64_t groupID,
                   const uint64_t uniqueID);
        
        FeatureKey();
        
        virtual ~FeatureKey();
        
        FeatureKey(const FeatureKey& obj);

        FeatureKey& operator=(const FeatureKey& obj);
        
        bool operator==(const FeatureKey& obj) const;

        bool isValid() const;

        uint64_t getGroupID() const;
        
        uint64_t getUniqueID() const;
        
        void setUniqueID(const uint64_t uniqueID);
        
        AString toSceneKeyString() const;
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void copyHelperFeatureKey(const FeatureKey& obj);

        uint64_t m_groupID;
        
        uint64_t m_uniqueID;
        
        bool m_validFlag = false;

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __FEATURE_KEY_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __FEATURE_KEY_DECLARE__

} // namespace
#endif  //__FEATURE_KEY_H__
