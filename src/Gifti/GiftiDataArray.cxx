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

#include <algorithm>
#include <fstream>
#include <ostream>
#include <limits>
#include <sstream>

#include "Base64.h"
#include "ByteOrderEnum.h"
#include "ByteSwapping.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DataCompressZLib.h"

//#include "FileUtilities.h"
#include "FastStatistics.h"
#include "GiftiDataArray.h"
#include "GiftiFile.h"
#include "GiftiMetaDataXmlElements.h"
#include "GiftiXmlElements.h"
#include "Histogram.h"
#include "NiftiEnums.h"
#include "PaletteColorMapping.h"
#include "SystemUtilities.h"
#include "XmlWriter.h"

using namespace caret;

/**
 * constructor.
 */
GiftiDataArray::GiftiDataArray(const NiftiIntentEnum::Enum intentIn,
                               const NiftiDataTypeEnum::Enum dataTypeIn,
                               const std::vector<int64_t>& dimensionsIn,
                               const GiftiEncodingEnum::Enum encodingIn)
{
   intent = intentIn;
   dataTypeSize = 0;
   dataPointerFloat = NULL;
   dataPointerInt = NULL;
   dataPointerUByte = NULL;    
   this->paletteColorMapping = NULL;
  this->descriptiveStatistics = NULL;
    this->descriptiveStatisticsLimitedValues = NULL;
   clear();
   dataType = dataTypeIn;
   setDimensions(dimensionsIn);
   encoding = encodingIn;
   endian = getSystemEndian();
   arraySubscriptingOrder = GiftiArrayIndexingOrderEnum::ROW_MAJOR_ORDER;
   externalFileName = "";
   externalFileOffset = 0;
   
    if (intent == NiftiIntentEnum::NIFTI_INTENT_POINTSET) {   
      Matrix4x4 gm;
      matrices.push_back(gm);
   }
}

/**
 * constructor.
 */
GiftiDataArray::GiftiDataArray(const NiftiIntentEnum::Enum intentIn)
{
   intent = intentIn;
   dataTypeSize = 0;
   dataPointerFloat = NULL;
   dataPointerInt = NULL;
   dataPointerUByte = NULL;
   this->paletteColorMapping = NULL;
   this->descriptiveStatistics = NULL;
    this->descriptiveStatisticsLimitedValues = NULL;
   clear();
   dimensions.clear();
   encoding = GiftiEncodingEnum::ASCII;
   endian = getSystemEndian();
    arraySubscriptingOrder = GiftiArrayIndexingOrderEnum::ROW_MAJOR_ORDER;
   externalFileName = "";
   externalFileOffset = 0;
   
    /*if (intent == NiftiIntentEnum::NIFTI_INTENT_POINTSET) {
      Matrix4x4 gm;
      matrices.push_back(gm);
   }//*///TSC: do not add a fake matrix with no data or transformed space BEFORE knowing if one already exists, instead add one in validate, after reading, if none exists
   
   dataType = NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32;
   getDataTypeAppropriateForIntent(intent, dataType);
    
}

/**
 * destructor.
 */
GiftiDataArray::~GiftiDataArray()
{
   clear();
}

/**
 * copy constructor.
 */
GiftiDataArray::GiftiDataArray(const GiftiDataArray& nda)
                : CaretObject(), TracksModificationInterface()
{
   dataTypeSize = 0;
   dataPointerFloat = NULL;
   dataPointerInt = NULL;
   dataPointerUByte = NULL;
   this->paletteColorMapping = NULL;
   this->descriptiveStatistics = NULL;
    this->descriptiveStatisticsLimitedValues = NULL;
   copyHelperGiftiDataArray(nda);
}

/**
 * assignment operator.
 */
GiftiDataArray&
GiftiDataArray::operator=(const GiftiDataArray& nda)
{
   if (this != &nda) {
      copyHelperGiftiDataArray(nda);
   }
   return *this;
}
      
/**
 * the copy helper (used by copy constructor and assignment operator).
 */
void 
GiftiDataArray::copyHelperGiftiDataArray(const GiftiDataArray& nda)
{
    this->paletteColorMapping = NULL;
    if (nda.paletteColorMapping != NULL) {
        this->paletteColorMapping = new PaletteColorMapping(*nda.paletteColorMapping);
    }
    if (this->descriptiveStatistics != NULL) {
        delete this->descriptiveStatistics;
        this->descriptiveStatistics = NULL;
    }
    if (this->descriptiveStatisticsLimitedValues != NULL) {
        delete this->descriptiveStatisticsLimitedValues;
        this->descriptiveStatisticsLimitedValues = NULL;
    }
   intent = nda.intent;
   encoding = nda.encoding;
   arraySubscriptingOrder = nda.arraySubscriptingOrder;
   dataType = nda.dataType;
   //dataLocation = nda.dataLocation;
   dataTypeSize = nda.dataTypeSize;
   endian = nda.endian;
   dimensions = nda.dimensions;
   allocateData();
   data = nda.data;
   metaData = nda.metaData;
   nonWrittenMetaData = nda.nonWrittenMetaData;
   externalFileName = nda.externalFileName;
   externalFileOffset = nda.externalFileOffset;
   minMaxFloatValuesValid = nda.minMaxFloatValuesValid;
   minValueFloat = nda.minValueFloat;
   maxValueFloat = nda.maxValueFloat;
   minMaxFloatValuesValid = nda.minMaxFloatValuesValid;
   minValueInt = nda.minValueInt;
   maxValueInt = nda.maxValueInt;
   minMaxIntValuesValid = nda.minMaxIntValuesValid;
   minMaxPercentageValuesValid = nda.minMaxPercentageValuesValid;
   negMaxPct = nda.negMaxPct;
   negMinPct = nda.negMinPct;
   posMinPct = nda.posMinPct;
   posMaxPct = nda.posMaxPct;
   negMaxPctValue = nda.negMaxPctValue;
   negMinPctValue = nda.negMinPctValue;
   posMinPctValue = nda.posMinPctValue;
   posMaxPctValue = nda.posMaxPctValue;
   matrices = nda.matrices;
    
   setModified();
}

/**
 * get the data type appropriate for the intent (returns true if intent is valid).
 */
bool 
GiftiDataArray::getDataTypeAppropriateForIntent(const NiftiIntentEnum::Enum intent,
                                                  NiftiDataTypeEnum::Enum& dataTypeOut)
{
   //
   // Default to float
   //
   dataTypeOut = NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32;
    
   if (intent == NiftiIntentEnum::NIFTI_INTENT_POINTSET) {
      dataTypeOut = NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32;
   }
   else if (intent == NiftiIntentEnum::NIFTI_INTENT_TIME_SERIES) {
      dataTypeOut = NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32;
   }
   else if (intent == NiftiIntentEnum::NIFTI_INTENT_NORMAL) {
      dataTypeOut = NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32;
   }
   else if (intent == NiftiIntentEnum::NIFTI_INTENT_LABEL) {
      dataTypeOut = NiftiDataTypeEnum::NIFTI_TYPE_INT32;
   }
   else if ((intent == NiftiIntentEnum::NIFTI_INTENT_RGB_VECTOR) ||
            (intent == NiftiIntentEnum::NIFTI_INTENT_RGBA_VECTOR)) {
      dataTypeOut = NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32;
   }
   else if (intent == NiftiIntentEnum::NIFTI_INTENT_SHAPE) {
      dataTypeOut = NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32;
   }
   else if (intent == NiftiIntentEnum::NIFTI_INTENT_SYMMATRIX) {
      dataTypeOut = NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32;
   }
   else if (intent == NiftiIntentEnum::NIFTI_INTENT_TRIANGLE) {
      dataTypeOut = NiftiDataTypeEnum::NIFTI_TYPE_INT32;
   }
//   else {
//       CaretLogFine("Unrecogized NIFTI intent \""
//                       + NiftiIntentEnum::toName(intent)
//                       + "\" in GiftiDataArray::getDataTypeAppropriateForIntent().");
//      return false;
//   }
   return true;
}
      
