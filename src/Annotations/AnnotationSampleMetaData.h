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


#include <cstdint>
#include <memory>

#include "CaretObject.h"



namespace caret {
    class GiftiMetaData;

    class AnnotationSampleMetaData : public CaretObject {
        
    public:
        static int32_t getFirstLeftHemisphereAllenSlabNumber();
        
        static int32_t getFirstRightHemisphereAllenSlabNumber();
        
        static int32_t getLastRightHemisphereAllenSlabNumber();

        static void setFirstLeftHemisphereAllenSlabNumber(const int32_t number);
        
        static void setFirstRightHemisphereAllenSlabNumber(const int32_t number);
        
        static void setLastRightHemisphereAllenSlabNumber(const int32_t number);
        
        AnnotationSampleMetaData(GiftiMetaData* metadata);
        
        void updateMetaData(const GiftiMetaData* metadata) const;
        
        virtual ~AnnotationSampleMetaData();
        
        bool validateMetaData(AString& errorMessageOut) const;
        
        static std::vector<AString> getAllValidAllenTissueTypeValues();
        
        static std::vector<AString> getAllValidHemisphereValues();
        
        static std::vector<AString> getAllValidSampleTypeValues();

        static std::vector<AString> getAllValidSlabFaceValues();
        
        static AString getDateFormat();
        
        static AString getSubjectNameLabelText() { return "Subject Name"; }
        
        static AString getAllenLocalNameLabelText() { return "Allen Local Name"; }
        
        static AString getHemisphereLabelText() { return "Hemisphere"; }
        
        static AString getAllenTissueTypeLabelText() { return "Allen Tissue Type"; }
        
        static AString getAllenSlabNumberLabelText() { return "Allen Slab Number"; }
        
        static AString getLocalSlabIdLabelText() { return "Local Slab ID"; }
        
        static AString getSlabFaceLabelText() { return "Slab Face"; }
        
        static AString getSampleTypeLabelText() { return "Sample Type"; }
        
        static AString getLocationLabelText() { return "Location"; }
        
        static AString getDesiredSampleEditDateLabelText() { return "Desired Sample Edit Date"; }
        
        static AString getActualSampleEditDateLabelText() { return "Actual Sample Edit Date"; }
        
        static AString getHmbaParcelDingAbbreviationLabelText() { return "Hmba Parcel Ding Abbreviation"; }
        
        static AString getHmbaParcelDingFullNameLabelText() { return "Hmba Parcel Ding Full Name"; }
        
        static AString getSampleNameLabelText() { return "Sample Name"; }
        
        static AString getAlternativeSampleNameLabelText() { return "Alternative Sample Name"; }
        
        static AString getSampleNumberLabelText() { return "Sample Number"; }
        
        static AString getLocalSampleIdLabelText() { return "Local Sample ID"; }
        
        static AString getPrimaryParcellationLabelText() { return "Primary Parcellation"; }
        
        static AString getAlternativeParcellationLabelText() { return "Alternative Parcellation"; }
        
        static AString getCommentLabelText() { return "Comment"; }
        
        static AString getBorderFileNameLabelText() { return "Border File"; }
        
        static AString getBorderIdLabelText() { return "Border ID"; }
        
        static AString getBorderNameLabelText() { return "Border Name"; }
        
        static AString getBorderClassLabelText() { return "Border Class"; }
        
        static AString getFocusFileNameLabelText() { return "Focus File"; }
        
        static AString getFocusIdLabelText() { return "Focus ID"; }
        
        static AString getFocusNameLabelText() { return "Focus Name"; }
        
        static AString getFocusClassLabelText() { return "Focus Class"; }
        
        static AString getBicanDonorIdLabelText() { return "BICAN Donor ID"; }
        
        static AString getNhashSlabIdLabelText() { return "NHash Slab ID"; }
        
        AnnotationSampleMetaData(const AnnotationSampleMetaData& obj) = delete;

        AnnotationSampleMetaData& operator=(const AnnotationSampleMetaData& obj) = delete;

        void copyMetaData(const AnnotationSampleMetaData& obj);
        
        void copyMetaDataForNewAnnotation(const AnnotationSampleMetaData& obj);
        
        AString getActualSampleEditDate() const;
        
        void setActualSampleEditDate(const AString& value);
        
        AString getAllenLocalName() const;
        
        void setAllenLocalName(const AString& value);
        
        AString getAllenSlabNumber() const;
        
        void setAllenSlabNumber(const AString& value);
        
        AString getAllenTissueType() const;
        
        void setAllenTissueType(const AString& value);
        
        AString getAlternateParcellation() const;
        
        void setAlternateParcellation(const AString& value);
        
