#ifndef __ANNOTATION_META_DATA_H__
#define __ANNOTATION_META_DATA_H__

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


#include <map>
#include <memory>
#include <set>

#include "AnnotationTypeEnum.h"
#include "GiftiMetaData.h"

namespace caret {

    class AnnotationMetaData : public GiftiMetaData {
        
    public:
        AnnotationMetaData(const AnnotationTypeEnum::Enum annotationType);
         
        virtual ~AnnotationMetaData();
        
        AnnotationMetaData(const AnnotationMetaData& obj);

        AnnotationMetaData& operator=(const AnnotationMetaData& obj);
        
        virtual GiftiMetaData* clone() const override;
        
        virtual void afterReadingProcessing() override;
        
        virtual GiftiMetaDataElementDataTypeEnum::Enum getDataTypeForMetaDataName(const QString& metaDataName) const override;
        
        virtual QStringList getValidValuesListForMetaDataName(const QString& metaDataName) const override;
        
        virtual void getMetaDataNamesForEditor(std::vector<AString>& metaDataNamesOut,
                                               std::vector<AString>& requiredMetaDataNamesOut) const override;
        
        virtual AString getToolTipForMetaDataName(const QString& metaDataName) const override;
        
        virtual bool isCompositeMetaDataName(const QString& metaDataName) const override;

        virtual AString getCompositeMetaDataValue(const QString& metaDataName) const override;
        

//        bool operator==(const AnnotationMetaData& obj) const override;
//        
//        bool operator!=(const AnnotationMetaData& rhs) const override { return !((*this) == rhs); }


        // ADD_NEW_METHODS_HERE

    private:
        void copyHelperAnnotationMetaData(const AnnotationMetaData& obj);

        void updatePolyhedronMetaData();
        
        AString assembleCompositeElementComponents(const std::vector<AString>& components,
                                                   const AString& separator) const;
        
        AnnotationTypeEnum::Enum m_annotationType;
        
        static std::vector<AString> s_polyhedronEditorMetaDataNames;
        
        static std::vector<AString> s_polyhedronEditorRequiredMetaDataNames;
        
        static std::map<AString, GiftiMetaDataElementDataTypeEnum::Enum> s_metaDataNameToDataTypeMap;
        
        static std::map<AString, AString> s_metaDataNameToolTips;
        
        static std::set<AString> s_compositeMetaDataNames;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_META_DATA_DECLARE__
    std::vector<AString> AnnotationMetaData::s_polyhedronEditorMetaDataNames;
    
    std::vector<AString> AnnotationMetaData::s_polyhedronEditorRequiredMetaDataNames;
    
    std::map<AString, GiftiMetaDataElementDataTypeEnum::Enum> AnnotationMetaData::s_metaDataNameToDataTypeMap;
    
    std::map<AString, AString> AnnotationMetaData::s_metaDataNameToolTips;
    
    std::set<AString> AnnotationMetaData::s_compositeMetaDataNames;

#endif // __ANNOTATION_META_DATA_DECLARE__

} // namespace
#endif  //__ANNOTATION_META_DATA_H__