/**
 * add nodes.
 */
void 
GiftiDataArray::addRows(const int32_t numRowsToAdd)
{
   dimensions[0] += numRowsToAdd;
   allocateData();
}

/**
 * delete rows.
 */
void 
GiftiDataArray::deleteRows(const std::vector<int32_t>& rowsToDeleteIn)
{
   if (rowsToDeleteIn.empty()) {
      return;
   }
   
   //
   // Sort rows in reverse order
   //
   std::vector<int32_t> rowsToDelete = rowsToDeleteIn;
   std::sort(rowsToDelete.begin(), rowsToDelete.end());
   (void)std::unique(rowsToDelete.begin(), rowsToDelete.end());
   std::reverse(rowsToDelete.begin(), rowsToDelete.end());
   
   //
   // size of row in bytes
   //
   int64_t numBytesInRow = 1;
   for (uint32_t i = 1; i < dimensions.size(); i++) {
      numBytesInRow = dimensions[i];
   }
   numBytesInRow *= dataTypeSize;
   
   //
   // Remove the unneeded rows
   //
   for (uint32_t i = 0; i < rowsToDelete.size(); i++) {
      const int32_t offset = rowsToDelete[i] * numBytesInRow;
      data.erase(data.begin() + offset, data.begin() + offset + numBytesInRow);
   }
   
   //
   // Update the dimensions
   //
   dimensions[0] -= rowsToDelete.size();
   setModified();
}      

/**
 * set number of nodes which causes reallocation of data.
 */
void 
GiftiDataArray::setDimensions(const std::vector<int64_t> dimensionsIn)
{
   dimensions = dimensionsIn;
   if (dimensions.size() == 1) {
      dimensions.push_back(1);
   }
   else if (dimensions.empty()) {
      dimensions.push_back(0);
      dimensions.push_back(0);
   }
   allocateData();
}

/**
 * allocate data for this array.
 */
void 
GiftiDataArray::allocateData()
{
   //
   // Determine the number of items to allocate
   //
   int64_t dataSizeInBytes = 1;
   for (uint32_t i = 0; i < dimensions.size(); i++) {
      dataSizeInBytes *= dimensions[i];
   }
   
   //
   // Bytes required by each data type
   //
   dataTypeSize = 0;
   switch (dataType) {
      case NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32:
         dataTypeSize = sizeof(float);
         break;
      case NiftiDataTypeEnum::NIFTI_TYPE_INT32:
         dataTypeSize = sizeof(int32_t);
         break;
      case NiftiDataTypeEnum::NIFTI_TYPE_UINT8:
         dataTypeSize = sizeof(uint8_t);
         break;
       default:
           CaretAssertMessage(0, "Unsupported GIFTI data type.");
           break;
   }
   
   dataSizeInBytes *= dataTypeSize;
   
   //
   // Does data need to be allocated
   //
   if (dataSizeInBytes > 0) {
       //
       //  Allocate the needed memory
       //
       data.resize(dataSizeInBytes);
   }
   else {
      data.clear();
   }
   
   //
   // Update the data pointers
   //
   updateDataPointers();

   setModified();
}

/**
 * update the data pointers.
 */
void 
GiftiDataArray::updateDataPointers()
{
   dataPointerFloat = NULL;
   dataPointerInt = NULL;
   dataPointerUByte = NULL;
   if (data.empty() == false) {
      switch (dataType) {
         case NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32:
            dataPointerFloat = (float*)&data[0];
            break;
         case NiftiDataTypeEnum::NIFTI_TYPE_INT32:
            dataPointerInt   = (int32_t*)&data[0];
            break;
         case NiftiDataTypeEnum::NIFTI_TYPE_UINT8:
            dataPointerUByte = (uint8_t*)&data[0];
            break;
          default:
              CaretAssertMessage(0, "Unsupported GIFTI Data Type");
              break;
      }
   }
}
      
/**
 * reset column.
 */
void 
GiftiDataArray::clear()
{
   arraySubscriptingOrder = GiftiArrayIndexingOrderEnum::ROW_MAJOR_ORDER;
   encoding = GiftiEncodingEnum::ASCII;
   dataType = NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32;
   endian = getSystemEndian();
   dataTypeSize = sizeof(float);
   metaData.clear();
   nonWrittenMetaData.clear();
   dimensions.clear();
   setDimensions(dimensions);
   externalFileName = "";
   externalFileOffset = 0;
   minMaxFloatValuesValid = false;
   minMaxPercentageValuesValid = false;
   
    if (this->paletteColorMapping != NULL) {
        delete this->paletteColorMapping;
        this->paletteColorMapping = NULL;
    }
    if (this->descriptiveStatistics != NULL) {
        delete this->descriptiveStatistics;
        this->descriptiveStatistics = NULL;
    }
    if (this->descriptiveStatisticsLimitedValues != NULL) {
        delete this->descriptiveStatisticsLimitedValues;
        this->descriptiveStatisticsLimitedValues = NULL;
    }
   // do not clear
   // parentGiftiDataFile;
   // arrayType;
   
   // setDimensions will call allocateData() which will set
   // dataPointer
   // dataPointerFloat
   // dataPointerInt
   // dataPointerUByte;
}

/**
 * get the number of nodes (1st dimension).
 */
int64_t 
GiftiDataArray::getNumberOfRows() const 
{ 
   if (dimensions.empty() == false) {
      return dimensions[0]; 
   }
   return 0;
}

/**
 * get the total number of elements.
 */
int64_t 
GiftiDataArray::getTotalNumberOfElements() const
{
   if (dimensions.empty()) {
      return 0;
   }
   
   int64_t num = 1;
   for (uint32_t i = 0; i < dimensions.size(); i++) {
      num *= dimensions[i];
   }
   return num;
}

/**
 * get number of components per node (2nd dimension).
 */
int64_t 
GiftiDataArray::getNumberOfComponents() const 
{ 
   if (dimensions.size() > 1) {
      return dimensions[1];
   }
   else if (dimensions.size() == 1) {
      return 1;
   }
   return 0; 
}
      
/**
 * get data offset.
 */
/*int64_t 
GiftiDataArray::getDataOffset(const int64_t nodeNum,
                                  const int64_t componentNum) const
{
   const int64_t off = nodeNum * dimensions[1] + componentNum;//TSC: this is WRONG! (assumes 2 dimensions, assumes a particular index order) fix it before uncommenting
   return off;
}//*/


/**
 * get the system's endian.
 */
GiftiEndianEnum::Enum 
GiftiDataArray::getSystemEndian()
{
    GiftiEndianEnum::Enum endian = GiftiEndianEnum::ENDIAN_BIG;
    if (ByteOrderEnum::isSystemLittleEndian()) {
        endian = GiftiEndianEnum::ENDIAN_LITTLE;
    }
   //if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
   //   return ENDIAN_BIG;
   //}
    return endian;
}
      
/**
 * get external file information.
 */
void 
GiftiDataArray::getExternalFileInformation(AString& nameOut,
                                           int64_t & offsetOut) const
{
   nameOut = externalFileName;
   offsetOut = externalFileOffset;
}
                                
/**
 * set external file information.
 */
void 
GiftiDataArray::setExternalFileInformation(const AString& nameIn,
                                           const int64_t offsetIn)
{
   externalFileName = nameIn;
   externalFileOffset = offsetIn;
}
                                      
