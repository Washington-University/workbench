#ifndef __CIFTI_XML_NEW_H__
#define __CIFTI_XML_NEW_H__

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
/*LICENSE_END*/

#include "AString.h"
#include "CaretPointer.h"
#include "CiftiIndexMap.h"
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
    class CiftiXMLNew
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
        const CiftiIndexMap& getMap(const int& direction) const;
        CiftiIndexMap::MappingType getMappingType(const int& direction) const;
        GiftiMetaData* getFileMetadata() const;//HACK: allow modification of palette and metadata within XML without setting the xml on a file again
        PaletteColorMapping* getFilePalette() const;
        
        const CiftiBrainModelsMap& getBrainModelsMap(const int& direction) const;//convenience functions
        const CiftiLabelsMap& getLabelsMap(const int& direction) const;
        const CiftiParcelsMap& getParcelsMap(const int& direction) const;
        const CiftiScalarsMap& getScalarsMap(const int& direction) const;
        const CiftiSeriesMap& getSeriesMap(const int& direction) const;
        
        void setNumberOfDimensions(const int& num);
        void setMap(const int& direction, const CiftiIndexMap& mapIn);
        
        void readXML(QXmlStreamReader& xml);
        void readXML(const AString& text);
        void readXML(const QByteArray& data);
        
        AString writeXMLToString(const CiftiVersion& writingVersion = CiftiVersion()) const;
        QByteArray writeXMLToQByteArray(const CiftiVersion& writingVersion = CiftiVersion()) const;
        void writeXML(QXmlStreamWriter& xml, const CiftiVersion& writingVersion = CiftiVersion()) const;
        
        CiftiXMLNew() { }
        CiftiXMLNew(const CiftiXMLNew& rhs);
        CiftiXMLNew& operator=(const CiftiXMLNew& rhs);
    private:
        std::vector<CaretPointer<CiftiIndexMap> > m_indexMaps;
        CiftiVersion m_parsedVersion;
        mutable GiftiMetaData m_fileMetaData;//hack to allow metadata to be modified without allowing dimension-changing operations
        mutable CaretPointer<PaletteColorMapping> m_filePalette;
        
        void copyHelper(const CiftiXMLNew& rhs);
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

#endif //__CIFTI_XML_NEW_H__
