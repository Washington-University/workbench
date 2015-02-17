#ifndef __BORDER__H_
#define __BORDER__H_

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

#include "BorderException.h"
#include "CaretObjectTracksModification.h"
#include "StructureEnum.h"
#include "XmlException.h"

class QXmlStreamReader;
class QXmlStreamWriter;

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
        
        static Border* newInstanceFromSurfaceNodes(const AString& borderName,
                                                   const SurfaceFile* surfaceFile,
                                                   std::vector<int32_t>& nodeIndices);
        
        virtual AString toString() const;
        
        void clear();
        
        AString getName() const;
        
        void setName(const AString& name);
        
        AString getClassName() const;
        
        void setClassName(const AString& name);
        
        StructureEnum::Enum getStructure() const;
        
        void setStructure(const StructureEnum::Enum& structure);
        
        bool verifyAllPointsOnSameStructure() const;
        
        bool verifyForSurfaceNumberOfNodes(const int32_t& numNodes) const;
        
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
        
        bool isClosed() const;
        
        void setClosed(const bool& closed);
        
        void addPointsToCloseBorderWithGeodesic(const SurfaceFile* surfaceFile);
        
        void removeAllPoints();
        
        void removePoint(const int32_t indx);
        
        void removeFirstPoint();
        
        void removeLastPoint();
        
        void replacePointsWithUndoSaving(const Border* border);

        void reverse();
        
        void reviseExtendFromPointIndex(SurfaceFile* surfaceFile,
                                        const int32_t pointIndex,
                                        const Border* segment);
        
        void reviseExtendFromEnd(SurfaceFile* surfaceFile,
                                 const Border* segment);
        
        void reviseEraseFromEnd(SurfaceFile* surfaceFile,
                                const Border* segment);
        
        void reviseReplaceSegment(SurfaceFile* surfaceFile,
                                  const Border* segment);
        
        void writeAsXML(XmlWriter& xmlWriter);
        
        void writeXML3(QXmlStreamWriter& xml) const;
        
        void readXML1(QXmlStreamReader& xml);
        
        void readXML3(QXmlStreamReader& xml);
        
        void setGroupNameSelectionItem(GroupAndNameHierarchyItem* item);
        
        const GroupAndNameHierarchyItem* getGroupNameSelectionItem() const;
        
        bool isClassRgbaValid() const;
        
        void setClassRgbaInvalid();
        
        const float* getClassRgba() const;
        
        void getClassRgba(float rgba[4]) const;
        
        void setClassRgba(const float rgba[4]);
        
        bool isNameRgbaValid() const;
        
        void setNameRgbaInvalid();
        
        const float* getNameRgba() const;
        
        void getNameRgba(float rgba[4]) const;
        
        void setNameRgba(const float rgba[4]);
        
        bool isUndoBorderValid() const;
        
        void undoLastBorderEditing();
        
        static const AString XML_TAG_BORDER;
        static const AString XML_TAG_NAME;
        static const AString XML_TAG_CLASS_NAME;
        
    private:
        void copyHelperBorder(const Border& obj);
        
        void setNameOrClassModified();
        
        float getSegmentLength(SurfaceFile* surfaceFile,
                                 const int32_t startPointIndex,
                                 const int32_t endPointIndex);
        
        void replacePoints(const Border* border);
        
        void saveBorderForUndoEditing();
        
        AString m_name;
        
        AString m_className;
        
        std::vector<SurfaceProjectedItem*> m_points;
        
        bool m_closed;
        
        /** RGBA color component assigned to border's class name */
        float m_classRgbaColor[4];
        
        /** RGBA color components assigned to border's class name validity */
        bool m_classRgbaColorValid;
        
        /** RGBA color components assigned to focus' name */
        float m_nameRgbaColor[4];
        
        /** RGBA color components assigned to focus' name validity */
        bool m_nameRgbaColorValid;
        
        /** Selection status of this border in the group/name hierarchy */
        GroupAndNameHierarchyItem* m_groupNameSelectionItem;
        
        /**
         * When "this" border is edited, a copy of "this" is placed into
         * this member so that the border can be restored if the editing
         * was not satisfactory.
         */
        Border* m_copyOfBorderPriorToLastEditing;
    };
    
#ifdef __BORDER_DECLARE__
    const AString Border::XML_TAG_BORDER = "Border";
    const AString Border::XML_TAG_CLASS_NAME   = "ClassName";
    const AString Border::XML_TAG_NAME   = "Name";
#endif // __BORDER_DECLARE__

} // namespace
#endif  //__BORDER__H_