/**
 * remap integer values that are indices to a table.
 *
void 
GiftiDataArray::remapIntValues(const std::vector<int32_t>& remappingTable)
{
   if (remappingTable.isEmpty()) {
      return;
   }
   if (dataType != NiftiDataType::NIFTI_TYPE_INT32) {
      return;
   }
   const int64_t num = getTotalNumberOfElements();
   for (int64_t i = 0; i < num; i++) {
      dataPointerInt[i] = remappingTable[dataPointerInt[i]];
   }
}
*/

/**
 * Update label indices using the index converter array.  Typically,
 * this is done when appending one label file to another.
 *
 * @param indexConverter  Converts the label indices to new values.
 */
void 
GiftiDataArray::transferLabelIndices(const std::map<int32_t,int32_t>& indexConverter) {
    if (this->getDataType() == NiftiDataTypeEnum::NIFTI_TYPE_INT32) {
        int64_t num = this->getTotalNumberOfElements();
        for (int i = 0; i < num; i++) {
            int oldIndex = this->dataPointerInt[i];
            std::map<int32_t,int32_t>::const_iterator iter = indexConverter.find(oldIndex);
            if (iter != indexConverter.end()) {
                this->dataPointerInt[i] = iter->second; //indexConverter.get(newIndex);
            }
            else {
                this->dataPointerInt[i] = 0;
            }
            this->setModified();
        }
    }
}

/**
 * read a GIFTI data array from text.
 * Data array should already be initialized and allocated.
 */
void 
GiftiDataArray::readFromText(const AString text,
                             const GiftiEndianEnum::Enum dataEndianForReading,
                             const GiftiArrayIndexingOrderEnum::Enum arraySubscriptingOrderForReading,
                             const NiftiDataTypeEnum::Enum dataTypeForReading,
                             const std::vector<int64_t>& dimensionsForReading,
                             const GiftiEncodingEnum::Enum encodingForReading,
                             const AString& externalFileNameForReading,
                             const int64_t externalFileOffsetForReading,
                             const bool isReadOnlyMetaData)
{
   const NiftiDataTypeEnum::Enum requiredDataType = dataType;
   dataType = dataTypeForReading;
   encoding = encodingForReading;
   endian   = dataEndianForReading;
   arraySubscriptingOrder = arraySubscriptingOrderForReading;
   setDimensions(dimensionsForReading);
   if (dimensionsForReading.size() == 0) {
      throw GiftiException("Data array has no dimensions.");
   }
   //setExternalFileInformation(externalFileNameForReading,
   //                           externalFileOffsetForReading);//TSC: don't set the external filename on the array, because that is what it uses when writing the array
                              
   //
   // If NOT metadata only
   //
   if (isReadOnlyMetaData == false) {
      //
      // Total number of elements in Data Array
      //
      const int64_t numElements = getTotalNumberOfElements();
      
      switch (encoding) {
          case GiftiEncodingEnum::ASCII:
            {
                std::istringstream stream(text.toStdString());
                
               switch (dataType) {
                  case NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32:
                     {
                        float* ptr = dataPointerFloat;
                        for (int64_t i = 0; i < numElements; i++) {
                           stream >> *ptr;
                           if (!stream) throw GiftiException("premature end of ascii-encoded gifti array");
                           ptr++;
                        }
                     }
                     break;
                   case NiftiDataTypeEnum::NIFTI_TYPE_INT32:
                     {
                        int32_t* ptr = dataPointerInt;
                        for (int64_t i = 0; i < numElements; i++) {
                           stream >> *ptr;
                           if (!stream) throw GiftiException("premature end of ascii-encoded gifti array");
                           ptr++;
                        }
                     }
                     break;
                   case NiftiDataTypeEnum::NIFTI_TYPE_UINT8:
                     {
                        uint8_t* ptr = dataPointerUByte;
                         int32_t c;
                        for (int64_t i = 0; i < numElements; i++) {
                           stream >> c;
                           if (!stream) throw GiftiException("premature end of ascii-encoded gifti array");
                           *ptr = static_cast<uint8_t>(c);
                           ptr++;
                        }
                     }
                     break;
                   default:
                       throw GiftiException("DataType " + NiftiDataTypeEnum::toName(dataType) + " not supported in GIFTI");
               }
            }
            break;
          case GiftiEncodingEnum::BASE64_BINARY:
            {
               //
               // Decode the Base64 data using VTK's algorithm
               //
               const uint64_t numDecoded =
                     Base64::decode((const unsigned char*)(text.toStdString().c_str()),
                                                data.size(),
                                                &data[0]);
               if (numDecoded != data.size()) {
                  std::ostringstream str;
                  str << "Decoding of Base64 Binary data failed.\n"
                   << "Decoded " << AString::number(numDecoded).toStdString() << " bytes but should be "
                      << AString::number(static_cast<int>(data.size())).toStdString() << " bytes.";
                  throw GiftiException(AString::fromStdString(str.str()));
               }
               
               //
               // Is byte swapping needed ?
               //
               if (endian != getSystemEndian()) {
                  byteSwapData(getSystemEndian());
               }
            }
            break;
          case GiftiEncodingEnum::GZIP_BASE64_BINARY:
            {
               //
               // Decode the Base64 data using VTK's algorithm
               //
               std::vector<unsigned char> dataBuffer(text.size() - text.size() / 4 + 10);//generous constant to make up for integer rounding
               //const char* textChars = text.toLatin1().constData();
               const uint64_t numDecoded =
                     Base64::decode((unsigned char*)text.toStdString().c_str(),
                                                dataBuffer.size(),
                                                dataBuffer.data());
               if (numDecoded == 0) {
                   std::ostringstream str;
                   str << "Decoding of GZip Base64 Binary data failed."
                   << "Decoded " << AString::number(numDecoded).toStdString() << " bytes but should be "
                   << AString::number(static_cast<int>(data.size())).toStdString() << " bytes.";
                   throw GiftiException(AString::fromStdString(str.str()));
               }
               
               
               //
               // Uncompress the data using VTK's algorithm
               // 
                DataCompressZLib compressor;
                const uint64_t uncompressedDataLength = 
                                   compressor.uncompressData(dataBuffer.data(),
                                                          numDecoded,
                                                          (unsigned char*)&data[0],
                                                          data.size());
               if (uncompressedDataLength != data.size()) {
                  std::ostringstream str;
                  str << "Decompression of Binary data failed.\n"
                   << "Uncompressed " << AString::number(uncompressedDataLength).toStdString() << " bytes but should be "
                   << AString::number(static_cast<uint64_t>(data.size())).toStdString() << " bytes.";
                  throw GiftiException(AString::fromStdString(str.str()));
               }
               
               //
               // Is byte swapping needed ? 
               //
               if (endian != getSystemEndian()) {
                  byteSwapData(getSystemEndian());
               }
            }
            break;
          case GiftiEncodingEnum::EXTERNAL_FILE_BINARY:
            {
               if (externalFileNameForReading.length() <= 0) {
                  throw GiftiException("External file name is empty.");
               }
               
                std::ifstream extBinFile(externalFileNameForReading.toStdString().c_str(),
                                         std::ifstream::in | std::ifstream::binary);
                if (extBinFile.good() == false) {
                        throw GiftiException("Error opening \""
                                            + externalFileNameForReading
                                            + "\"");
                }
                else {
                  //
                  // Move to the offset of the data
                  //
                  extBinFile.seekg(externalFileOffsetForReading, std::ios::beg);
                  if (extBinFile.good() == false) {
                        throw GiftiException("Error seeking to \""
                                             + AString::number(externalFileOffsetForReading)
                                             + "\" in \""
                                             + externalFileNameForReading
                                             + "\"");
                  }
                    
                  //
                  // Set the number of bytes that must be read
                  //
                    std::streamsize numberOfBytesToRead = 0;
                  char* pointerToForReadingData = NULL;
                  switch (dataType) {
                     case NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32:
                        numberOfBytesToRead = numElements * sizeof(float);
                        pointerToForReadingData = (char*)dataPointerFloat;
                        break;
                     case NiftiDataTypeEnum::NIFTI_TYPE_INT32:
                        numberOfBytesToRead = numElements * sizeof(int32_t);
                        pointerToForReadingData = (char*)dataPointerInt;
                        break;
                     case NiftiDataTypeEnum::NIFTI_TYPE_UINT8:
                        numberOfBytesToRead = numElements * sizeof(uint8_t);
                        pointerToForReadingData = (char*)dataPointerUByte;
                        break;
                      default:
                          throw GiftiException("DataType " + NiftiDataTypeEnum::toName(dataType) + " not supported in GIFTI");
                  }
               
                  //
                  // Read the data
                  //
                  extBinFile.read((char*)pointerToForReadingData,
                                    numberOfBytesToRead);
                  if(extBinFile.good() == false) {
                     throw GiftiException("Tried to read "
                                         + AString::number((int64_t)numberOfBytesToRead)
                                         + " from "
                                         + AString::number(externalFileOffsetForReading)
                                         + " but failed");
                  }
                  
                  //
                  // Is byte swapping needed ?
                  //
                  if (endian != getSystemEndian()) {
                     byteSwapData(getSystemEndian());
                  }
               }
            }
            break;
      }
   
      //
      // Check if data type needs to be converted
      //
      if (requiredDataType != dataType) {
          if (intent != NiftiIntentEnum::NIFTI_INTENT_POINTSET) {
            convertToDataType(requiredDataType);
         }
      }
      
       //
       // Are array indices in opposite order
       //
       if (arraySubscriptingOrderForReading == GiftiArrayIndexingOrderEnum::COLUMN_MAJOR_ORDER) {
           convertArrayIndexingOrder();
       }
   } // If NOT metadata only
   
   setModified();
}

