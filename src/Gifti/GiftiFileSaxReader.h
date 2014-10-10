
#ifndef __GIFTI_FILE_SAX_READER_H__
#define __GIFTI_FILE_SAX_READER_H__

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

#include <stack>
#include <AString.h>
#include <stdint.h>

#include "CaretPointer.h"
#include "GiftiArrayIndexingOrderEnum.h"
#include "GiftiEndianEnum.h"
#include "GiftiEncodingEnum.h"
#include "NiftiEnums.h"
#include "XmlSaxParserException.h"
#include "XmlSaxParserHandlerInterface.h"


namespace caret {

    class GiftiDataArray;
    class GiftiFile;
    class GiftiLabelTableSaxReader;
    class GiftiMetaDataSaxReader;
    class GiftiFile;
    class Matrix4x4;
    class XmlAttributes;
    class XmlException;
    
    /// class for reading a GIFTI Node Data File with a SAX Parser
    class GiftiFileSaxReader : public CaretObject, public XmlSaxParserHandlerInterface {
    public:
        GiftiFileSaxReader(GiftiFile* giftiFileIn);
        
        virtual ~GiftiFileSaxReader();
        
        void startElement(const AString& namespaceURI,
                          const AString& localName,
                          const AString& qName,
                          const XmlAttributes& attributes);
        
        void endElement(const AString& namspaceURI,
                        const AString& localName,
                        const AString& qName);
        
        void characters(const char* ch);
        
        void fatalError(const XmlSaxParserException& e);
        
        void warning(const XmlSaxParserException& e);
        
        void error(const XmlSaxParserException& e);
        
        void startDocument();
        
        void endDocument();
        
        
    protected:
        /// file reading states
        enum STATE {
            /// no state
            STATE_NONE,
            /// processing GIFTI tag
            STATE_GIFTI,
            /// processing MetaData tag
            STATE_METADATA,
            /// processing LabelTable tag
            STATE_LABEL_TABLE,
            /// processing DataArray tag
            STATE_DATA_ARRAY,
            /// processing DataArray Data tag
            STATE_DATA_ARRAY_DATA,
            /// processing DataArray Matrix Tag
            STATE_DATA_ARRAY_MATRIX,
            /// processing DataArray Matrix Data Space Tag
            STATE_DATA_ARRAY_MATRIX_DATA_SPACE,
            /// processing DataArray Matrix Transformed Space Tag
            STATE_DATA_ARRAY_MATRIX_TRANSFORMED_SPACE,
            /// processing DataArray Matrix Data Tag
            STATE_DATA_ARRAY_MATRIX_DATA
        };
        
        // process the array data into numbers
        void processArrayData();
        
        // create a data array
        void createDataArray(const XmlAttributes& attributes);
        
        /// file reading state
        STATE state;
        
        /// the state stack used when reading a file
        std::stack<STATE> stateStack;
        
        /// the error message
        AString errorMessage;
        
        /// GIFTI file that is being read
        GiftiFile* giftiFile;
        
        /// element text
        AString elementText;
        
        /// GIFTI data array being read
        CaretPointer<GiftiDataArray> dataArray;
        
        /// GIFTI label table being read
        GiftiLabelTable* labelTable;
        
        /// GIFTI label table sax reader
        GiftiLabelTableSaxReader* labelTableSaxReader;
        
        /// GIFTI meta data sax reader
        GiftiMetaDataSaxReader* metaDataSaxReader;
        
        /// GIFTI matrix data being read
        Matrix4x4* matrix;
        
        /// endian attribute data
        GiftiEndianEnum::Enum endianForReadingArrayData;
        
        /// array subscripting order for reading
        GiftiArrayIndexingOrderEnum::Enum arraySubscriptingOrderForReadingArrayData;
        
        /// data type for reading
        NiftiDataTypeEnum::Enum dataTypeForReadingArrayData;
        
        /// dimension for reading
        std::vector<int64_t> dimensionsForReadingArrayData;
        
        /// encoding for reading
        GiftiEncodingEnum::Enum encodingForReadingArrayData;
        
        /// data location for reading
        //GiftiDataArray::DATA_LOCATION dataLocationForReadingArrayData;
        
        /// external file name
        AString externalFileNameForReadingData;
        
        /// external file offset
        int64_t externalFileOffsetForReadingData;
        
        /// tracks if data has been read since external binary may not have DATA tag
        bool dataArrayDataHasBeenRead;
    };

} // namespace

#endif // __GIFTI_FILE_SAX_READER_H__