        AString getAlternateSampleName() const;
        
        void setAlternateSampleName(const AString& value);
        
        AString getBicanDonorID() const;
        
        void setBicanDonorID(const AString& value);
        
        AString getComment() const;
        
        void setComment(const AString& value);
        
        AString getDesiredSampleEditDate() const;
        
        void setDesiredSampleEditDate(const AString& value);
        
        AString getHemisphere() const;
        
        void setHemisphere(const AString& value);
        
        AString getHmbaParcelDingAbbreviation() const;
        
        void setHmbaParcelDingAbbreviation(const AString& value);
        
        AString getHmbaParcelDingFullName() const;
        
        void setHmbaParcelDingFullName(const AString& value);
        
        AString getLocalSampleID() const;
                
        AString getLocalSlabID() const;
        
        AString getNHashSlabID() const;
        
        void setNHashSlabID(const AString& value);
        
        AString getPrimaryParcellation() const;
        
        void setPrimaryParcellation(const AString& value);
                
        AString getSampleName() const;
        
        void setSampleName(const AString& value);
        
        AString getSampleNumber() const;
        
        void setSampleNumber(const AString& value);
        
        AString getSampleType() const;
        
        void setSampleType(const AString& value);
        
        AString getSlabFace() const;
        
        void setSlabFace(const AString& value);
        
        AString getSubjectName() const;
        
        void setSubjectName(const AString& value);

        AString getBorderFileName() const;
        
        void setBorderFileName(const AString& value);

        AString getBorderClass() const;
        
        void setBorderClass(const AString& value);
        
        AString getBorderID() const;
        
        void setBorderID(const AString& value);

        AString getBorderName() const;
        
        void setBorderName(const AString& value);
        
        AString getFocusFileName() const;
        
        void setFocusFileName(const AString& value);
        
        AString getFocusClass() const;
        
        void setFocusClass(const AString& value);
        
        AString getFocusID() const;
        
        void setFocusID(const AString& value);
                
        AString getFocusName() const;
        
        void setFocusName(const AString& value);
        
        AString toFormattedHtml() const;
        
        void updateMetaDataWithNameChanges();
        
        void getAllMetaDataNamesAndValues(std::vector<std::pair<AString, AString>>& namesAndValuesOut) const;
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:

        AString assembleCompositeElementComponents(const std::vector<AString>& components,
                                                   const AString& separator) const;
        
        AString get(const AString& currentMetaDataName,
                    const AString& previousMetaDataNameOne = "",
                    const AString& previousMetaDataNameTwo = "") const;
        
        void set(const AString& currentMetaDataName,
                 const AString& value);
        
        int32_t getInt(const AString& currentMetaDataName,
                       const AString& previousMetaDataNameOne = "",
                       const AString& previousMetaDataNameTwo = "") const;
        
        void setInt(const AString& currentMetaDataName,
                    const int32_t value);
        
        /** An instance of AnnotationSampleMetaData wraps an instance of GiftiMetaData - DO NOT DELETE */
        mutable GiftiMetaData* m_metadata;
        
        bool m_metaDataHasBeenUpdatedFlag = false;
        
        // ADD_NEW_MEMBERS_HERE

        static const AString SAMPLES_ACTUAL_SAMPLE_EDIT_DATE;
        
        static const AString SAMPLES_ALLEN_LOCAL_NAME;
        
        static const AString SAMPLES_ALLEN_SLAB_NUMBER;
        
        static const AString SAMPLES_ALLEN_TISSUE_TYPE;
        
        static const AString SAMPLES_ALTERNATE_PARCELLATION;
        
        static const AString SAMPLES_ALTERNATE_SAMPLE_NAME;
        
        static const AString SAMPLES_BICAN_DONOR_ID;
        
        static const AString SAMPLES_COMMENT;
        
        static const AString SAMPLES_DESIRED_SAMPLE_ENTRY_DATE;
        
        static const AString SAMPLES_DING_ABBREVIATION;
        
        static const AString SAMPLES_DING_FULL_NAME;
        
        static const AString SAMPLES_HEMISPHERE;
        
        static const AString SAMPLES_NHASH_SLAB_ID;
        
        static const AString SAMPLES_PRIMARY_PARCELLATION;
        
        static const AString SAMPLES_SAMPLE_NAME;
        
        static const AString SAMPLES_SAMPLE_NUMBER;
        
        static const AString SAMPLES_SLAB_FACE;
        
        static const AString SAMPLES_SAMPLE_TYPE;
        
        static const AString SAMPLES_SUBJECT_NAME;
        
        static const AString SAMPLES_BORDER_FILENAME;
        
