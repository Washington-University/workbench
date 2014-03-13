#ifndef __GIFTIXMLELEMENTS_H__
#define __GIFTIXMLELEMENTS_H__
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

#include <sstream>
#include <AString.h>
#include <stdint.h>

namespace caret {

/**
  * GIFTI XML Element tags
  */
namespace GiftiXmlElements {
    
   /** tag for coordinate transformation matrix element */
   static const AString TAG_COORDINATE_TRANSFORMATION_MATRIX = 
                                         "CoordinateSystemTransformMatrix"; 

   /** tag for data element */
   static const AString TAG_DATA = "Data"; 

   /** tag for data array element */
   static const AString TAG_DATA_ARRAY =  "DataArray"; 

   /** tag for coordinate transformation data space element */
   static const AString TAG_MATRIX_DATA_SPACE =  "DataSpace"; 

   /** tag for GIFTI element */
   static const AString TAG_GIFTI =  "GIFTI"; 

   /** tag for label element */
   static const AString TAG_LABEL =  "Label"; 

   /** tag for label table element */
   static const AString TAG_LABEL_TABLE =  "LabelTable"; 

   /** tag for a metadata entry */
   static const AString TAG_METADATA_ENTRY =  "MD"; 

   /** tag for matrix data */
   static const AString TAG_MATRIX_DATA =  "MatrixData"; 

   /** tag for metadata */
   static const AString TAG_METADATA =  "MetaData"; 

   /** tag for metadata name */
   static const AString TAG_METADATA_NAME =  "Name"; 

   /** tag for coordinate transformation space element */
   static const AString TAG_MATRIX_TRANSFORMED_SPACE =  "TransformedSpace"; 

   /** tag for metadata value element */
   static const AString TAG_METADATA_VALUE =  "Value"; 



   /** attribute for data array indexing order */
   static const AString ATTRIBUTE_DATA_ARRAY_INDEXING_ORDER = 
                                                           "ArrayIndexingOrder"; 

   /** attribute for data array data type */
   static const AString ATTRIBUTE_DATA_ARRAY_DATA_TYPE =  "DataType"; 

   /** attribute for data array dimensionality */
   static const AString ATTRIBUTE_DATA_ARRAY_DIMENSIONALITY = 
                                                               "Dimensionality"; 

   /** attribute for data array dimensionality */
   static const AString ATTRIBUTE_DATA_ARRAY_DIM_PREFIX =  "Dim"; 

   /** attribute for data array encoding */
   static const AString ATTRIBUTE_DATA_ARRAY_ENCODING =  "Encoding"; 

   /** attribute for data array ending */
   static const AString ATTRIBUTE_DATA_ARRAY_ENDIAN =  "Endian"; 

   /** attribute for data array external file name */
   static const AString ATTRIBUTE_DATA_ARRAY_EXTERNAL_FILE_NAME = 
                                                            "ExternalFileName"; 

   /** attribute for data array external file offset */
   static const AString ATTRIBUTE_DATA_ARRAY_EXTERNAL_FILE_OFFSET = 
                                                          "ExternalFileOffset"; 

   /** attribute for data array label index REPLACED BY KEY*/
   static const AString ATTRIBUTE_LABEL_INDEX_obsolete =  "Index"; 

   /** attribute for data array label key */
   static const AString ATTRIBUTE_LABEL_KEY =  "Key"; 

   /** attribute for data array label red color component */
   static const AString ATTRIBUTE_LABEL_RED =  "Red"; 

   /** attribute for data array label green color component */
   static const AString ATTRIBUTE_LABEL_GREEN =  "Green"; 

   /** attribute for data array label blue color component */
   static const AString ATTRIBUTE_LABEL_BLUE =  "Blue"; 

   /** attribute for data array label alpha color component */
   static const AString ATTRIBUTE_LABEL_ALPHA =  "Alpha"; 

    /** attribute for data array label X-coordinate */
    static const AString ATTRIBUTE_LABEL_X =  "X"; 
    
    /** attribute for data array label X-coordinate */
    static const AString ATTRIBUTE_LABEL_Y =  "Y"; 
    
    /** attribute for data array label X-coordinate */
    static const AString ATTRIBUTE_LABEL_Z =  "Z"; 
    
   /** attribute for data array intent */
   static const AString ATTRIBUTE_DATA_ARRAY_INTENT =  "Intent"; 

   /** attribute for data array intent parameter 1 */
   static const AString ATTRIBUTE_DATA_ARRAY_INTENT_P1 =  "intent_p1"; 

   /** attribute for data array intent parameter 2 */
   static const AString ATTRIBUTE_DATA_ARRAY_INTENT_P2 =  "intent_p2"; 

   /** attribute for data array intent parameter 3 */
   static const AString ATTRIBUTE_DATA_ARRAY_INTENT_P3 =  "intent_p3"; 

   /** attribute for GIFTI Number of Data Arrays */
   static const AString ATTRIBUTE_GIFTI_NUMBER_OF_DATA_ARRAYS = 
                                                          "NumberOfDataArrays"; 

   /** attribute for GIFTI Version */
   static const AString ATTRIBUTE_GIFTI_VERSION =  "Version"; 
    
   AString getAttributeDimension(int32_t dimIndex);
    
    /**
     * Get an attribute dimension (Dim0, Dim1, etc).
     * @param  Index value for dimension.
     * @return Dim0, Dim1, etc
     *
    static AString getAttributeDimension(int32_t dimIndex) {
        std::ostringstream str;
        str << ATTRIBUTE_DATA_ARRAY_DIM_PREFIX << dimIndex;;
        return str.str();
    }
    */
/*
    static const AString tagGIFTI  = "GIFTI";
    static const AString tagMetaData = "MetaData";
    static const AString tagMD = "MD";
    static const AString tagName = "Name";
    static const AString tagValue = "Value";
    static const AString tagDataArray = "DataArray";
    static const AString tagData = "Data";
    static const AString tagLabelTable = "LabelTable";
    static const AString tagLabel = "Labe";
    static const AString tagMatrix = "CoordinateSystemTransformMatrix";
    static const AString tagMatrixDataSpace = "DataSpace";
    static const AString tagMatrixTransformedSpace = "TransformedSpace";
    static const AString tagMatrixData = "MatrixData";
    
    static const AString attVersion = "Version";
    static const AString attNumberOfDataArrays = "NumberOfDataArrays";
    static const AString attArraySubscriptingOrder = "ArrayIndexingOrder";
    static const AString attKey = "Key";
    static const AString attRed = "Red";
    static const AString attGreen = "Green";
    static const AString attBlue = "Blue";
    static const AString attAlpha = "Alpha";
    static const AString attIntent = "Intent";
    static const AString attDataType = "DataType";
    static const AString attDimensionality = "Dimensionality";
    static const AString attDim = "Dim";
    static const AString attEncoding = "Encoding";
    static const AString attEndian = "Endian";
    static const AString attExternalFileName = "ExternalFileName";
    static const AString attExternalFileOffset = "ExternalFileOffset";
*/
    
    
};  // namespace

    
}  // namespace

#endif // __GIFTIXMLELEMENTS_H__

