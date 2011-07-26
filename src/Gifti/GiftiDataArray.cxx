/*LICENSE_START*/
/*
 *  Copyright 1995-2002 Washington University School of Medicine
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

#include <algorithm>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <ostream>
#include <limits>
#include <sstream>

#include "Base64.h"
#include "ByteOrder.h"
#include "ByteSwapping.h"
#include "DataCompressZLib.h"

//#include "FileUtilities.h"
#include "GiftiDataArray.h"
#include "GiftiFile.h"
#include "GiftiMetaDataXmlElements.h"
#include "GiftiXmlElements.h"
#include "NiftiTransform.h"
#include "StringUtilities.h"
#include "SystemUtilities.h"
#include "XmlWriter.h"

using namespace caret;

/**
 * constructor.
 */
GiftiDataArray::GiftiDataArray(GiftiFile* parentGiftiFileIn,
                               const NiftiIntent::Enum intentIn,
                               const NiftiDataType::Enum dataTypeIn,
                               const std::vector<int64_t>& dimensionsIn,
                               const GiftiEncoding::Enum encodingIn)
{
   parentGiftiFile = parentGiftiFileIn;
   intent = intentIn;
   dataTypeSize = 0;
   dataPointerFloat = NULL;
   dataPointerInt = NULL;
   dataPointerUByte = NULL;
   clear();
   dataType = dataTypeIn;
   setDimensions(dimensionsIn);
   encoding = encodingIn;
   endian = getSystemEndian();
    arraySubscriptingOrder = GiftiArrayIndexingOrder::ROW_MAJOR_ORDER;
   externalFileName = "";
   externalFileOffset = 0;
   
    if (intent == NiftiIntent::NIFTI_INTENT_POINTSET) {   
      Matrix4x4 gm;
      matrices.push_back(gm);
   }
   
   //getDataTypeAppropriateForIntent(intent, dataType);
   metaData.set(GiftiMetaDataXmlElements::METADATA_NAME_UNIQUE_ID, 
                SystemUtilities::createUniqueID());
                
}

/**
 * constructor.
 */
