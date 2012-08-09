#ifndef __FOCUS__H_
#define __FOCUS__H_

/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/


#include "CaretObjectTracksModification.h"
#include "SurfaceProjectedItem.h"

namespace caret {

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
        
        void removeAllProjections();
        
        StudyMetaDataLinkSet* getStudyMetaDataLinkSet();
        
        const StudyMetaDataLinkSet* getStudyMetaDataLinkSet() const;
        
        void writeAsXML(XmlWriter& xmlWriter,
                        const int32_t focusIndex) throw (XmlException);
        
        bool isSelectionClassOrNameModified() const;
        
        void setSelectionClassAndNameKeys(const int32_t selectionClassKey,
                                          const int32_t selectionNameKey);
        
        int32_t getSelectionNameKey() const;
        
        int32_t getSelectionClassKey() const;
        
        bool setElementFromText(const AString& elementName,
                                const AString& textValue);
        
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
        
    private:
        void copyHelperFocus(const Focus& obj);

        void setNameOrClassModified();
        
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
        
        /** Used for determining display status: not saved to file and does not affect modification status */
        int32_t m_selectionNameKey;
        
        /** Used for determining display status: not saved to file and does not affect modification status */
        int32_t m_selectionClassKey;
        
        /** 
         * Name/Class modification status, not saved to file. 
         * COMPLETELY separate from the modification status
         * that tracks all modifications to a border.
         */
        bool m_selectionClassNameModificationStatus;
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

#endif // __FOCUS_DECLARE__

} // namespace
#endif  //__FOCUS__H_