        static const AString SAMPLES_BORDER_CLASS;
        
        static const AString SAMPLES_BORDER_ID;

        static const AString SAMPLES_BORDER_NAME;
        
        static const AString SAMPLES_FOCUS_FILENAME;
        
        static const AString SAMPLES_FOCUS_CLASS;
        
        static const AString SAMPLES_FOCUS_ID;
        
        static const AString SAMPLES_FOCUS_NAME;
        
        static const AString SAMPLES_REMOVED_ALT_ATLAS_DESCRIPTION;
        
        static const AString SAMPLES_REMOVED_ORIG_ATLAS_NAME;
        
        static const AString SAMPLES_REMOVED_ORIG_SHORTHAND_ID;
        
        
        
        
        /** Replaced by SAMPLES_BICAN_DONOR_ID */
        static const AString SAMPLES_OBSOLETE_BICAN_DONOR_ID;
        
        /** Replaced by SAMPLES_SUBJECT_NAME */
        static const AString SAMPLES_OBSOLETE_CASE_ID;
        
        /** Replaced by SAMPLES_DING_FULL_NAME*/
        static const AString SAMPLES_OBSOLETE_DING_DESCRIPTION;
        
        /** Replaced by SAMPLES_ENTRY_DATE */
        static const AString SAMPLES_OBSOLETE_DISSECTION_DATE;
        
        /** Replaced by SAMPLES_ALLEN_LOCAL_NAME */
        static const AString SAMPLES_OBSOLETE_DONOR_ID;
        
        static const AString SAMPLES_OBSOLETE_ENTRY_DATE;

        /** Repalced by SAMPLES_PRIMARY_PARCELLATION*/
        static const AString SAMPLES_OBSOLETE_ORIGINAL_PARCELLATION;

        /** Replaced by  SAMPLES_SAMPLE_NUMBER */
        static const AString SAMPLES_OBSOLETE_SAMPLE_ID;
        
        /** Replaced by SAMPLES_SAMPLE_TYPE */
        static const AString SAMPLES_OBSOLETE_SAMPLE_TYPE;
        
        /** Replaced by SAMPLES_DING_ABBREVIATION */
        static const AString SAMPLES_OBSOLETE_SHORTHAND_ID;
        
        /** Replaced by SAMPLES_SLAB_FACE */
        static const AString SAMPLES_OBSOLETE_SLAB_FACE;
        
        /** Replaced by  SAMPLES_ALLEN_SLAB_NUMBER */
        static const AString SAMPLES_OBSOLETE_SLAB_ID;
        
        
        
        /** Used for converting a QDate to a string */
        static const AString SAMPLES_QT_DATE_FORMAT;
        
        static const AString SAMPLES_OBSOLETE_QT_DATE_FORMAT;
        
        
        static int32_t s_firstLeftHemisphereAllenSlabNumber;
        
        static int32_t s_firstRightHemisphereAllenSlabNumber;
        
        static int32_t s_lastRightHemisphereAllenSlabNumber;
        
    };
    
#ifdef __ANNOTATION_SAMPLE_META_DATA_DECLARE__
    const AString AnnotationSampleMetaData::SAMPLES_ACTUAL_SAMPLE_EDIT_DATE = "Actual Sample Edit Date";

    const AString AnnotationSampleMetaData::SAMPLES_ALLEN_LOCAL_NAME = "Allen Local Name";
    
    const AString AnnotationSampleMetaData::SAMPLES_ALLEN_SLAB_NUMBER = "Allen Slab Number";
    
    const AString AnnotationSampleMetaData::SAMPLES_ALLEN_TISSUE_TYPE= "Allen Tissue Type";
    
    const AString AnnotationSampleMetaData::SAMPLES_ALTERNATE_PARCELLATION = "Alternate Parcellation";
    
    const AString AnnotationSampleMetaData::SAMPLES_ALTERNATE_SAMPLE_NAME = "Alternate Sample Name";
    
    const AString AnnotationSampleMetaData::SAMPLES_BICAN_DONOR_ID = "BICAN Donor ID";
    
    const AString AnnotationSampleMetaData::SAMPLES_COMMENT = "Comment";
    
    const AString AnnotationSampleMetaData::SAMPLES_DESIRED_SAMPLE_ENTRY_DATE = "Desired Sample Entry Date";
    
    const AString AnnotationSampleMetaData::SAMPLES_DING_ABBREVIATION = "Ding Abbreviation";
    
    const AString AnnotationSampleMetaData::SAMPLES_DING_FULL_NAME = "Ding Full Name";
    
    const AString AnnotationSampleMetaData::SAMPLES_HEMISPHERE = "Hemisphere";
    