/**
 * convert array indexing order of data.
 */
void
GiftiDataArray::convertArrayIndexingOrder()
{
    const int32_t numDim = static_cast<int32_t>(dimensions.size());

    if (numDim > 2) {
        throw GiftiException("Row/Column Major order conversion unavailable for arrays "
                             "with dimensions greater than two.");
    }

    //
    // Swap data
    //
    if (numDim == 2) {
        int32_t dimI = dimensions[0];
        int32_t dimJ = dimensions[1];

        /*
         * If a dimensions is "1", the data does not need to be transposed.
         */
        if ((dimI != 1) && (dimJ != 1)) {

            //
            // Is matrix square?
            //
            if (dimI == dimJ) {
                switch (dataType) {
                case NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32:
                {
                    for (int64_t i = 1; i < dimI; i++) {
                        for (int64_t j = 0; j < i; j++) {
                            const int64_t indexLowerLeft  = (i * dimJ) + j;
                            const int64_t indexUpperRight = (j * dimI) + i;
                            float temp = dataPointerFloat[indexLowerLeft];
                            dataPointerFloat[indexLowerLeft] = dataPointerFloat[indexUpperRight];
                            dataPointerFloat[indexUpperRight] = temp;
                        }
                    }
                }
                break;
                case NiftiDataTypeEnum::NIFTI_TYPE_INT32:
                {
                    for (int64_t i = 1; i < dimI; i++) {
                        for (int64_t j = 0; j < i; j++) {
                            const int64_t indexLowerLeft  = (i * dimJ) + j;
                            const int64_t indexUpperRight = (j * dimI) + i;
                            const int32_t temp = dataPointerInt[indexLowerLeft];
                            dataPointerInt[indexLowerLeft] = dataPointerInt[indexUpperRight];
                            dataPointerInt[indexUpperRight] = temp;
                        }
                    }
                }
                break;
                case NiftiDataTypeEnum::NIFTI_TYPE_UINT8:
                {
                    for (int64_t i = 1; i < dimI; i++) {
                        for (int64_t j = 0; j < i; j++) {
                            const int64_t indexLowerLeft  = (i * dimJ) + j;
                            const int64_t indexUpperRight = (j * dimI) + i;
                            const uint8_t temp = dataPointerUByte[indexLowerLeft];
                            dataPointerUByte[indexLowerLeft] = dataPointerUByte[indexUpperRight];
                            dataPointerUByte[indexUpperRight] = temp;
                        }
                    }
                }
                break;
                default:
                    throw GiftiException("DataType " + NiftiDataTypeEnum::toName(dataType) + " not supported in GIFTI");
                    break;
                }
            } else {
                //
                // Copy the data
                //
                std::vector<uint8_t> dataCopy = data;

                switch (arraySubscriptingOrder)
                {
                case GiftiArrayIndexingOrderEnum::ROW_MAJOR_ORDER:
                    switch (dataType) {
                    case NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32:
                    {
                        float* ptr = (float*)&(dataCopy[0]);
                        for (int64_t i = 0; i < dimI; i++) {
                            for (int64_t j = 0; j < dimJ; j++) {
                                const int64_t indx = (j * dimI) + i;
                                const int64_t ptrIndex = (i * dimJ) + j;
                                dataPointerFloat[indx] = ptr[ptrIndex];
                            }
                        }
                    }
                    break;
                    case NiftiDataTypeEnum::NIFTI_TYPE_INT32:
                    {
                        uint32_t* ptr = (uint32_t*)&(dataCopy[0]);
                        for (int64_t i = 0; i < dimI; i++) {
                            for (int64_t j = 0; j < dimJ; j++) {
                                const int64_t indx = (j * dimI) + i;
                                const int64_t ptrIndex = (i * dimJ) + j;
                                dataPointerInt[indx] = ptr[ptrIndex];
                            }
                        }
                    }
                    break;
                    case NiftiDataTypeEnum::NIFTI_TYPE_UINT8:
                    {
                        uint8_t* ptr = (uint8_t*)&(dataCopy[0]);
                        for (int64_t i = 0; i < dimI; i++) {
                            for (int64_t j = 0; j < dimJ; j++) {
                                const int64_t indx = (j * dimI) + i;
                                const int64_t ptrIndex = (i * dimJ) + j;
                                dataPointerUByte[indx] = ptr[ptrIndex];
                            }
                        }
                    }
                    break;
                    default:
                        throw GiftiException("DataType " + NiftiDataTypeEnum::toName(dataType) + " not supported in GIFTI");
                        break;
                    }
                    break;
                case GiftiArrayIndexingOrderEnum::COLUMN_MAJOR_ORDER:
                    switch (dataType) {
                    case NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32:
                    {
                        float* ptr = (float*)&(dataCopy[0]);
                        for (int64_t i = 0; i < dimI; i++) {
                            for (int64_t j = 0; j < dimJ; j++) {
                                const int64_t indx = (i * dimJ) + j;
                                const int64_t ptrIndex = (j * dimI) + i;
                                dataPointerFloat[indx] = ptr[ptrIndex];
                            }
                        }
                    }
                    break;
                    case NiftiDataTypeEnum::NIFTI_TYPE_INT32:
                    {
                        uint32_t* ptr = (uint32_t*)&(dataCopy[0]);
                        for (int64_t i = 0; i < dimI; i++) {
                            for (int64_t j = 0; j < dimJ; j++) {
                                const int64_t indx = (i * dimJ) + j;
                                const int64_t ptrIndex = (j * dimI) + i;
                                dataPointerInt[indx] = ptr[ptrIndex];
                            }
                        }
                    }
                    break;
                    case NiftiDataTypeEnum::NIFTI_TYPE_UINT8:
                    {
                        uint8_t* ptr = (uint8_t*)&(dataCopy[0]);
                        for (int64_t i = 0; i < dimI; i++) {
                            for (int64_t j = 0; j < dimJ; j++) {
                                const int64_t indx = (i * dimJ) + j;
                                const int64_t ptrIndex = (j * dimI) + i;
                                dataPointerUByte[indx] = ptr[ptrIndex];
                            }
                        }
                    }
                    break;
                    default:
                        throw GiftiException("DataType " + NiftiDataTypeEnum::toName(dataType) + " not supported in GIFTI");
                        break;
                    }
                    break;
                }

            }
        }
    }
    switch (arraySubscriptingOrder)
    {
    case GiftiArrayIndexingOrderEnum::COLUMN_MAJOR_ORDER:
        arraySubscriptingOrder = GiftiArrayIndexingOrderEnum::ROW_MAJOR_ORDER;
        break;
    case GiftiArrayIndexingOrderEnum::ROW_MAJOR_ORDER:
        arraySubscriptingOrder = GiftiArrayIndexingOrderEnum::COLUMN_MAJOR_ORDER;
        break;
    }
}

