#ifndef __FOCUS__H_
#define __FOCUS__H_

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

#include "CaretObjectTracksModification.h"
#include "SurfaceProjectedItem.h"

namespace caret {

    class FociFileSaxReader;
    class GroupAndNameHierarchyItem;
    class StudyMetaDataLinkSet;
    
    class Focus : public CaretObjectTracksModification {
        
    public:
        Focus();
        
        virtual ~Focus();
        
        Focus(const Focus& obj);

        Focus& operator=(const Focus& obj);
        
        void clear();
        
        AString getClassName() const;
        
        void setClassName(const AString& name);
        
        AString getArea() const;
        
        void setArea(const AString& area);
        
        AString getComment() const;
        
        void setComment(const AString& area);
        
        float getExtent() const;
        
        void setExtent(const float extent);
        
        AString getGeography() const;
        
        void setGeography(const AString& geography);
        
        AString getName() const;
        
        void setName(const AString& name);
        
        AString getRegionOfInterest() const;
        
        void setRegionOfInterest(const AString& regionOfInterest);
        
        const float* getSearchXYZ() const;
        
        void setSearchXYZ(const float searchXYZ[3]);
        
        AString getStatistic() const;
        
        void setStatistic(const AString& statistic);
        
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
        
        AString getFocusID() const;
        
        void setFocusID(const AString& focusID);
        
        AString getSumsIdNumber() const;
        
        void setSumsIdNumber(const AString& sumsIdNumber);
        
        AString getSumsRepeatNumber() const;
        
        void setSumsRepeatNumber(const AString& sumsRepeatNumber);
        
        AString getSumsParentFocusBaseId() const;
        
        void setSumsParentFocusBaseId(const AString& sumsParentFocusBaseId);
        
        AString getSumsVersionNumber() const;
        
        void setSumsVersionNumber(const AString& sumsVersionNumber);
        
        AString getSumsMSLID() const;
        
        void setSumsMSLID(const AString& sumsMSLID);
        
        AString getSumsAttributeID() const;
        
        void setSumsAttributeID(const AString& attributeID);
        
        int32_t getNumberOfProjections() const;
        
        const SurfaceProjectedItem* getProjection(const int32_t indx) const;
        
        SurfaceProjectedItem* getProjection(const int32_t indx);
        
        void addProjection(SurfaceProjectedItem* projection);
        
        void removeExtraProjections();
        
        StudyMetaDataLinkSet* getStudyMetaDataLinkSet();
        
        const StudyMetaDataLinkSet* getStudyMetaDataLinkSet() const;
        
        void setGroupNameSelectionItem(GroupAndNameHierarchyItem* item);
        
        const GroupAndNameHierarchyItem* getGroupNameSelectionItem() const;
        
        void writeAsXML(XmlWriter& xmlWriter,
                        const int32_t focusIndex);
        
        bool setElementFromText(const AString& elementName,
                                const AString& textValue);
        
        virtual void clearModified();
        
        virtual bool isModified() const;
        
        static const AString XML_ATTRIBUTE_FOCUS_INDEX;
        static const AString XML_TAG_FOCUS;
        static const AString XML_TAG_AREA;
        static const AString XML_TAG_CLASS_NAME;
        static const AString XML_TAG_COMMENT;
        static const AString XML_TAG_EXTENT;
        static const AString XML_TAG_GEOGRAPHY;
        static const AString XML_TAG_NAME;
        static const AString XML_TAG_REGION_OF_INTEREST;
        static const AString XML_TAG_SEARCH_XYZ;
        static const AString XML_TAG_STATISTIC;
        static const AString XML_TAG_SUMS_ID_NUMBER;
        static const AString XML_TAG_SUMS_REPEAT_NUMBER;
        static const AString XML_TAG_SUMS_PARENT_FOCUS_BASE_ID;
        static const AString XML_TAG_SUMS_VERSION_NUMBER;
        static const AString XML_TAG_SUMS_MSLID;
        static const AString XML_TAG_SUMS_ATTRIBUTE_ID;
        static const AString XML_TAG_FOCUS_ID;
        
    private:
        void copyHelperFocus(const Focus& obj);

        void setNameOrClassModified();
        
        void removeAllProjections();
        
        AString m_area;
        
        AString m_className;

        AString m_comment;
        
        float m_extent;
        
        AString m_geography;
        
        AString m_name;
        
        AString m_regionOfInterest;
        
        float m_searchXYZ[3];
        
        AString m_statistic;
        
        StudyMetaDataLinkSet* m_studyMetaDataLinkSet;
        
        AString m_sumsIdNumber;
        
        AString m_sumsRepeatNumber;
        
        AString m_sumsParentFocusBaseId;
        
        AString m_sumsVersionNumber;
        
        AString m_sumsMSLID;
        
        AString m_attributeID;
        
        AString m_focusID;
        
        /** RGBA color components assigned to focus' name */
        float m_nameRgbaColor[4];
        
        /** RGBA color components assigned to focus' name validity */
        bool m_nameRgbaColorValid;
        
        /** RGBA color component assigned to focus' class name */
        float m_classRgbaColor[4];
        
        /** RGBA color components assigned to focus' name validity */
        bool m_classRgbaColorValid;
        
        /** May project to more than one surface */
        std::vector<SurfaceProjectedItem*> m_projections;
        
        /** Selection status of this border in the group/name hierarchy */
        GroupAndNameHierarchyItem* m_groupNameSelectionItem;
        
        /** Allow foci file SAX reader to remove all projections */
        friend class FociFileSaxReader;
        
    };
    
#ifdef __FOCUS_DECLARE__
    const AString Focus::XML_ATTRIBUTE_FOCUS_INDEX = "Index";
    const AString Focus::XML_TAG_FOCUS = "Focus";
    const AString Focus::XML_TAG_AREA = "Area";
    const AString Focus::XML_TAG_CLASS_NAME = "ClassName";
    const AString Focus::XML_TAG_COMMENT = "Comment";
    const AString Focus::XML_TAG_EXTENT = "Extent";
    const AString Focus::XML_TAG_GEOGRAPHY = "Geography";
    const AString Focus::XML_TAG_NAME = "Name";
    const AString Focus::XML_TAG_REGION_OF_INTEREST = "RegionOfInterest";
    const AString Focus::XML_TAG_SEARCH_XYZ = "SearchXYZ";
    const AString Focus::XML_TAG_STATISTIC = "Statistic";
    const AString Focus::XML_TAG_SUMS_ID_NUMBER = "SumsIDNumber";
    const AString Focus::XML_TAG_SUMS_REPEAT_NUMBER = "SumsRepeatNumber";
    const AString Focus::XML_TAG_SUMS_PARENT_FOCUS_BASE_ID = "SumsParentFocusBaseID";
    const AString Focus::XML_TAG_SUMS_VERSION_NUMBER = "SumsVersionNumber";
    const AString Focus::XML_TAG_SUMS_MSLID = "SumsMSLID";
    const AString Focus::XML_TAG_SUMS_ATTRIBUTE_ID = "AttributeID";
    const AString Focus::XML_TAG_FOCUS_ID = "FocusID";

#endif // __FOCUS_DECLARE__

} // namespace
#endif  //__FOCUS__H_