GiftiDataArray::GiftiDataArray(GiftiFile* parentGiftiFileIn,
                               const NiftiIntent::Enum intentIn)
{
   parentGiftiFile = parentGiftiFileIn;
   intent = intentIn;
   dataTypeSize = 0;
   dataPointerFloat = NULL;
   dataPointerInt = NULL;
   dataPointerUByte = NULL;
   clear();
   dimensions.clear();
   encoding = GiftiEncoding::ASCII;
   endian = getSystemEndian();
    arraySubscriptingOrder = GiftiArrayIndexingOrder::ROW_MAJOR_ORDER;
   externalFileName = "";
   externalFileOffset = 0;
   
    if (intent == NiftiIntent::NIFTI_INTENT_POINTSET) {   
      Matrix4x4 gm;
      matrices.push_back(gm);
   }
   
   dataType = NiftiDataType::NIFTI_TYPE_FLOAT32;
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
   intent = nda.intent;
   encoding = nda.encoding;
   arraySubscriptingOrder = nda.arraySubscriptingOrder;
   dataType = nda.dataType;
   //dataLocation = nda.dataLocation;
   dataTypeSize = nda.dataTypeSize;
   endian = nda.endian;
   parentGiftiFile = NULL;  // caused modified to be set !! nda.parentGiftiFile;
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
GiftiDataArray::getDataTypeAppropriateForIntent(const NiftiIntent::Enum intent,
                                                  NiftiDataType::Enum& dataTypeOut)
{
    if (intent == NiftiIntent::NIFTI_INTENT_POINTSET) {
      dataTypeOut = NiftiDataType::NIFTI_TYPE_FLOAT32;
   }
   else if (intent == NiftiIntent::NIFTI_INTENT_TIME_SERIES) {
      dataTypeOut = NiftiDataType::NIFTI_TYPE_FLOAT32;
   }
   else if (intent == NiftiIntent::NIFTI_INTENT_NORMAL) {
      dataTypeOut = NiftiDataType::NIFTI_TYPE_FLOAT32;
   }
   else if (intent == NiftiIntent::NIFTI_INTENT_LABEL) {
      dataTypeOut = NiftiDataType::NIFTI_TYPE_INT32;
   }
   else if ((intent == NiftiIntent::NIFTI_INTENT_RGB_VECTOR) ||
            (intent == NiftiIntent::NIFTI_INTENT_RGBA_VECTOR)) {
      dataTypeOut = NiftiDataType::NIFTI_TYPE_UINT8;
   }
   else if (intent == NiftiIntent::NIFTI_INTENT_SHAPE) {
      dataTypeOut = NiftiDataType::NIFTI_TYPE_FLOAT32;
   }
   else if (intent == NiftiIntent::NIFTI_INTENT_SYMMATRIX) {
      dataTypeOut = NiftiDataType::NIFTI_TYPE_FLOAT32;
   }
   else if (intent == NiftiIntent::NIFTI_INTENT_TRIANGLE) {
      dataTypeOut = NiftiDataType::NIFTI_TYPE_INT32;
   }
   else {
      std::cout << "WARNING: unrecogized intent \""
       << NiftiIntent::toString(intent, NULL).c_str()
                << " in GiftiDataArray::getDataTypeAppropriateForIntent()." << std::endl;
      return false;
   }
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
   std::unique(rowsToDelete.begin(), rowsToDelete.end());
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
      case NiftiDataType::NIFTI_TYPE_FLOAT32:
         dataTypeSize = sizeof(float);
         break;
      case NiftiDataType::NIFTI_TYPE_INT32:
         dataTypeSize = sizeof(int32_t);
         break;
      case NiftiDataType::NIFTI_TYPE_UINT8:
         dataTypeSize = sizeof(uint8_t);
         break;
       default:
           assert(0);
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
         case NiftiDataType::NIFTI_TYPE_FLOAT32:
            dataPointerFloat = (float*)&data[0];
            break;
         case NiftiDataType::NIFTI_TYPE_INT32:
            dataPointerInt   = (int32_t*)&data[0];
            break;
         case NiftiDataType::NIFTI_TYPE_UINT8:
            dataPointerUByte = (uint8_t*)&data[0];
            break;
          default:
              assert(0);
      }
   }
}
      
/**
 * reset column.
 */