/**
 * write the data as XML.
 * @param stream
 *    Stream for XML.
 * @param externalBinaryOutputStream
 *    Stream for external binary file.
 * @param encodingForWriting
 *    GIFTI encoding used when writing the data.
 */
void 
GiftiDataArray::writeAsXML(std::ostream& stream, 
                           std::ostream* externalBinaryOutputStream,
                           GiftiEncodingEnum::Enum encodingForWriting) 
                                               
{
    this->encoding = encodingForWriting;
    
    //
    // Do not write if data array is isEmpty()
    //
    const int64_t numRows = this->dimensions[0];
    if (numRows <= 0) {
        return;
    }
    
    XmlWriter xmlWriter(stream);
   //
   // Clean up the dimensions by removing any "last" dimensions that
   // are one with the exception of the first dimension
   //   e.g.:   dimension = [73730, 1]  becomes [73730]
   //
   int64_t dimensionality = static_cast<int64_t>(dimensions.size());
   for (int64_t i = (dimensionality - 1); i >= 1; i--) {
      if (dimensions[i] <= 1) {
         dimensions.resize(i);
      }
   }
   dimensionality = static_cast<int64_t>(dimensions.size());
    
   /*
    * Push the palette color mapping into the metadata.
    */
    if (this->paletteColorMapping != NULL) {
        const AString paletteXML = this->paletteColorMapping->encodeInXML();
        this->getMetaData()->set(GiftiMetaDataXmlElements::METADATA_NAME_PALETTE_COLOR_MAPPING,
                                 paletteXML);
    }
    
   //
   // External file not supported
   //
   //const AString externalFileName = "";
   //const AString externalFileOffset = "0";
   
   //
   // Write the opening tag
   //
    XmlAttributes dataAtt;
    dataAtt.addAttribute(GiftiXmlElements::ATTRIBUTE_DATA_ARRAY_INTENT, NiftiIntentEnum::toName(this->intent));
    dataAtt.addAttribute(GiftiXmlElements::ATTRIBUTE_DATA_ARRAY_DATA_TYPE, NiftiDataTypeEnum::toName(this->dataType));
    dataAtt.addAttribute(GiftiXmlElements::ATTRIBUTE_DATA_ARRAY_INDEXING_ORDER, GiftiArrayIndexingOrderEnum::toGiftiName(this->arraySubscriptingOrder));
    dataAtt.addAttribute(GiftiXmlElements::ATTRIBUTE_DATA_ARRAY_DIMENSIONALITY, dimensionality);
    for (int64_t i = 0; i < dimensionality; i++) {
        const AString dimName = GiftiXmlElements::ATTRIBUTE_DATA_ARRAY_DIM_PREFIX + AString::number(i);
        dataAtt.addAttribute(dimName, this->dimensions[i]);
    }
    dataAtt.addAttribute(GiftiXmlElements::ATTRIBUTE_DATA_ARRAY_ENCODING, GiftiEncodingEnum::toGiftiName(this->encoding));
    dataAtt.addAttribute(GiftiXmlElements::ATTRIBUTE_DATA_ARRAY_ENDIAN, GiftiEndianEnum::toGiftiName(this->endian));
    dataAtt.addAttribute(GiftiXmlElements::ATTRIBUTE_DATA_ARRAY_EXTERNAL_FILE_NAME, externalFileName);
    dataAtt.addAttribute(GiftiXmlElements::ATTRIBUTE_DATA_ARRAY_EXTERNAL_FILE_OFFSET, externalFileOffset);

    
    
    xmlWriter.writeStartElement(GiftiXmlElements::TAG_DATA_ARRAY, dataAtt);
    
   
   //
   // Write the metadata
   //
    this->metaData.writeAsXML(xmlWriter);
   
    /*
     * Write one identity matrix for the transformation matrix. 
     *
     * We do not want to write the transformation matrices that
     * were read from the file as the first matrix was applied to
     * the coordinates when the file was read.  To do so would require 
     * applying an inverse of the matrix.  However, the user may have 
     * modified the coordinates so the matrix is no longer valid for 
     * the file's coordinates.
     */
    if (getIntent() == NiftiIntentEnum::NIFTI_INTENT_POINTSET) {
        Matrix4x4 identityMatrix;
        identityMatrix.identity();
        identityMatrix.setDataSpaceName(NiftiTransformEnum::toName(NiftiTransformEnum::NIFTI_XFORM_TALAIRACH));
        identityMatrix.setTransformedSpaceName(NiftiTransformEnum::toName(NiftiTransformEnum::NIFTI_XFORM_TALAIRACH));
        identityMatrix.writeAsGiftiXML(xmlWriter,
                                       GiftiXmlElements::TAG_COORDINATE_TRANSFORMATION_MATRIX,
                                       GiftiXmlElements::TAG_MATRIX_DATA_SPACE,
                                       GiftiXmlElements::TAG_MATRIX_TRANSFORMED_SPACE,
                                       GiftiXmlElements::TAG_MATRIX_DATA);
    }

   //
   // NOTE: for the base64 and ZLIB-Base64 data, it is important that there are
   // no spaces between the <DATA> and </DATA> tags.
   //
   switch (encoding) {
       case GiftiEncodingEnum::ASCII:
         {
             //
             // Write the start element.
             //
             xmlWriter.writeStartElement(GiftiXmlElements::TAG_DATA);
             
            //
            // Newline after <DATA> tag (only do this for ASCII !!!)
            //
            //stream << "\n";
            
            //
            // determine the number of items per row (node)
            //
            int64_t numItemsPerRow = 1;
            for (uint32_t i = 1; i < dimensions.size(); i++) {
               numItemsPerRow *= dimensions[i];
            }
            
            //
            // Write the rows
            //
            int32_t offset = 0;
            for (int32_t i = 0; i < numRows; i++) {
                xmlWriter.writeCharacters("      ");
               switch (dataType) {
                  case NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32:
                     for (int64_t j = 0; j < numItemsPerRow; j++) {
                         xmlWriter.writeCharacters(AString::number(this->dataPointerFloat[offset + j]));
                         xmlWriter.writeCharacters(" ");                         
                     }
                     break;
                  case NiftiDataTypeEnum::NIFTI_TYPE_INT32:
                     for (int64_t j = 0; j < numItemsPerRow; j++) {
                         xmlWriter.writeCharacters(AString::number(this->dataPointerInt[offset + j]));
                         xmlWriter.writeCharacters(" ");                         
                     }
                     break;
                  case NiftiDataTypeEnum::NIFTI_TYPE_UINT8:
                     for (int64_t j = 0; j < numItemsPerRow; j++) {
                         xmlWriter.writeCharacters(AString::number(this->dataPointerUByte[offset + j]));
                         xmlWriter.writeCharacters(" ");                         
                     }
                     break;
                   default:
                       throw GiftiException("DataType " + NiftiDataTypeEnum::toName(dataType) + " not supported in GIFTI");
                       break;
               }
               xmlWriter.writeCharacters("\n");
               offset += numItemsPerRow;
            }
             
             //
             // Write the closing Data tag
             //
             xmlWriter.writeEndElement();
             
         }
         break;
       case GiftiEncodingEnum::BASE64_BINARY:
         {
            //
            // Encode the data with VTK's Base64 algorithm
            //
            const uint64_t bufferLength = static_cast<uint64_t>(data.size() * 1.5);
            char* buffer = new char[bufferLength];
            const uint64_t compressedLength =
               Base64::encode(&data[0],
                                          data.size(),
                                          (unsigned char*)buffer);
            if (compressedLength >= bufferLength) {
               throw GiftiException(
                     "Base64 encoding buffer length ("
                     + AString::number(bufferLength)
                     + ") is too small but needs to be "
                                    + AString::number(compressedLength));
            }
            buffer[compressedLength] = '\0';
            
            //
            // Write the data  MUST BE NO space around data
            //
            xmlWriter.writeElementNoSpace(GiftiXmlElements::TAG_DATA, buffer);
            
            //
            // Free memory
            //
            delete[] buffer;
         }
         break;
       case GiftiEncodingEnum::GZIP_BASE64_BINARY:
         {
            //
            // Compress the data with VTK's ZLIB algorithm
            //
             DataCompressZLib compressor;
             uint64_t compressedDataBufferLength =
                              compressor.getMaximumCompressionSpace(data.size());
            std::vector<unsigned char> compressedDataBuffer(compressedDataBufferLength);
            uint64_t compressedDataLength =
                          compressor.compressData(&data[0], 
                                               data.size(),
                                               compressedDataBuffer.data(),
                                               compressedDataBufferLength);
            
            //
            // Encode the data with VTK's Base64 algorithm
            //
            std::vector<unsigned char> buffer(compressedDataLength + compressedDataLength / 3 + 10);//generous constant for partial bytes, integer rounding, and possible "=" formatting
            const uint64_t compressedLength =
               Base64::encode(compressedDataBuffer.data(),
                                          compressedDataLength,
                                          buffer.data());
            buffer[compressedLength] = '\0';
            
             //
             // Write the data  MUST BE NO space around data
             //
             xmlWriter.writeElementNoSpace(GiftiXmlElements::TAG_DATA, (char*)buffer.data());
             
         }
         break;
       case GiftiEncodingEnum::EXTERNAL_FILE_BINARY:
         {
            const int64_t dataLength = data.size();
            externalBinaryOutputStream->write((const char*)&data[0], dataLength);
            if (externalBinaryOutputStream->bad()) {
               throw GiftiException("Output stream for external file reports its status as bad.");
            }
            //
            // Write the empty data
            //
            xmlWriter.writeElementNoSpace(GiftiXmlElements::TAG_DATA, "");
             
         }
         break;
   }
   
   //
   // write the closing data array tag
   //
   xmlWriter.writeEndElement();
}                      

