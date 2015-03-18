#ifndef __GIFTILABEL_H__
#define __GIFTILABEL_H__

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


#include "CaretObject.h"
#include "TracksModificationInterface.h"

#include <limits>
#include <stdint.h>

#include <AString.h>

namespace caret {
    
    class GroupAndNameHierarchyItem;
    
    /**
     * Represents a GIFTI Label.
     */
    class GiftiLabel : public CaretObject, TracksModificationInterface {
        
    public:
        GiftiLabel(
                   const int32_t key,
                   const AString& name);
        
        explicit GiftiLabel(
                            const int32_t key,
                            const AString& name,
                            const float red,
                            const float green,
                            const float blue,
                            const float alpha);
        
        explicit GiftiLabel(
                            const int32_t key,
                            const AString& name,
                            const float red,
                            const float green,
                            const float blue,
                            const float alpha,
                            const float x,
                            const float y,
                            const float z);
        
        explicit GiftiLabel(
                            const int32_t key,
                            const AString& name,
                            const double red,
                            const double green,
                            const double blue,
                            const double alpha);
        
        GiftiLabel(
                   const int32_t key,
                   const AString& name,
                   const float rgba[]);
        
        explicit GiftiLabel(
                            const int32_t key,
                            const AString& name,
                            const int32_t red,
                            const int32_t green,
                            const int32_t blue,
                            const int32_t alpha);
        
        GiftiLabel(
                   const int32_t key,
                   const AString& name,
                   const int32_t rgba[]);
        
        GiftiLabel(const int32_t key);
        
        GiftiLabel(const GiftiLabel& gl);
        
    public:
        GiftiLabel& operator=(const GiftiLabel& gl);
        
        virtual ~GiftiLabel();
        
    private:
        static float colorClamp(const float& in);
        
        void copyHelper(const GiftiLabel& o);
        
        void initializeMembersGiftiLabel();
        
    public:
        int32_t hashCode();
        
        bool equals(const GiftiLabel&);
        
        int32_t operator<(const GiftiLabel& gl);
        
        int32_t getKey() const;
        
        void setKey(const int32_t key);
        
        AString getName() const;
        
        /**
         * @return True if the name of the label is detected
         * to be a "medial wall" name.
         */
        inline bool isMedialWallName() const { return this->medialWallNameFlag; }
        
        void setName(const AString& name);
        
        AString getNameAndKeyForLabelEditor() const;
        
        bool isSelected() const;
        
        void setSelected(const bool selected);
        
        void getColor(float rgbaOut[4]) const;
        
        void setColor(const float rgba[4]);
        
        static void getDefaultColor(float rgbaOut[4]);
        
        float getRed() const;
        
        float getGreen() const;
        
        float getBlue() const;
        
        float getAlpha() const;
        
        float getX() const;
        
        float getY() const;
        
        float getZ() const;
        
        void getXYZ(float xyz[3]) const;
        
        void setX(const float x);
        
        void setY(const float y);
        
        void setZ(const float z);
        
        void setXYZ(const float xyz[3]);
        
        void setModified();
        
        void clearModified();
        
        bool isModified() const;
        
        AString toString() const;
        
        int32_t getCount() const;
        
        void setCount(const int32_t count);
        
        void incrementCount();
        
        bool matches(const GiftiLabel& rhs, const bool checkColor = false, const bool checkCoord = false) const;
        
        void setGroupNameSelectionItem(GroupAndNameHierarchyItem* item);
        
        const GroupAndNameHierarchyItem* getGroupNameSelectionItem() const;
        
        GroupAndNameHierarchyItem* getGroupNameSelectionItem();
        
        /**
         * @return The invalid label key.
         */
        static inline int32_t getInvalidLabelKey() { return s_invalidLabelKey; }
        
    private:
        void setNamePrivate(const AString& name);
        
        /**tracks modification status (DO NOT CLONE) */
        bool modifiedFlag;
        
        /**
         * Name of label
         * DO NOT set directly with assignment operation.
         * Use setName() or setNamePrivate() so that the 
         * medial wall name flag is updated.
         */
        AString name;
        
        int32_t key;
        
        bool selected;
        
        bool medialWallNameFlag;
        
        float red;
        
        float green;
        
        float blue;
        
        float alpha;
        
        float x;
        
        float y;
        
        float z;
        
        /**Used to count nodes/voxel using label (not saved in file) */
        int32_t count;
        
        /** Selection status of this label in the map/label hierarchy */
        mutable GroupAndNameHierarchyItem* m_groupNameSelectionItem;
        
        /** The invalid label key */
        const static int32_t s_invalidLabelKey;
    };
    
#ifdef __GIFTI_LABEL_DECLARE__
    const int32_t GiftiLabel::s_invalidLabelKey =  std::numeric_limits<int32_t>::min(); 
    //const int32_t GiftiLabel::s_invalidLabelKey = -2147483648;
#endif // __GIFTI_LABEL_DECLARE__
} // namespace

#endif // __GIFTILABEL_H__