void 
GiftiDataArray::clear()
{
    arraySubscriptingOrder = GiftiArrayIndexingOrder::ROW_MAJOR_ORDER;
    encoding = GiftiEncoding::ASCII;
   dataType = NiftiDataType::NIFTI_TYPE_FLOAT32;
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
int64_t 
GiftiDataArray::getDataOffset(const int64_t nodeNum,
                                  const int64_t componentNum) const
{
   const int64_t off = nodeNum * dimensions[1] + componentNum;
   return off;
}


/**
 * get the system's endian.
 */
GiftiEndian::Enum 
GiftiDataArray::getSystemEndian()
{
    GiftiEndian::Enum endian = GiftiEndian::ENDIAN_BIG;
    if (ByteOrder::isSystemLittleEndian()) {
        endian = GiftiEndian::ENDIAN_LITTLE;
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
GiftiDataArray::getExternalFileInformation(std::string& nameOut,
                                           int64_t & offsetOut) const
{
   nameOut = externalFileName;
   offsetOut = externalFileOffset;
}
                                
/**
 * set external file information.
 */
void 
GiftiDataArray::setExternalFileInformation(const std::string& nameIn,
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
   if (remappingTable.empty()) {
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
    if (this->getDataType() == NiftiDataType::NIFTI_TYPE_INT32) {
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
GiftiDataArray::readFromText(const std::string text,
            const GiftiEndian::Enum dataEndianForReading,
            const GiftiArrayIndexingOrder::Enum arraySubscriptingOrderForReading,
            const NiftiDataType::Enum dataTypeForReading,
            const std::vector<int64_t>& dimensionsForReading,
            const GiftiEncoding::Enum encodingForReading,
            const std::string& externalFileNameForReading,
            const int64_t externalFileOffsetForReading) throw (GiftiException)
{
   const NiftiDataType::Enum requiredDataType = dataType;
   dataType = dataTypeForReading;
   encoding = encodingForReading;
   endian   = dataEndianForReading;
   setDimensions(dimensionsForReading);
   if (dimensionsForReading.size() == 0) {
      throw GiftiException("Data array has no dimensions.");
   }
   setExternalFileInformation(externalFileNameForReading,
                              externalFileOffsetForReading);
                              
   //
   // If NOT metadata only
   //
   if (parentGiftiFile->getReadMetaDataOnlyFlag() == false) {
      //
      // Total number of elements in Data Array
      //
      const int64_t numElements = getTotalNumberOfElements();
      
      switch (encoding) {
          case GiftiEncoding::ASCII:
            {
                std::istringstream stream(text);
                
               switch (dataType) {
                  case NiftiDataType::NIFTI_TYPE_FLOAT32:
                     {
                        float* ptr = dataPointerFloat;
                        for (int64_t i = 0; i < numElements; i++) {
                           stream >> *ptr;
                           ptr++;
                        }
                     }
                     break;
                   case NiftiDataType::NIFTI_TYPE_INT32:
                     {
                        int32_t* ptr = dataPointerInt;
                        for (int64_t i = 0; i < numElements; i++) {
                           stream >> *ptr;
                           ptr++;
                        }
                     }
                     break;
                   case NiftiDataType::NIFTI_TYPE_UINT8:
                     {
                        uint8_t* ptr = dataPointerUByte;
                         int32_t c;
                        for (int64_t i = 0; i < numElements; i++) {
                           stream >> c;
                           *ptr = static_cast<uint8_t>(c);
                           ptr++;
                        }
                     }
                     break;
                   default:
                       throw GiftiException("DataType " + NiftiDataType::toString(dataType, NULL) + " not supported in GIFTI");
               }
            }
            break;
          case GiftiEncoding::BASE64_BINARY:
            {
               //
               // Decode the Base64 data using VTK's algorithm
               //
               const uint64_t numDecoded =
                     Base64::decode((const unsigned char*)(text.c_str()),
                                                data.size(),
                                                &data[0]);
               if (numDecoded != data.size()) {
                  std::ostringstream str;
                  str << "Decoding of Base64 Binary data failed.\n"
                   << "Decoded " << StringUtilities::fromNumber(numDecoded) << " bytes but should be "
                      << StringUtilities::fromNumber(static_cast<int>(data.size())) << " bytes.";
                  throw GiftiException(str.str());
               }
               
               //
               // Is byte swapping needed ?
               //
               if (endian != getSystemEndian()) {
                  byteSwapData(getSystemEndian());
               }
            }
            break;
          case GiftiEncoding::GZIP_BASE64_BINARY:
            {
               //
               // Decode the Base64 data using VTK's algorithm
               //
               unsigned char* dataBuffer = new unsigned char[data.size()];
               // crashes const char* textChars = text.toAscii().constData();
               const uint64_t numDecoded =
                     Base64::decode((unsigned char*)text.c_str(),
                                                data.size(),
                                                dataBuffer);
               if (numDecoded == 0) {
                  throw GiftiException("Decoding of GZip Base64 Binary data failed.");
               }
               
               
               //
               // Uncompress the data using VTK's algorithm
               // 
                DataCompressZLib compressor;
                const uint64_t uncompressedDataLength = 
                                   compressor.uncompressData(dataBuffer,
                                                          numDecoded,
                                                          (unsigned char*)&data[0],
                                                          data.size());
               if (uncompressedDataLength != data.size()) {
                  std::ostringstream str;
                  str << "Decompression of Binary data failed.\n"
                   << "Uncompressed " << StringUtilities::fromNumber(uncompressedDataLength) << " bytes but should be "
                   << StringUtilities::fromNumber(static_cast<uint64_t>(data.size())) << " bytes.";
                  throw GiftiException(str.str());
               }
               
               //
               // Free memory
               //
               delete[] dataBuffer;
               
               //
               // Is byte swapping needed ?
               //
               if (endian != getSystemEndian()) {
                  byteSwapData(getSystemEndian());
               }
            }
            break;
          case GiftiEncoding::EXTERNAL_FILE_BINARY:
            {
               if (externalFileName.length() <= 0) {
                  throw GiftiException("External file name is empty.");
               }
               
                std::ifstream extBinFile(StringUtilities::toStdString(externalFileName).c_str());
                if (!extBinFile) {
                        throw GiftiException("Error opening \""
                                            + externalFileName
                                            + "\"");
                }
                else {
                  //
                  // Move to the offset of the data
                  //
                  extBinFile.seekg(externalFileOffset, std::ios::beg);
                    
                  //
                  // Set the number of bytes that must be read
                  //
                    std::streamsize numberOfBytesToRead = 0;
                  char* pointerToForReadingData = NULL;
                  switch (dataType) {
                     case NiftiDataType::NIFTI_TYPE_FLOAT32:
                        numberOfBytesToRead = numElements * sizeof(float);
                        pointerToForReadingData = (char*)dataPointerFloat;
                        break;
                     case NiftiDataType::NIFTI_TYPE_INT32:
                        numberOfBytesToRead = numElements * sizeof(int32_t);
                        pointerToForReadingData = (char*)dataPointerInt;
                        break;
                     case NiftiDataType::NIFTI_TYPE_UINT8:
                        numberOfBytesToRead = numElements * sizeof(uint8_t);
                        pointerToForReadingData = (char*)dataPointerUByte;
                        break;
                      default:
                          assert(0);
                  }
               
                  //
                  // Read the data
                  //
                  extBinFile.read((char*)&pointerToForReadingData,
                                    numberOfBytesToRead);
                  if(extBinFile.fail()) {
                     throw GiftiException("Tried to read "
                                         + StringUtilities::fromNumber((int64_t)numberOfBytesToRead)
                                         + " from "
                                         + externalFileName
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
          if (intent != NiftiIntent::NIFTI_INTENT_POINTSET) {
            convertToDataType(requiredDataType);
         }
      }
      
      //
      // Are array indices in opposite order
      //
      if (arraySubscriptingOrderForReading != arraySubscriptingOrder) {
         convertArrayIndexingOrder();
      }
   } // If NOT metadata only
   
   setModified();
}

/**
 * convert array indexing order of data.
 */
void 
GiftiDataArray::convertArrayIndexingOrder() throw (GiftiException)
{
   const int32_t numDim = static_cast<int32_t>(dimensions.size());
   if (numDim <= 1) {
      return;
   }
   
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
       if ((dimI == 1) || (dimJ == 1)) {
          return;
       }

       //
       // Is matrix square?
       //
       if (dimI == dimJ) {
           switch (dataType) {
               case NiftiDataType::NIFTI_TYPE_FLOAT32:
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
              case NiftiDataType::NIFTI_TYPE_INT32:
                 {
                   for (int64_t i = 1; i < dimI; i++) {
                       for (int64_t j = 0; j < i; j++) {
                           const int64_t indexLowerLeft  = (i * dimJ) + j;
                           const int64_t indexUpperRight = (j * dimI) + i;
                           float temp = dataPointerInt[indexLowerLeft];
                           dataPointerInt[indexLowerLeft] = dataPointerInt[indexUpperRight];
                           dataPointerInt[indexUpperRight] = temp;
                       }
                   }
                 }
                 break;
              case NiftiDataType::NIFTI_TYPE_UINT8:
                 {
                   for (int64_t i = 1; i < dimI; i++) {
                       for (int64_t j = 0; j < i; j++) {
                           const int64_t indexLowerLeft  = (i * dimJ) + j;
                           const int64_t indexUpperRight = (j * dimI) + i;
                           float temp = dataPointerUByte[indexLowerLeft];
                           dataPointerUByte[indexLowerLeft] = dataPointerUByte[indexUpperRight];
                           dataPointerUByte[indexUpperRight] = temp;
                       }
                   }
                 }
                 break;
               default:
                   assert(0);
                   break;
           }
       }
       else {
           //
           // Copy the data
           //
           std::vector<uint8_t> dataCopy = data;

          switch (dataType) {
             case NiftiDataType::NIFTI_TYPE_FLOAT32:
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
             case NiftiDataType::NIFTI_TYPE_INT32:
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
             case NiftiDataType::NIFTI_TYPE_UINT8:
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
                  assert(0);
                  break;
          }
          
          dimensions[0] = dimJ;
          dimensions[1] = dimI;
      }
   }
}

/**
 * write the data as XML.
 */
void 
GiftiDataArray::writeAsXML(std::ostream& stream, 
                           std::ostream* externalBinaryOutputStream) 
                                                throw (GiftiException)
{
    //
    // Do not write if data array is isEmpty
    //
    const int64_t numRows = dimensions[0];
    if (numRows <= 0) {
        return;
    }
    
    XmlWriter xmlWriter(stream);
   //
   // Clean up the dimensions by removing any "last" dimensions that
   // are one with the exception of the first dimension
   //   e.g.:   dimension = [73730, 1]  becomes [73730]
   //
   const int32_t dimensionality = static_cast<int32_t>(dimensions.size());
   for (int32_t i = (dimensionality - 1); i >= 1; i--) {
      if (dimensions[i] <= 1) {
         dimensions.resize(i);
      }
   }
    
   //
   // External file not supported
   //
   //const std::string externalFileName = "";
   //const std::string externalFileOffset = "0";
   
   //
   // Write the opening tag
   //
    XmlAttributes dataAtt;
    dataAtt.addAttribute(GiftiXmlElements::ATTRIBUTE_DATA_ARRAY_INTENT, NiftiIntent::toString(this->intent, NULL));
    dataAtt.addAttribute(GiftiXmlElements::ATTRIBUTE_DATA_ARRAY_DATA_TYPE, NiftiDataType::toString(this->dataType, NULL));
    dataAtt.addAttribute(GiftiXmlElements::ATTRIBUTE_DATA_ARRAY_INDEXING_ORDER, GiftiArrayIndexingOrder::toString(this->arraySubscriptingOrder, NULL));
    dataAtt.addAttribute(GiftiXmlElements::ATTRIBUTE_DATA_ARRAY_DIMENSIONALITY, this->dimensions, ",");
    dataAtt.addAttribute(GiftiXmlElements::ATTRIBUTE_DATA_ARRAY_ENCODING, GiftiEncoding::toString(this->encoding, NULL));
    dataAtt.addAttribute(GiftiXmlElements::ATTRIBUTE_DATA_ARRAY_ENDIAN, GiftiEndian::toString(this->endian, NULL));
    dataAtt.addAttribute(GiftiXmlElements::ATTRIBUTE_DATA_ARRAY_EXTERNAL_FILE_NAME, externalFileName);
    dataAtt.addAttribute(GiftiXmlElements::ATTRIBUTE_DATA_ARRAY_EXTERNAL_FILE_OFFSET, externalFileOffset);

    
    
    xmlWriter.writeStartElement(GiftiXmlElements::TAG_DATA_ARRAY, dataAtt);
    
   
   //
   // Write the metadata
   //
    this->metaData.writeAsXML(xmlWriter);
   
   //
   // Write the matrices
   //
   for (int32_t i = 0; i < getNumberOfMatrices(); i++) {
       this->matrices[i].writeAsXML(xmlWriter);
   }
   
   //
   // Write the data element opening tag
   //
    xmlWriter.writeStartElement(GiftiXmlElements::TAG_DATA);
   
   //
   // NOTE: for the base64 and ZLIB-Base64 data, it is important that there are
   // no spaces between the <DATA> and </DATA> tags.
   //
   switch (encoding) {
       case GiftiEncoding::ASCII:
         {
            //
            // Newline after <DATA> tag (only do this for ASCII !!!)
            //
            stream << "\n";
            
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
               switch (dataType) {
                  case NiftiDataType::NIFTI_TYPE_FLOAT32:
                     for (int64_t j = 0; j < numItemsPerRow; j++) {
                         xmlWriter.writeCharacters(StringUtilities::fromNumber(this->dataPointerFloat[offset + j]));
                         xmlWriter.writeCharacters(" ");                         
                     }
                     break;
                  case NiftiDataType::NIFTI_TYPE_INT32:
                     for (int64_t j = 0; j < numItemsPerRow; j++) {
                         xmlWriter.writeCharacters(StringUtilities::fromNumber(this->dataPointerInt[offset + j]));
                         xmlWriter.writeCharacters(" ");                         
                     }
                     break;
                  case NiftiDataType::NIFTI_TYPE_UINT8:
                     for (int64_t j = 0; j < numItemsPerRow; j++) {
                         xmlWriter.writeCharacters(StringUtilities::fromNumber(this->dataPointerUByte[offset + j]));
                         xmlWriter.writeCharacters(" ");                         
                     }
                     break;
                   default:
                       assert(0);
                       break;
               }
               xmlWriter.writeCharacters("\n");
               offset += numItemsPerRow;
            }
         }
         break;
       case GiftiEncoding::BASE64_BINARY:
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
                     + StringUtilities::fromNumber(bufferLength)
                     + ") is too small but needs to be "
                                    + StringUtilities::fromNumber(compressedLength));
            }
            buffer[compressedLength] = '\0';
            
            //
            // Write the data
            //
            stream << buffer;
            
            //
            // Free memory
            //
            delete[] buffer;
         }
         break;
       case GiftiEncoding::GZIP_BASE64_BINARY:
         {
            //
            // Compress the data with VTK's ZLIB algorithm
            //
             DataCompressZLib compressor;
             unsigned long compressedDataBufferLength = 
                              compressor.getMaximumCompressionSpace(data.size());
            unsigned char* compressedDataBuffer = new unsigned char[compressedDataBufferLength];
            unsigned long compressedDataLength =
                          compressor.compressData(&data[0], 
                                               data.size(),
                                               compressedDataBuffer,
                                               compressedDataBufferLength);
            
            //
            // Encode the data with VTK's Base64 algorithm
            //
            char* buffer = new char[static_cast<int64_t>(compressedDataLength * 1.5)];
            const uint64_t compressedLength =
               Base64::encode(compressedDataBuffer,
                                          compressedDataLength,
                                          (unsigned char*)buffer);
            buffer[compressedLength] = '\0';
            
            //
            // Write the data
            //
            stream << buffer;
            
            //
            // Free memory
            //
            delete[] buffer;
            delete[] compressedDataBuffer;
         }
         break;
       case GiftiEncoding::EXTERNAL_FILE_BINARY:
         {
            const int64_t dataLength = data.size();
            externalBinaryOutputStream->write((const char*)&data[0], dataLength);
            if (externalBinaryOutputStream->bad()) {
               throw GiftiException("Output stream for external file reports its status as bad.");
            }
         }
         break;
   }
   
   //
   // Write the closing Data tag
   //
    xmlWriter.writeEndElement();
   
   //
   // write the closing data array tag
   //
   xmlWriter.writeEndElement();
}                      

/**
 * convert to data type.
 */
void 
GiftiDataArray::convertToDataType(const NiftiDataType::Enum newDataType)
{
   if (newDataType != dataType) {      
      //
      // make a copy of myself
      //
      GiftiDataArray copyOfMe(*this);
      
      //
      // Set my new data type and reallocate memory
      //
      const NiftiDataType::Enum oldDataType = dataType;
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
               case NiftiDataType::NIFTI_TYPE_FLOAT32:
                  switch (oldDataType) {
                     case NiftiDataType::NIFTI_TYPE_FLOAT32:
                        dataPointerFloat[i] = static_cast<float>(copyOfMe.dataPointerFloat[i]);
                        break;
                     case NiftiDataType::NIFTI_TYPE_INT32:
                        dataPointerFloat[i] = static_cast<float>(copyOfMe.dataPointerInt[i]);
                        break;
                     case NiftiDataType::NIFTI_TYPE_UINT8:
                        dataPointerFloat[i] = static_cast<float>(copyOfMe.dataPointerUByte[i]);
                        break;
                      default:
                          assert(0);
                          break;
                  }
                  break;
               case NiftiDataType::NIFTI_TYPE_INT32:
                  switch (oldDataType) {
                     case NiftiDataType::NIFTI_TYPE_FLOAT32:
                        dataPointerInt[i] = static_cast<int32_t>(copyOfMe.dataPointerFloat[i]);
                        break;
                     case NiftiDataType::NIFTI_TYPE_INT32:
                        dataPointerInt[i] = static_cast<int32_t>(copyOfMe.dataPointerInt[i]);
                        break;
                     case NiftiDataType::NIFTI_TYPE_UINT8:
                        dataPointerInt[i] = static_cast<int32_t>(copyOfMe.dataPointerUByte[i]);
                        break;
                      default:
                          assert(0);
                          break;
                  }
                  break;
               case NiftiDataType::NIFTI_TYPE_UINT8:
                  switch (oldDataType) {
                     case NiftiDataType::NIFTI_TYPE_FLOAT32:
                        dataPointerUByte[i] = static_cast<uint8_t>(copyOfMe.dataPointerFloat[i]);
                        break;
                     case NiftiDataType::NIFTI_TYPE_INT32:
                        dataPointerUByte[i] = static_cast<uint8_t>(copyOfMe.dataPointerInt[i]);
                        break;
                     case NiftiDataType::NIFTI_TYPE_UINT8:
                        dataPointerUByte[i] = static_cast<uint8_t>(copyOfMe.dataPointerUByte[i]);
                        break;
                      default:
                          assert(0);
                          break;
                  }
                  break;
                default:
                    assert(0);
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
GiftiDataArray::byteSwapData(const GiftiEndian::Enum newEndian)
{
   endian = newEndian;
   switch (dataType) {
      case NiftiDataType::NIFTI_TYPE_FLOAT32:
         ByteSwapping::swapBytes(dataPointerFloat, getTotalNumberOfElements());
         break;
      case NiftiDataType::NIFTI_TYPE_INT32:
         ByteSwapping::swapBytes(dataPointerInt, getTotalNumberOfElements());
         break;
      case NiftiDataType::NIFTI_TYPE_UINT8:
         // should not need to swap ??
         break;
       default:
           assert(0);
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
}

/**
 * Get the modification status.  Returns true if this object or
 * any of its children have been modified.
 * @return - The modification status.
 *
 */
bool
GiftiDataArray::isModified() const
{
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
 * get minimum and maximum values (valid for float data only).
 */
void 
GiftiDataArray::getMinMaxValues(float& minValue, float& maxValue) const
{
   if (minMaxFloatValuesValid == false) {
      minValueFloat = std::numeric_limits<float>::max();
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
 * Get data column min/max for the specified percentages.
 */
void
GiftiDataArray::getMinMaxValuesFromPercentages(const float negMaxPctIn,
                                               const float negMinPctIn,
                                               const float posMinPctIn,
                                               const float posMaxPctIn,
                                               float& negMaxPctValueOut,
                                               float& negMinPctValueOut,
                                               float& posMinPctValueOut,
                                               float& posMaxPctValueOut)
{
   if ((negMaxPctIn != negMaxPct) ||
       (negMinPctIn != negMinPct) ||
       (posMinPctIn != posMinPct) ||
       (posMaxPctIn != posMaxPct)) {
      minMaxPercentageValuesValid = false;
   }
   if (minMaxPercentageValuesValid == false) {
      negMaxPct = negMaxPctIn;
      negMinPct = negMinPctIn;
      posMinPct = posMinPctIn;
      posMaxPct = posMaxPctIn;

      negMaxPctValue = 0.0;
      negMinPctValue = 0.0;
      posMinPctValue = 0.0;
      posMaxPctValue = 0.0;

      const int64_t num = getTotalNumberOfElements();
      if (num > 0) {
         std::vector<float> negatives, positives;
         negatives.reserve(num);
         positives.reserve(num);
         for (int64_t i = 0; i < num; i++) {
            if (dataPointerFloat[i] > 0.0) {
               positives.push_back(dataPointerFloat[i]);
            }
            else if (dataPointerFloat[i] < 0.0) {
               negatives.push_back(dataPointerFloat[i]);
            }
         }

         int32_t numPos = static_cast<int32_t>(positives.size());
         if (numPos > 0) {
            std::sort(positives.begin(), positives.end());

            if (numPos == 1) {
               posMinPctValue = positives[0];
               posMaxPctValue = positives[0];
            }
            else {
               int32_t minIndex = numPos * (posMinPct / 100.0);
               if (minIndex < 0) minIndex = 0;
               if (minIndex >= numPos) minIndex = numPos - 1;
               posMinPctValue = positives[minIndex];

               int32_t maxIndex = numPos * (posMaxPct / 100.0);
               if (maxIndex < 0) maxIndex = 0;
               if (maxIndex >= numPos) maxIndex = numPos - 1;
               posMaxPctValue = positives[maxIndex];
            }
         }

         int32_t numNeg = static_cast<int32_t>(negatives.size());
         if (numNeg > 0) {
            std::sort(negatives.begin(), negatives.end());

            if (numNeg == 1) {
               negMinPctValue = negatives[0];
               negMaxPctValue = negatives[0];
            }
            else {
               int32_t maxIndex = numNeg * ((100.0 - negMaxPct) / 100.0);
               if (maxIndex < 0) maxIndex = 0;
               if (maxIndex >= numNeg) maxIndex = numNeg - 1;
               negMaxPctValue = negatives[maxIndex];

               int32_t minIndex = numNeg * ((100.0 - negMinPct) / 100.0);
               if (minIndex < 0) minIndex = 0;
               if (minIndex >= numNeg) minIndex = numNeg - 1;
               negMinPctValue = negatives[minIndex];
            }
         }
      }

      minMaxPercentageValuesValid = true;
   }

   negMaxPctValueOut = negMaxPctValue;
   negMinPctValueOut = negMinPctValue;
   posMaxPctValueOut = posMaxPctValue;
   posMinPctValueOut = posMinPctValue;
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
       case GiftiArrayIndexingOrder::ROW_MAJOR_ORDER:
         for (int32_t d = (numDim - 1); d >= 0; d--) {
            offset += indices[d] * dimProduct;
            dimProduct *= dimensions[d];
         }
         break;
      case GiftiArrayIndexingOrder::COLUMN_MAJOR_ORDER:  // correct???
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
GiftiDataArray::intentNameValid(const std::string& intentNameIn)
{
    bool valid = false;
    NiftiIntent::fromString(intentNameIn, &valid);
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
      
std::string 
GiftiDataArray::toString() const
{
    std::ostringstream str;
    str << "Data Array" << std::endl;
    str << "   DataType=" << NiftiDataType::toString(this->dataType, NULL) << std::endl;
    str << "   Intent=" << NiftiIntent::toString(this->intent, NULL) << std::endl;
    str << "   Dimensions=" << StringUtilities::fromNumbers(this->dimensions, ",");
    return str.str();
}