/**
 * convert to data type.
 */
void 
GiftiDataArray::convertToDataType(const NiftiDataTypeEnum::Enum newDataType)
{
   if (newDataType != dataType) {      
      //
      // make a copy of myself
      //
      GiftiDataArray copyOfMe(*this);
      
      //
      // Set my new data type and reallocate memory
      //
      const NiftiDataTypeEnum::Enum oldDataType = dataType;
      dataType = newDataType;
      allocateData();
      
      if (data.empty() == false) {
         //
         //  Get total number of elements
         //
         int64_t numElements = 1;
         for (uint32_t i = 0; i < dimensions.size(); i++) {
            numElements *= dimensions[i];
         }
         
         //
         // copy the data
         //
         for (int64_t i = 0; i < numElements; i++) {
            switch (dataType) {
               case NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32:
                  switch (oldDataType) {
                     case NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32:
                        dataPointerFloat[i] = static_cast<float>(copyOfMe.dataPointerFloat[i]);
                        break;
                     case NiftiDataTypeEnum::NIFTI_TYPE_INT32:
                        dataPointerFloat[i] = static_cast<float>(copyOfMe.dataPointerInt[i]);
                        break;
                     case NiftiDataTypeEnum::NIFTI_TYPE_UINT8:
                        dataPointerFloat[i] = static_cast<float>(copyOfMe.dataPointerUByte[i]);
                        break;
                      default:
                          throw GiftiException("DataType " + NiftiDataTypeEnum::toName(oldDataType) + " not supported in GIFTI");
                          break;
                  }
                  break;
               case NiftiDataTypeEnum::NIFTI_TYPE_INT32:
                  switch (oldDataType) {
                     case NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32:
                        dataPointerInt[i] = static_cast<int32_t>(copyOfMe.dataPointerFloat[i]);
                        break;
                     case NiftiDataTypeEnum::NIFTI_TYPE_INT32:
                        dataPointerInt[i] = static_cast<int32_t>(copyOfMe.dataPointerInt[i]);
                        break;
                     case NiftiDataTypeEnum::NIFTI_TYPE_UINT8:
                        dataPointerInt[i] = static_cast<int32_t>(copyOfMe.dataPointerUByte[i]);
                        break;
                      default:
                          throw GiftiException("DataType " + NiftiDataTypeEnum::toName(oldDataType) + " not supported in GIFTI");
                          break;
                  }
                  break;
               case NiftiDataTypeEnum::NIFTI_TYPE_UINT8:
                  switch (oldDataType) {
                     case NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32:
                        dataPointerUByte[i] = static_cast<uint8_t>(copyOfMe.dataPointerFloat[i]);
                        break;
                     case NiftiDataTypeEnum::NIFTI_TYPE_INT32:
                        dataPointerUByte[i] = static_cast<uint8_t>(copyOfMe.dataPointerInt[i]);
                        break;
                     case NiftiDataTypeEnum::NIFTI_TYPE_UINT8:
                        dataPointerUByte[i] = static_cast<uint8_t>(copyOfMe.dataPointerUByte[i]);
                        break;
                      default:
                          throw GiftiException("DataType " + NiftiDataTypeEnum::toName(oldDataType) + " not supported in GIFTI");
                          break;
                  }
                  break;
                default:
                    throw GiftiException("DataType " + NiftiDataTypeEnum::toName(dataType) + " not supported in GIFTI");
                    break;
            }
         }
      }
   }
   
   setModified();
}      

/**
 * byte swap the data (data read is different endian than this system).
 */
void 
GiftiDataArray::byteSwapData(const GiftiEndianEnum::Enum newEndian)
{
   endian = newEndian;
   switch (dataType) {
      case NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32:
         ByteSwapping::swapBytes(dataPointerFloat, getTotalNumberOfElements());
         break;
      case NiftiDataTypeEnum::NIFTI_TYPE_INT32:
         ByteSwapping::swapBytes(dataPointerInt, getTotalNumberOfElements());
         break;
      case NiftiDataTypeEnum::NIFTI_TYPE_UINT8:
         // should not need to swap ??
         break;
       default:
           CaretAssertMessage(0, "Unsupported GIFTI data Type");
           break;
   }
}      

/**
 * Set this object has been modified.
 *
 */
void
GiftiDataArray::setModified()
{
    this->modifiedFlag = true;
    if (this->descriptiveStatistics != NULL) {
        delete this->descriptiveStatistics;
        this->descriptiveStatistics = NULL;
    }
    if (this->descriptiveStatisticsLimitedValues != NULL) {
        delete this->descriptiveStatisticsLimitedValues;
        this->descriptiveStatisticsLimitedValues = NULL;
    }
}

