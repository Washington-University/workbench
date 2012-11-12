#ifndef __BORDER__H_
#define __BORDER__H_

/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#include "BorderException.h"
#include "CaretColorEnum.h"
#include "CaretObjectTracksModification.h"
#include "StructureEnum.h"
#include "XmlException.h"

namespace caret {

    class GroupAndNameHierarchyItem;
    class SurfaceFile;
    class SurfaceProjectedItem;
    class XmlWriter;
    
    class Border : public CaretObjectTracksModification {
        
    public:
        Border();
        
        virtual ~Border();
        
        Border(const Border& obj);

        Border& operator=(const Border& obj);
        
        virtual AString toString() const;
        
        void clear();
        
        AString getName() const;
        
        void setName(const AString& name);
        
        AString getClassName() const;
        
        void setClassName(const AString& name);
        
        CaretColorEnum::Enum getColor() const;
        
        void setColor(const CaretColorEnum::Enum color);
        
        StructureEnum::Enum getStructure();
        
        bool verifyAllPointsOnSameStructure() const;
        
        int32_t getNumberOfPoints() const;
        
        const SurfaceProjectedItem* getPoint(const int32_t indx) const;
        
        SurfaceProjectedItem* getPoint(const int32_t indx);
        
        int32_t findPointIndexNearestXYZ(const SurfaceFile* surfaceFile,
                                        const float xyz[3],
                                        const float maximumDistance,
                                        float& distanceToNearestPointOut) const;
        
        void addPoint(SurfaceProjectedItem* point);
        
        void addPoints(const Border* border,
                       const int32_t startPointIndex = -1,
                       const int32_t pointCount = -1);
        
        void addPointsToCloseBorderWithGeodesic(const SurfaceFile* surfaceFile);
        
        void removeAllPoints();
        
        void removePoint(const int32_t indx);
        
        void removeLastPoint();
        
        void replacePoints(const Border* border);
        
        void reverse();
        
        void reviseExtendFromEnd(SurfaceFile* surfaceFile,
                                 const Border* segment) throw (BorderException);
        
        void reviseEraseFromEnd(SurfaceFile* surfaceFile,
                                const Border* segment) throw (BorderException);
        
        void reviseReplaceSegment(SurfaceFile* surfaceFile,
                                  const Border* segment) throw (BorderException);
        
        void writeAsXML(XmlWriter& xmlWriter) throw (XmlException);
        
        void setGroupNameSelectionItem(GroupAndNameHierarchyItem* item);
        
        const GroupAndNameHierarchyItem* getGroupNameSelectionItem() const;
        
        bool isClassRgbaValid() const;
        
        void setClassRgbaInvalid();
        
        const float* getClassRgba() const;
        
        void getClassRgba(float rgba[4]) const;
        
        void setClassRgba(const float rgba[4]);
        
        static const AString XML_TAG_BORDER;
        static const AString XML_TAG_NAME;
        static const AString XML_TAG_CLASS_NAME;
        static const AString XML_TAG_COLOR_NAME;
        
    private:
        void copyHelperBorder(const Border& obj);
        
        void setNameOrClassModified();
        
        AString m_name;
        
        AString m_className;
        
        CaretColorEnum::Enum m_color;
        
        std::vector<SurfaceProjectedItem*> m_points;
        
        /** RGBA color component assigned to border's class name */
        float m_classRgbaColor[4];
        
        /** RGBA color components assigned to border's class name validity */
        bool m_classRgbaColorValid;
        
        /** Selection status of this border in the group/name hierarchy */
        GroupAndNameHierarchyItem* m_groupNameSelectionItem;        
    };
    
#ifdef __BORDER_DECLARE__
    const AString Border::XML_TAG_BORDER = "Border";
    const AString Border::XML_TAG_CLASS_NAME   = "ClassName";
    const AString Border::XML_TAG_COLOR_NAME   = "ColorName";
    const AString Border::XML_TAG_NAME   = "Name";
#endif // __BORDER_DECLARE__

} // namespace
#endif  //__BORDER__H_
