#ifndef __CIFTI_XML_H__
#define __CIFTI_XML_H__

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

#include "CaretPointer.h"
#include "CiftiMappingType.h"
#include "CiftiVersion.h"
#include "GiftiMetaData.h"
//could also be forward declared
#include "PaletteColorMapping.h"

//just include the mapping types themselves, for convenience
#include "CiftiBrainModelsMap.h"
#include "CiftiLabelsMap.h"
#include "CiftiParcelsMap.h"
#include "CiftiScalarsMap.h"
#include "CiftiSeriesMap.h"

#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include <vector>

namespace caret
{
    class CiftiXML
    {
    public:
        enum
        {
            ALONG_ROW = 0,
            ALONG_COLUMN = 1,
            ALONG_STACK = 2//better name for this?
        };
        int getNumberOfDimensions() const { return m_indexMaps.size(); }
        const CiftiVersion& getParsedVersion() const { return m_parsedVersion; }
        const CiftiMappingType* getMap(const int& direction) const;//can return null in unfilled XML object
        CiftiMappingType* getMap(const int& direction);//can return null in unfilled XML object
        GiftiMetaData* getFileMetaData() const;//HACK: allow modification of palette and metadata within XML without setting the xml on a file again
        PaletteColorMapping* getFilePalette() const;
        
        CiftiMappingType::MappingType getMappingType(const int& direction) const;//convenience functions
        const CiftiBrainModelsMap& getBrainModelsMap(const int& direction) const;
        CiftiBrainModelsMap& getBrainModelsMap(const int& direction);
        const CiftiLabelsMap& getLabelsMap(const int& direction) const;
        CiftiLabelsMap& getLabelsMap(const int& direction);
        const CiftiParcelsMap& getParcelsMap(const int& direction) const;
        CiftiParcelsMap& getParcelsMap(const int& direction);
        const CiftiScalarsMap& getScalarsMap(const int& direction) const;
        CiftiScalarsMap& getScalarsMap(const int& direction);
        const CiftiSeriesMap& getSeriesMap(const int& direction) const;
        CiftiSeriesMap& getSeriesMap(const int& direction);
        int64_t getDimensionLength(const int& direction) const;
        std::vector<int64_t> getDimensions() const;
        
        void setNumberOfDimensions(const int& num);
        void setMap(const int& direction, const CiftiMappingType& mapIn);
        void clear();
        
        void readXML(QXmlStreamReader& xml);
        void readXML(const QString& text);
        void readXML(const QByteArray& data);
        
        QString writeXMLToString(const CiftiVersion& writingVersion = CiftiVersion()) const;
        QByteArray writeXMLToQByteArray(const CiftiVersion& writingVersion = CiftiVersion()) const;
        void writeXML(QXmlStreamWriter& xml, const CiftiVersion& writingVersion = CiftiVersion()) const;
        
        ///uses the mapping types to figure out what the intent info should be
        int32_t getIntentInfo(const CiftiVersion& writingVersion, char intentNameOut[16]) const;
        
        CiftiXML() { }
        CiftiXML(const CiftiXML& rhs);
        CiftiXML& operator=(const CiftiXML& rhs);
        bool operator==(const CiftiXML& rhs) const;
        bool operator!=(const CiftiXML& rhs) const { return !((*this) == rhs); }
        bool approximateMatch(const CiftiXML& rhs) const;
        
        bool mutablesModified() const;
        void clearMutablesModified() const;//HACK: clear modified status on a const object
    private:
        std::vector<CaretPointer<CiftiMappingType> > m_indexMaps;
        CiftiVersion m_parsedVersion;
        mutable GiftiMetaData m_fileMetaData;//hack to allow metadata to be modified without allowing dimension-changing operations
        mutable CaretPointer<PaletteColorMapping> m_filePalette;
        
        void copyHelper(const CiftiXML& rhs);
        //parsing functions
        void parseCIFTI1(QXmlStreamReader& xml);
        void parseMatrix1(QXmlStreamReader& xml);
        void parseCIFTI2(QXmlStreamReader& xml);
        void parseMatrix2(QXmlStreamReader& xml);
        void parseMatrixIndicesMap1(QXmlStreamReader& xml);
        void parseMatrixIndicesMap2(QXmlStreamReader& xml);
        //writing functions
        void writeMatrix1(QXmlStreamWriter& xml) const;
        void writeMatrix2(QXmlStreamWriter& xml) const;
    };
}

#endif //__CIFTI_XML_H__