/**
 * Set this object as not modified.  Object should also
 * clear the modification status of its children.
 *
 */
void
GiftiDataArray::clearModified()
{
    this->modifiedFlag = false;
    if (this->paletteColorMapping != NULL) {
        this->paletteColorMapping->clearModified();
    }
}

/**
 * Get the modification status.  Returns true if this object or
 * any of its children have been modified.  
 *
 * DOES NOT include palette color mapping modification status.
 *
 * @return - The modification status.
 *
 */
bool
GiftiDataArray::isModified() const
{
//    if (this->paletteColorMapping != NULL) {
//        if (this->paletteColorMapping->isModified()) {
//            return true;
//        }
//    }
    return this->modifiedFlag;
}


/** 
 * get minimum and maximum values (valid for int data only).
 */
void 
GiftiDataArray::getMinMaxValues(int& minValue, int& maxValue) const
{
   if (minMaxIntValuesValid == false) {
      minValueInt = std::numeric_limits<int32_t>::max();
      minValueInt = std::numeric_limits<int32_t>::min();
      
      int64_t numItems = getTotalNumberOfElements();
      for (int64_t i = 0; i < numItems; i++) {
         minValueInt = std::min(minValueInt, dataPointerInt[i]);
         maxValueInt = std::max(maxValueInt, dataPointerInt[i]);
      }
      minMaxIntValuesValid = true;
   }
   minValue = minValueInt;
   maxValue = maxValueInt;
}

/**
 * Get the minimum and maximum values for float data.
 *
 * @param minValue
 *     Output minimum value.
 * @param maxValue
 *     Output maximum value.
 */
void
GiftiDataArray::getMinMaxValuesFloat(float& minValue,
                          float& maxValue) const
{
    if (minMaxFloatValuesValid == false) {
        minValueFloat =  std::numeric_limits<float>::max();
        maxValueFloat = -std::numeric_limits<float>::max();
        
        int64_t numItems = getTotalNumberOfElements();
        for (int64_t i = 0; i < numItems; i++) {
            minValueFloat = std::min(minValueFloat, dataPointerFloat[i]);
            maxValueFloat = std::max(maxValueFloat, dataPointerFloat[i]);
        }
        minMaxFloatValuesValid = true;
    }
    
    minValue = minValueFloat;
    maxValue = maxValueFloat;
}

/**
 * set all elements of array to zero.
 */
void 
GiftiDataArray::zeroize()
{
   if (data.empty() == false) {
      std::fill(data.begin(), data.end(), 0);
   }
   metaData.clear();
   nonWrittenMetaData.clear();
}

/**
 * get an offset for indices into data (dimensionality of indices must be same as data).
 */
int64_t 
GiftiDataArray::getDataOffset(const int32_t indices[]) const
{
   const int32_t numDim = static_cast<int32_t>(dimensions.size());
   
   int64_t offset = 0;
   int64_t dimProduct = 1;
   switch (this->arraySubscriptingOrder) {
       case GiftiArrayIndexingOrderEnum::ROW_MAJOR_ORDER:
         for (int32_t d = (numDim - 1); d >= 0; d--) {
            offset += indices[d] * dimProduct;
            dimProduct *= dimensions[d];
         }
         break;
      case GiftiArrayIndexingOrderEnum::COLUMN_MAJOR_ORDER:  // correct???
         for (int32_t d = 0; d <= (numDim - 1); d++) {
            offset += indices[d] * dimProduct;
            dimProduct *= dimensions[d];
         }
         break;
   }
   
   return offset;
}

/**
 * get a float value (data type must be float and dimensionality of indices must be same as data).
 */
float 
GiftiDataArray::getDataFloat32(const int32_t indices[]) const
{
   const int64_t offset = getDataOffset(indices);
   return dataPointerFloat[offset];
}

/**
 * get a float value pointer(data type must be float and dimensionality of indices must be same as data).
 */
const float* 
GiftiDataArray::getDataFloat32Pointer(const int32_t indices[]) const
{
   const int64_t offset = getDataOffset(indices);
   return &dataPointerFloat[offset];
}

/**
 * get an int value (data type must be int and dimensionality of indices must be same as data).
 */
int32_t 
GiftiDataArray::getDataInt32(const int32_t indices[]) const
{
   const int64_t offset = getDataOffset(indices);
   return dataPointerInt[offset];
}

/**
 * get an int value pointer (data type must be int and dimensionality of indices must be same as data).
 */
const int32_t* 
GiftiDataArray::getDataInt32Pointer(const int32_t indices[]) const
{
   const int64_t offset = getDataOffset(indices);
   return &dataPointerInt[offset];
}

/**
 * get a byte value (data type must be unsigned char and dimensionality of indices must be same as data).
 */
uint8_t 
GiftiDataArray::getDataUInt8(const int32_t indices[]) const
{
   const int64_t offset = getDataOffset(indices);
   return dataPointerUByte[offset];
}

/**
 * get a byte value pointer(data type must be unsigned char and dimensionality of indices must be same as data).
 */
const uint8_t*
GiftiDataArray::getDataUInt8Pointer(const int32_t indices[]) const
{
   const int64_t offset = getDataOffset(indices);
   return &dataPointerUByte[offset];
}

/**
 * set a float value (data type must be float and dimensionality of indices must be same as data).
 */
void 
GiftiDataArray::setDataFloat32(const int32_t indices[], const float dataValue) const
{
   const int64_t offset = getDataOffset(indices);
   dataPointerFloat[offset] = dataValue;
}

/**
 * set an int value (data type must be int and dimensionality of indices must be same as data).
 */
void 
GiftiDataArray::setDataInt32(const int32_t indices[], const int32_t dataValue) const
{
   const int64_t offset = getDataOffset(indices);
   dataPointerInt[offset] = dataValue;
}

/**
 * set a byte value (data type must be unsigned char and dimensionality of indices must be same as data).
 */
void 
GiftiDataArray::setDataUInt8(const int32_t indices[], const uint8_t dataValue) const
{
   const int64_t offset = getDataOffset(indices);
   dataPointerUByte[offset] = dataValue;
}      

/**
 * valid intent name.
 */
bool 
GiftiDataArray::intentNameValid(const AString& intentNameIn)
{
    bool valid = false;
    NiftiIntentEnum::fromName(intentNameIn, &valid);
    return valid;
}      

/**
 * remove all matrices.
 */
void 
GiftiDataArray::removeAllMatrices()
{
   matrices.clear();
   setModified();
}
      
/**
 * remove a matrix.
 */
void 
GiftiDataArray::removeMatrix(const int32_t indx)
{
   matrices.erase(matrices.begin() + indx);
   setModified();
}

/** 
 * Get the color palette mapping.
 *
 * @return
 *   The palette color mapping.
 */;
PaletteColorMapping* 
GiftiDataArray::getPaletteColorMapping()
{
    if (this->paletteColorMapping == NULL) {
        this->paletteColorMapping = new PaletteColorMapping();
        const AString paletteString = this->getMetaData()->get(GiftiMetaDataXmlElements::METADATA_NAME_PALETTE_COLOR_MAPPING);
        if (paletteString.isEmpty() == false) {
            try {
                this->paletteColorMapping->decodeFromStringXML(paletteString);
            }
            catch (const XmlException& e) {
                this->paletteColorMapping = new PaletteColorMapping();
                CaretLogSevere("Failed to parse Palette XML: " + e.whatString());
            }
        }
        
        this->paletteColorMapping->clearModified();
    }
    
    return this->paletteColorMapping;
}

/** 
 * Get the color palette mapping.
 *
 * @return
 *   The palette color mapping.
 */;