    const AString AnnotationSampleMetaData::SAMPLES_NHASH_SLAB_ID = "NHash Slab ID";
    
    const AString AnnotationSampleMetaData::SAMPLES_PRIMARY_PARCELLATION = "Primary Parcellation";
    
    const AString AnnotationSampleMetaData::SAMPLES_SAMPLE_NAME = "Sample Name";
    
    const AString AnnotationSampleMetaData::SAMPLES_SAMPLE_NUMBER = "Sample Number";
    
    const AString AnnotationSampleMetaData::SAMPLES_SLAB_FACE = "Slab Face";
    
    const AString AnnotationSampleMetaData::SAMPLES_SAMPLE_TYPE = "Sample Type";
    
    const AString AnnotationSampleMetaData::SAMPLES_SUBJECT_NAME = "Subject Name";
    
    const AString AnnotationSampleMetaData::SAMPLES_BORDER_FILENAME = "Border File Name";
    
    const AString AnnotationSampleMetaData::SAMPLES_BORDER_CLASS = "Border Class";
    
    const AString AnnotationSampleMetaData::SAMPLES_BORDER_ID = "Border ID";
    
    const AString AnnotationSampleMetaData::SAMPLES_BORDER_NAME = "Border Name";
    
    const AString AnnotationSampleMetaData::SAMPLES_FOCUS_FILENAME = "Focus File Name";
    
    const AString AnnotationSampleMetaData::SAMPLES_FOCUS_CLASS = "Focus Class";
    
    const AString AnnotationSampleMetaData::SAMPLES_FOCUS_ID = "Focus ID";

    const AString AnnotationSampleMetaData::SAMPLES_FOCUS_NAME = "Focus Name";
    
    const AString AnnotationSampleMetaData::SAMPLES_REMOVED_ALT_ATLAS_DESCRIPTION = "Alt_Atlas_description";
    
    const AString AnnotationSampleMetaData::SAMPLES_REMOVED_ORIG_ATLAS_NAME = "Orig_atlas_name";
    
    const AString AnnotationSampleMetaData::SAMPLES_REMOVED_ORIG_SHORTHAND_ID = "Orig_shorthand_id";
    
    const AString AnnotationSampleMetaData::SAMPLES_OBSOLETE_BICAN_DONOR_ID = "BICAN_Donor_id";
    
    const AString AnnotationSampleMetaData::SAMPLES_OBSOLETE_CASE_ID = "Case_id";
    
    const AString AnnotationSampleMetaData::SAMPLES_OBSOLETE_DING_DESCRIPTION = "Ding_description";
    
    const AString AnnotationSampleMetaData::SAMPLES_OBSOLETE_DISSECTION_DATE = "Dissection_date";
    
    const AString AnnotationSampleMetaData::SAMPLES_OBSOLETE_DONOR_ID = "Donor_id";
    
    const AString AnnotationSampleMetaData::SAMPLES_OBSOLETE_ENTRY_DATE = "Sample Entry Date";
    
    const AString AnnotationSampleMetaData::SAMPLES_OBSOLETE_ORIGINAL_PARCELLATION = "Original Parcellation";
    
    const AString AnnotationSampleMetaData::SAMPLES_OBSOLETE_SAMPLE_ID = "Sample_id";
    
    const AString AnnotationSampleMetaData::SAMPLES_OBSOLETE_SAMPLE_TYPE = "Sample_type";
    
    const AString AnnotationSampleMetaData::SAMPLES_OBSOLETE_SHORTHAND_ID = "Shorthand_id";
    
    const AString AnnotationSampleMetaData::SAMPLES_OBSOLETE_SLAB_FACE = "Slab_face";
    
    const AString AnnotationSampleMetaData::SAMPLES_OBSOLETE_SLAB_ID = "Slab_id";
    
    
    
    /** Used for converting a QDate to a string */
    const AString AnnotationSampleMetaData::SAMPLES_QT_DATE_FORMAT = "dd/MM/yyyy";
    
    const AString AnnotationSampleMetaData::SAMPLES_OBSOLETE_QT_DATE_FORMAT = "dd MMM yyyy";
    
    
    int32_t AnnotationSampleMetaData::s_firstLeftHemisphereAllenSlabNumber = 1;
    
    int32_t AnnotationSampleMetaData::s_firstRightHemisphereAllenSlabNumber = 40;
    
    int32_t AnnotationSampleMetaData::s_lastRightHemisphereAllenSlabNumber = 99;

#endif // __ANNOTATION_SAMPLE_META_DATA_DECLARE__

} // namespace
#endif  //__ANNOTATION_SAMPLE_META_DATA_H__
