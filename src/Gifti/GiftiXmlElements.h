#ifndef __GIFTIXMLELEMENTS_H__
#define __GIFTIXMLELEMENTS_H__
/*
 * Copyright 1995-2009 Washington University School of Medicine.
 *
 * http://brainmap.wustl.edu
 *
 * CARET is free software
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <sstream>
#include <QString>
#include <stdint.h>

namespace caret {

/**
  * GIFTI XML Element tags
  */
namespace GiftiXmlElements {
    
   /** tag for coordinate transformation matrix element */
   static const QString TAG_COORDINATE_TRANSFORMATION_MATRIX = 
                                         "CoordinateSystemTransformMatrix"; 

   /** tag for data element */
   static const QString TAG_DATA = "Data"; 

   /** tag for data array element */
   static const QString TAG_DATA_ARRAY =  "DataArray"; 

   /** tag for coordinate transformation data space element */
   static const QString TAG_MATRIX_DATA_SPACE =  "DataSpace"; 

   /** tag for GIFTI element */
   static const QString TAG_GIFTI =  "GIFTI"; 

   /** tag for label element */
   static const QString TAG_LABEL =  "Label"; 

   /** tag for label table element */
   static const QString TAG_LABEL_TABLE =  "LabelTable"; 

   /** tag for a metadata entry */
   static const QString TAG_METADATA_ENTRY =  "MD"; 

   /** tag for matrix data */
   static const QString TAG_MATRIX_DATA =  "MatrixData"; 

   /** tag for metadata */
   static const QString TAG_METADATA =  "MetaData"; 

   /** tag for metadata name */
   static const QString TAG_METADATA_NAME =  "Name"; 

   /** tag for coordinate transformation space element */
   static const QString TAG_MATRIX_TRANSFORMED_SPACE =  "TransformedSpace"; 

   /** tag for metadata value element */
   static const QString TAG_METADATA_VALUE =  "Value"; 



   /** attribute for data array indexing order */
   static const QString ATTRIBUTE_DATA_ARRAY_INDEXING_ORDER = 
                                                           "ArrayIndexingOrder"; 

   /** attribute for data array data type */
   static const QString ATTRIBUTE_DATA_ARRAY_DATA_TYPE =  "DataType"; 

   /** attribute for data array dimensionality */
   static const QString ATTRIBUTE_DATA_ARRAY_DIMENSIONALITY = 
                                                               "Dimensionality"; 

   /** attribute for data array dimensionality */
   static const QString ATTRIBUTE_DATA_ARRAY_DIM_PREFIX =  "Dim"; 

   /** attribute for data array encoding */
   static const QString ATTRIBUTE_DATA_ARRAY_ENCODING =  "Encoding"; 

   /** attribute for data array ending */
   static const QString ATTRIBUTE_DATA_ARRAY_ENDIAN =  "Endian"; 

   /** attribute for data array external file name */
   static const QString ATTRIBUTE_DATA_ARRAY_EXTERNAL_FILE_NAME = 
                                                            "ExternalFileName"; 

   /** attribute for data array external file offset */
   static const QString ATTRIBUTE_DATA_ARRAY_EXTERNAL_FILE_OFFSET = 
                                                          "ExternalFileOffset"; 

   /** attribute for data array label index REPLACED BY KEY*/
   static const QString ATTRIBUTE_LABEL_INDEX_obsolete =  "Index"; 

   /** attribute for data array label key */
   static const QString ATTRIBUTE_LABEL_KEY =  "Key"; 

   /** attribute for data array label red color component */
   static const QString ATTRIBUTE_LABEL_RED =  "Red"; 

   /** attribute for data array label green color component */
   static const QString ATTRIBUTE_LABEL_GREEN =  "Green"; 

   /** attribute for data array label blue color component */
   static const QString ATTRIBUTE_LABEL_BLUE =  "Blue"; 

   /** attribute for data array label alpha color component */
   static const QString ATTRIBUTE_LABEL_ALPHA =  "Alpha"; 

    /** attribute for data array label X-coordinate */
    static const QString ATTRIBUTE_LABEL_X =  "X"; 
    
    /** attribute for data array label X-coordinate */
    static const QString ATTRIBUTE_LABEL_Y =  "Y"; 
    
    /** attribute for data array label X-coordinate */
    static const QString ATTRIBUTE_LABEL_Z =  "Z"; 
    
   /** attribute for data array intent */
   static const QString ATTRIBUTE_DATA_ARRAY_INTENT =  "Intent"; 

   /** attribute for data array intent parameter 1 */
   static const QString ATTRIBUTE_DATA_ARRAY_INTENT_P1 =  "intent_p1"; 

   /** attribute for data array intent parameter 2 */
   static const QString ATTRIBUTE_DATA_ARRAY_INTENT_P2 =  "intent_p2"; 

   /** attribute for data array intent parameter 3 */
   static const QString ATTRIBUTE_DATA_ARRAY_INTENT_P3 =  "intent_p3"; 

   /** attribute for GIFTI Number of Data Arrays */
   static const QString ATTRIBUTE_GIFTI_NUMBER_OF_DATA_ARRAYS = 
                                                          "NumberOfDataArrays"; 

   /** attribute for GIFTI Version */
   static const QString ATTRIBUTE_GIFTI_VERSION =  "Version"; 
    
   QString getAttributeDimension(int32_t dimIndex);
    
    /**
     * Get an attribute dimension (Dim0, Dim1, etc).
     * @param  Index value for dimension.
     * @return Dim0, Dim1, etc
     *
    static QString getAttributeDimension(int32_t dimIndex) {
        std::ostringstream str;
        str << ATTRIBUTE_DATA_ARRAY_DIM_PREFIX << dimIndex;;
        return str.str();
    }
    */
/*
    static const QString tagGIFTI  = "GIFTI";
    static const QString tagMetaData = "MetaData";
    static const QString tagMD = "MD";
    static const QString tagName = "Name";
    static const QString tagValue = "Value";
    static const QString tagDataArray = "DataArray";
    static const QString tagData = "Data";
    static const QString tagLabelTable = "LabelTable";
    static const QString tagLabel = "Labe";
    static const QString tagMatrix = "CoordinateSystemTransformMatrix";
    static const QString tagMatrixDataSpace = "DataSpace";
    static const QString tagMatrixTransformedSpace = "TransformedSpace";
    static const QString tagMatrixData = "MatrixData";
    
    static const QString attVersion = "Version";
    static const QString attNumberOfDataArrays = "NumberOfDataArrays";
    static const QString attArraySubscriptingOrder = "ArrayIndexingOrder";
    static const QString attKey = "Key";
    static const QString attRed = "Red";
    static const QString attGreen = "Green";
    static const QString attBlue = "Blue";
    static const QString attAlpha = "Alpha";
    static const QString attIntent = "Intent";
    static const QString attDataType = "DataType";
    static const QString attDimensionality = "Dimensionality";
    static const QString attDim = "Dim";
    static const QString attEncoding = "Encoding";
    static const QString attEndian = "Endian";
    static const QString attExternalFileName = "ExternalFileName";
    static const QString attExternalFileOffset = "ExternalFileOffset";
*/
    
    
};  // namespace

    
}  // namespace

#endif // __GIFTIXMLELEMENTS_H__