const PaletteColorMapping* 
GiftiDataArray::getPaletteColorMapping() const
{
    if (this->paletteColorMapping == NULL) {
        this->paletteColorMapping = new PaletteColorMapping();
        const AString paletteString = this->getMetaData()->get(GiftiMetaDataXmlElements::METADATA_NAME_PALETTE_COLOR_MAPPING);
        if (paletteString.isEmpty() == false) {
            try {
                this->paletteColorMapping->decodeFromStringXML(paletteString);
            }
            catch (const XmlException& e) {
                this->paletteColorMapping = new PaletteColorMapping();
                CaretLogSevere("Failed to parse Palette XML: " + e.whatString());
            }
        }
        
        this->paletteColorMapping->clearModified();
    }
    
    return this->paletteColorMapping;
}

/**
 * Get the descriptive statistics for this data array.
 *
 * @return Descriptive statistics.
 */
const DescriptiveStatistics* 
GiftiDataArray::getDescriptiveStatistics() const
{
    if (this->getDataType() == NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32) {
        if (this->descriptiveStatistics == NULL) {
            this->descriptiveStatistics = new DescriptiveStatistics();
        }
        this->descriptiveStatistics->update(this->dataPointerFloat,
                                            this->getTotalNumberOfElements());
    }
    return this->descriptiveStatistics;
}

const FastStatistics* GiftiDataArray::getFastStatistics() const
{
    if (this->getDataType() == NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32) {
        if (m_fastStatistics == NULL) {
            m_fastStatistics.grabNew(new FastStatistics());
        }
        m_fastStatistics->update(dataPointerFloat, getTotalNumberOfElements());
    }
    return m_fastStatistics;
}

/**
 * Invalidate the histograms
 */
void
GiftiDataArray::invalidateHistograms()
{
    m_histogramNeedsUpdateFlag = true;
    m_histogramLimitedValuesNeedsUpdateFlag = true;
}

const Histogram* GiftiDataArray::getHistogram(const int32_t numberOfBuckets) const
{
    if (this->getDataType() == NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32) {
        bool updateHistogramFlag = m_histogramNeedsUpdateFlag;
        m_histogramNeedsUpdateFlag = false;
        if (m_histogram == NULL) {
            m_histogram.grabNew(new Histogram(numberOfBuckets));
            updateHistogramFlag = true;
        }
        else if (numberOfBuckets != m_histogramNumberOfBuckets) {
            updateHistogramFlag = true;
        }
        if (updateHistogramFlag) {
            m_histogram->update(numberOfBuckets, dataPointerFloat, getTotalNumberOfElements());
            m_histogramNumberOfBuckets = numberOfBuckets;
        }
    }
    return m_histogram;
}

/**
 * Get the descriptive statistics for this data array limited
 * to values within the given ranges.
 *
 * @param mostPositiveValueInclusive
 *    Values more positive than this value are excluded.
 * @param leastPositiveValueInclusive
 *    Values less positive than this value are excluded.
 * @param leastNegativeValueInclusive
 *    Values less negative than this value are excluded.
 * @param mostNegativeValueInclusive
 *    Values more negative than this value are excluded.
 * @param includeZeroValues
 *    If true zero values (very near zero) are included.
 * @return Descriptive statistics.
 */
const DescriptiveStatistics* 
GiftiDataArray::getDescriptiveStatistics(const float mostPositiveValueInclusive,
                                                      const float leastPositiveValueInclusive,
                                                      const float leastNegativeValueInclusive,
                                                      const float mostNegativeValueInclusive,
                                                      const bool includeZeroValues) const
{
    if (this->getDataType() == NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32) {
        if (this->descriptiveStatisticsLimitedValues == NULL) {
            this->descriptiveStatisticsLimitedValues = new DescriptiveStatistics();
        }
        this->descriptiveStatisticsLimitedValues->update(this->dataPointerFloat,
                                                         this->getTotalNumberOfElements(),
                                                         mostPositiveValueInclusive,
                                                         leastPositiveValueInclusive,
                                                         leastNegativeValueInclusive,
                                                         mostNegativeValueInclusive,
                                                         includeZeroValues);
    }
    return this->descriptiveStatisticsLimitedValues;
}

const Histogram* GiftiDataArray::getHistogram(const int32_t numberOfBuckets,
                                              const float mostPositiveValueInclusive,
                                              const float leastPositiveValueInclusive,
                                              const float leastNegativeValueInclusive,
                                              const float mostNegativeValueInclusive,
                                              const bool includeZeroValues) const
{
    if (this->getDataType() == NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32) {
        bool updateHistogramFlag = m_histogramLimitedValuesNeedsUpdateFlag;
        m_histogramLimitedValuesNeedsUpdateFlag = false;
        if (m_histogramLimitedValues == NULL)
        {
            m_histogramLimitedValues.grabNew(new Histogram(numberOfBuckets));
            updateHistogramFlag = true;
        }
        else if ((numberOfBuckets != m_histogramLimitedValuesNumberOfBuckets)
                 || (mostPositiveValueInclusive != m_histogramLimitedValuesMostPositiveValueInclusive)
                 || (leastPositiveValueInclusive != m_histogramLimitedValuesLeastPositiveValueInclusive)
                 || (leastNegativeValueInclusive != m_histogramLimitedValuesLeastNegativeValueInclusive)
                 || (mostNegativeValueInclusive != m_histogramLimitedValuesMostNegativeValueInclusive)
                 || (includeZeroValues != m_histogramLimitedValuesIncludeZeroValues)) {
            updateHistogramFlag = true;
        }
        if (updateHistogramFlag) {
            m_histogramLimitedValues->update(numberOfBuckets,
                                             dataPointerFloat, getTotalNumberOfElements(),
                                             mostPositiveValueInclusive,
                                             leastPositiveValueInclusive,
                                             leastNegativeValueInclusive,
                                             mostNegativeValueInclusive,
                                             includeZeroValues);
            m_histogramLimitedValuesNumberOfBuckets = numberOfBuckets;
            m_histogramLimitedValuesMostPositiveValueInclusive = mostPositiveValueInclusive;
            m_histogramLimitedValuesLeastPositiveValueInclusive = leastPositiveValueInclusive;
            m_histogramLimitedValuesLeastNegativeValueInclusive = leastNegativeValueInclusive;
            m_histogramLimitedValuesMostNegativeValueInclusive = mostNegativeValueInclusive;
            m_histogramLimitedValuesIncludeZeroValues = includeZeroValues;
        }
    }
    return m_histogramLimitedValues;
}

AString 
GiftiDataArray::toString() const
{
    std::ostringstream str;
    str << "Data Array" << std::endl;
    str << "   DataType=" << NiftiDataTypeEnum::toName(this->dataType).toStdString() << std::endl;
    str << "   Intent=" << NiftiIntentEnum::toName(this->intent).toStdString() << std::endl;
    str << "   Dimensions=" << AString::fromNumbers(this->dimensions, ",").toStdString();
    str << "   MetaData=" << this->metaData.toString().toStdString() << std::endl;
    return AString::fromStdString(str.str());
}

void GiftiDataArray::validateArrayAfterReading()
{
    //pointset arrays are mandated to have at least one tranfsormation matrix, for some unknown reason
    if (intent == NiftiIntentEnum::NIFTI_INTENT_POINTSET && matrices.size() == 0)
    {
        CaretLogWarning("pointset gifti array did not include a transformation matrix, adding identity transform");
        Matrix4x4 gm;
        gm.setDataSpaceName("NIFTI_XFORM_TALAIRACH");
        gm.setTransformedSpaceName("NIFTI_XFORM_TALAIRACH");
        matrices.push_back(gm);
    }
}
