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

#include <memory>
#include <set>
#include <sstream>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DataFileException.h"
#include "FileInformation.h"
#include "GiftiEncodingEnum.h"
#define __GIFTI_FILE_MAIN__
#include "GiftiFile.h"
#undef __GIFTI_FILE_MAIN__
#include "GiftiFileSaxReader.h"
#include "GiftiMetaDataXmlElements.h"
#include "GiftiFileWriter.h"
#include "GiftiXmlElements.h"

#include "XmlSaxParser.h"

#include <QFile>

using namespace caret;

/**
 * Constructor
 */
GiftiFile::GiftiFile(const AString& descriptiveName,
                     const NiftiIntentEnum::Enum defaultDataArrayIntentIn,
                                       const NiftiDataTypeEnum::Enum defaultDataTypeIn,
                                       const AString& defaultExtension,
                                       const bool dataAreIndicesIntoLabelTableIn)
: DataFile()
{
    this->descriptiveName = descriptiveName;
   defaultDataArrayIntent = defaultDataArrayIntentIn;
   defaultDataType = defaultDataTypeIn;
   dataAreIndicesIntoLabelTable = dataAreIndicesIntoLabelTableIn;
    this->defaultExtension = defaultExtension;
   numberOfNodesForSparseNodeIndexFile = 0;
    this->encodingForWriting = GiftiFile::defaultEncodingForWriting;
}

/**
 * Constructor for generic gifti data array file
 */
GiftiFile::GiftiFile()
: DataFile()
{
    this->descriptiveName = "GIFTI";
    defaultDataArrayIntent = NiftiIntentEnum::NIFTI_INTENT_NONE;
    defaultDataType = NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32;
    dataAreIndicesIntoLabelTable = false;
    numberOfNodesForSparseNodeIndexFile = 0;
    this->defaultExtension = ".gii";
    this->encodingForWriting = GiftiFile::defaultEncodingForWriting;
}

/**
 * copy constructor.
 */
GiftiFile::GiftiFile(const GiftiFile& nndf)
: DataFile(nndf)
{
   copyHelperGiftiFile(nndf);
}
      
/**
 * assignment operator.
 */
GiftiFile& 
GiftiFile::operator=(const GiftiFile& nndf)
{
   if (this != &nndf) {
       DataFile::operator=(nndf);
       this->copyHelperGiftiFile(nndf);
   }
   
   return *this;
}
      
/**
 * copy helper.
 */
void 
GiftiFile::copyHelperGiftiFile(const GiftiFile& nndf)
{
   labelTable = nndf.labelTable;
   metaData = nndf.metaData;
   defaultDataType = nndf.defaultDataType;
   defaultDataArrayIntent = nndf.defaultDataArrayIntent;
   dataAreIndicesIntoLabelTable = nndf.dataAreIndicesIntoLabelTable;
   numberOfNodesForSparseNodeIndexFile = nndf.numberOfNodesForSparseNodeIndexFile;
    this->descriptiveName = nndf.descriptiveName;
    this->defaultExtension = nndf.defaultExtension;
   int32_t numArrays = this->getNumberOfDataArrays();
   for (int32_t i = (numArrays - 1); i >= 0; i--) {
      this->removeDataArray(i);
   }
    for (std::size_t i = 0; i < nndf.dataArrays.size(); i++) {
      addDataArray(new GiftiDataArray(*nndf.dataArrays[i]));
   }
    this->encodingForWriting = nndf.encodingForWriting;
}
      
/**
 * Destructor
 */
GiftiFile::~GiftiFile()
{
    for (uint64_t i = 0; i < this->dataArrays.size(); i++) {
        delete this->dataArrays[i];
    }
}

/**
 * compare a file for unit testing (returns true if "within tolerance").
 */
bool 
GiftiFile::compareFileForUnitTesting(const GiftiFile* gf,
                                              const float tolerance,
                                              AString& messageOut) const
{
   messageOut = "";
   
   if (gf == NULL) {
      messageOut += "ERROR: File for comparison is not a GiftiFile or subtype.\n";
      return false;
   }
   
   const int32_t numLabels = labelTable.getNumberOfLabels();
   if (numLabels != gf->labelTable.getNumberOfLabels()) {
      messageOut += "ERROR: The files contain a different number of labels.\n";
   }
   else {
      int32_t labelCount = 0;
      for (int32_t k = 0; k < numLabels; k++) {
         if (labelTable.getLabel(k) != gf->getLabelTable()->getLabel(k)) {
            labelCount++;
         }
      }
      
      if (labelCount > 0) {
         messageOut += "ERROR: The files have "
                    + AString::number(labelCount)
                    + " different labels.\n";
      }
   }
   
   const int32_t numArrays = getNumberOfDataArrays();
   if (numArrays != gf->getNumberOfDataArrays()) {
      messageOut += "ERROR: The files contain a different number of data arrays (data columns)";
   }
   else {
      for (int32_t i = 0; i < numArrays; i++) {
         const GiftiDataArray* gdf1 = getDataArray(i);
         const GiftiDataArray* gdf2 = gf->getDataArray(i);
         
         const std::vector<int64_t> dim1 = gdf1->getDimensions();
         const std::vector<int64_t> dim2 = gdf2->getDimensions();
         if (dim1 != dim2) {
            messageOut += "ERROR: Data Array " 
                          + AString::number(i)
                          + " have a different number of dimensions.\n";
         }
         else {
            if (gdf1->getDataType() != gdf2->getDataType()) {
               messageOut += "ERROR: Data Array "
                          + AString::number(i)
                          + " are different data types.\n";
            }
            else if (gdf1->getTotalNumberOfElements() != gdf2->getTotalNumberOfElements()) {
               messageOut += "ERROR: Data Array "
                          + AString::number(i)
                          + " have a different number of total elements.\n";
            }
            else {
               const int32_t numElem = gdf1->getTotalNumberOfElements();
               int32_t diffCount = 0;
               
               switch (gdf1->getDataType()) {
                   case NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32:
                     {
                        const float* p1 = gdf1->getDataPointerFloat();
                        const float* p2 = gdf2->getDataPointerFloat();
                        for (int64_t m = 0; m < numElem; m++) {
                           float diff = p1[m] - p2[m];
                           if (diff < 0.0) diff = -diff;
                           if (diff > tolerance) {
                              diffCount++;
                           }
                        }
                     }
                     break;
                  case NiftiDataTypeEnum::NIFTI_TYPE_INT32:
                     {
                        const int32_t tol = static_cast<int32_t>(tolerance);
                        const int32_t* p1 = gdf1->getDataPointerInt();
                        const int32_t* p2 = gdf2->getDataPointerInt();
                        for (int64_t m = 0; m < numElem; m++) {
                           float diff = p1[m] - p2[m];
                           if (diff < 0.0) diff = -diff;
                           if (diff > tol) {
                              diffCount++;
                           }
                        }
                     }
                     break;
                  case NiftiDataTypeEnum::NIFTI_TYPE_UINT8:
                     {
                        const uint8_t tol = static_cast<uint8_t>(tolerance);
                        const uint8_t* p1 = gdf1->getDataPointerUByte();
                        const uint8_t* p2 = gdf2->getDataPointerUByte();
                        for (int64_t m = 0; m < numElem; m++) {
                           float diff = p1[m] - p2[m];
                           if (diff < 0.0) diff = -diff;
                           if (diff > tol) {
                              diffCount++;
                           }
                        }
                     }
                     break;
                   default:
                       CaretAssertMessage(0, "Invalid Data Type");
                       break;
               }
               
               if (diffCount > 0) {
                  messageOut += "ERROR: There are " 
                                + AString::number(diffCount)
                                + " elements with a difference that are greater than "
                                + AString::number(tolerance)
                                + " in data array "
                                + AString::number(i)
                                + ".\n";
               }
            }
         }
      }
   }
   
   return (messageOut.isEmpty());
}                                     

/**
 * Set the name of a data array.
 */
void
GiftiFile::setDataArrayName(const int32_t arrayIndex, const AString& name)
{
    CaretAssertVectorIndex(this->dataArrays, arrayIndex);
    dataArrays[arrayIndex]->getMetaData()->set(GiftiXmlElements::TAG_METADATA_NAME, name);
    setModified();
}

/**
 * returns true if the file is isEmpty() (contains no data).
 */
bool 
GiftiFile::isEmpty() const
{
   return dataArrays.empty();
}
      
/**
 * get the data array with the specified name.
 */
GiftiDataArray* 
GiftiFile::getDataArrayWithName(const AString& n) 
{
   for (int32_t i = 0; i < getNumberOfDataArrays(); i++) {
      if (getDataArrayName(i) == n) {
         return getDataArray(i);
      }
   }
   return NULL;
}

/**
 * get the data array with the specified name.
 */
const GiftiDataArray* 
GiftiFile::getDataArrayWithName(const AString& n) const 
{
   for (int32_t i = 0; i < getNumberOfDataArrays(); i++) {
      if (getDataArrayName(i) == n) {
         return getDataArray(i);
      }
   }
   return NULL;
}

/**
 * Get the data array index for an array with the specified name.  If the
 * name is not found a negative number is returned.
 */
int
GiftiFile::getDataArrayWithNameIndex(const AString& n) const
{
   for (int32_t i = 0; i < getNumberOfDataArrays(); i++) {
      if (getDataArrayName(i) == n) {
         return i;
      }
   }
   return -1;
}

/**
 * get the data array of the specified intent.
 */
GiftiDataArray* 
GiftiFile::getDataArrayWithIntent(const NiftiIntentEnum::Enum intent) 
{
   for (int32_t i = 0; i < getNumberOfDataArrays(); i++) {
      GiftiDataArray* gda = getDataArray(i);
      if (gda->getIntent() == intent) {
         return gda;
      }
   }
   return NULL;
}

/**
 * get the data array of the specified intent (const method).
 */
const GiftiDataArray* 
GiftiFile::getDataArrayWithIntent(const NiftiIntentEnum::Enum intent) const
{
   for (int32_t i = 0; i < getNumberOfDataArrays(); i++) {
      const GiftiDataArray* gda = getDataArray(i);
      if (gda->getIntent() == intent) {
         return gda;
      }
   }
   return NULL;
}

/**
 * get the index of the data array of the specified intent.
 */
int32_t 
GiftiFile::getDataArrayWithIntentIndex(const NiftiIntentEnum::Enum intent) const
{
   for (int32_t i = 0; i < getNumberOfDataArrays(); i++) {
      const GiftiDataArray* gda = getDataArray(i);
      if (gda->getIntent() == intent) {
         return i;
      }
   }
   return -1;
}

/**
 * Get the name for a data array.
 */
AString
GiftiFile::getDataArrayName(const int32_t arrayIndex) const
{
    CaretAssertVectorIndex(this->dataArrays, arrayIndex);
   AString s = dataArrays[arrayIndex]->getMetaData()->get(GiftiXmlElements::TAG_METADATA_NAME);
   return s;
}

/**
 * Set the comment for a data array.
 */
void
GiftiFile::setDataArrayComment(const int32_t arrayIndex, const AString& comm)
{
    CaretAssertVectorIndex(this->dataArrays, arrayIndex);
    dataArrays[arrayIndex]->getMetaData()->set(GiftiMetaDataXmlElements::METADATA_NAME_COMMENT, comm);
   setModified();
}

/**
 * Append to the comment for a data array.
 */
void
GiftiFile::appendToDataArrayComment(const int32_t arrayIndex, const AString& comm)
{
    CaretAssertVectorIndex(this->dataArrays, arrayIndex);
   if (comm.isEmpty() == false) {
      AString s(getDataArrayComment(arrayIndex));
      s.append(comm);
      setDataArrayComment(arrayIndex, s);
      setModified();
   }
}

/**
 * Prepend to the comment for a data array.
 */
void
GiftiFile::prependToDataArrayComment(const int32_t arrayIndex, const AString& comm)
{
    CaretAssertVectorIndex(this->dataArrays, arrayIndex);
   if (comm.isEmpty() == false) {
      AString s(comm);
      s.append(getDataArrayComment(arrayIndex));
      setDataArrayComment(arrayIndex, s);
      setModified();
   }
}

/**
 * Get the comment for a data array.
 */
AString
GiftiFile::getDataArrayComment(const int32_t arrayIndex) const
{
    CaretAssertVectorIndex(this->dataArrays, arrayIndex);
  AString s;
    (void)dataArrays[arrayIndex]->getMetaData()->get(GiftiMetaDataXmlElements::METADATA_NAME_COMMENT);
   return s;
}

/**
 * get the default data array intent.
 */
void 
GiftiFile::setDefaultDataArrayIntent(const NiftiIntentEnum::Enum newIntent) 
{ 
   defaultDataArrayIntent = newIntent; 
   setModified();
}
      
/**
 *
 */
void
GiftiFile::clear()
{
    DataFile::clear();
    
    for (std::size_t i = 0; i < dataArrays.size(); i++) {
      if (dataArrays[i] != NULL) {
         delete dataArrays[i];
         dataArrays[i] = NULL;
      }
   }
   dataArrays.clear();
   
   labelTable.clear();
   metaData.clear();
   
   // do not clear
   // giftiElementName
   // requiredArrayTypeDataTypes
}

void GiftiFile::clearAndKeepMetadata()
{//same as above, minus metaData.clear()
    DataFile::clear();
    for (std::size_t i = 0; i < dataArrays.size(); i++)
    {
        if (dataArrays[i] != NULL)
        {
            delete dataArrays[i];
            dataArrays[i] = NULL;
        }
    }
    dataArrays.clear();
    labelTable.clear();
}

/**
 * get all of the data array names.
 */
void 
GiftiFile::getAllArrayNames(std::vector<AString>& names) const
{
   names.clear();
   
   for (int32_t i = 0; i < getNumberOfDataArrays(); i++) {
      names.push_back(getDataArrayName(i));
   }
}

/**
 * check for data arrays with the same name (returns true if there are any).
 */
bool 
GiftiFile::checkForDataArraysWithSameName(std::vector<AString>& multipleArrayNames) const
{
   multipleArrayNames.clear();
   
   const int32_t numArrays = getNumberOfDataArrays();
   
   if (numArrays > 0) {
      std::set<AString> badNames;
      for (int32_t i = 0; i < (numArrays - 1); i++) {
         for (int32_t j = i + 1; j < numArrays; j++) {
            if (getDataArrayName(i) == getDataArrayName(j)) {
               badNames.insert(getDataArrayName(i));
            }
         }
      }
      
      if (badNames.empty() == false) {
         multipleArrayNames.insert(multipleArrayNames.begin(),
                                    badNames.begin(), badNames.end());
      }
   }
   
   return (multipleArrayNames.size() > 0);
}
      
/**
 * add a data array.
 */
void 
GiftiFile::addDataArray(GiftiDataArray* gda)
{
    CaretAssert(gda);
    
//    const AString uuid = gda->getMetaData()->getUniqueID();
//    const int32_t numDataArrays = this->getNumberOfDataArrays();
//    for (int32_t i = 0; i < numDataArrays; i++) {
//        const AString daUuid = this->getDataArray(i)->getMetaData()->getUniqueID();
//        if (daUuid == uuid) {
//            CaretLogWarning("File "
//                            + this->getFileNameNoPath()
//                            + " contains multiple data arrays with the same unique identifier.  "
//                            "Unique ID has been modified to ensure uniqueness within file.");
//            const bool modStatus = gda->isModified();
//            gda->getMetaData()->resetUniqueIdentifier();
//            if (modStatus == false) {
//                gda->clearModified();
//            }
//        }
//    }
    
    dataArrays.push_back(gda);
   
    
    setModified();
}

/**
 * append a gifti array data  file to this one.
 */
void 
GiftiFile::append(const GiftiFile& gf)
{
    const bool copyDataArrayFlag = true;
   const int32_t numArrays = gf.getNumberOfDataArrays();
   if (numArrays <= 0) {
      return;
   }
   
   //
   // Replace filename if "this" file is isEmpty()
   //
   if (getNumberOfDataArrays() == 0) {
      setFileName(gf.getFileName());
   }
   
    /*
     * Append metadata.
     */
    this->metaData.append(*gf.getMetaData());
    
    /*
     * Updates for label table.
     */
    std::map<int32_t,int32_t> labelIndexConverter =
    this->labelTable.append(*gf.getLabelTable());
    
   //
   // Append the data arrays
   //
   for (int32_t i = 0; i < gf.getNumberOfDataArrays(); i++) {
       GiftiDataArray* gda = gf.dataArrays[i];
       if (copyDataArrayFlag) {
           gda = new GiftiDataArray(*(gf.dataArrays[i]));
       }
       if (gda->getIntent() == NiftiIntentEnum::NIFTI_INTENT_LABEL) {
           gda->transferLabelIndices(labelIndexConverter);
       }
       this->addDataArray(gda);
   }
   
   setModified();
}

/**
 * append array data file to this one but selectively load/overwrite arrays
 * indexDestination is where naf's data arrays should be (-1=new, -2=do not load).
 * "indexDestination" will be updated with the columns actually used.
 * @param gf
 *    GIFTI file that is appended.
 * @param indexDestination
 *    How arrays are added/replaced.
 */
void 
GiftiFile::append(const GiftiFile& gf, 
                     std::vector<int32_t>& indexDestination)
{
    const bool copyDataArrayFlag = true;
   const int32_t numArrays = gf.getNumberOfDataArrays();
   if (numArrays <= 0) {
      return;
   }
   
    /*
     * Append metadata.
     */
    this->metaData.append(*gf.getMetaData());
    
    /*
     * Updates for label table.
     */
    std::map<int32_t,int32_t> labelIndexConverter =
    this->labelTable.append(*gf.getLabelTable());
    
   
   //
   // Replace file name if this file is isEmpty()
   //
   if (getNumberOfDataArrays() == 0) {
      setFileName(gf.getFileName());
   }
   
   //
   // append the data arrays
   //
   for (int32_t i = 0; i < numArrays; i++) {
       const int32_t arrayIndex = indexDestination[i];
       if (arrayIndex != 0) {
      
           GiftiDataArray* gda = gf.dataArrays[i];
           if (copyDataArrayFlag) {
               gda = new GiftiDataArray(*gda);
               if (gda->getIntent() == NiftiIntentEnum::NIFTI_INTENT_LABEL) {
                   gda->transferLabelIndices(labelIndexConverter);
               }
               this->addDataArray(gda);
           }
           
           //
           // Replacing existing array ?
           //
           if (arrayIndex >= 0) {
               delete dataArrays[arrayIndex];
               dataArrays[arrayIndex] = gda;
           }
           //
           // create new array
           //
           else if (arrayIndex == -1) {
               dataArrays.push_back(gda);
               
               //
               // Lets others know where the array was placed
               //
               indexDestination[i] = getNumberOfDataArrays() - 1;
            }
       }
      //
      // Ignore array
      //
      else {
         // nothing
      }
   }
   

   this->appendToFileComment(gf.getFileComment());
   
   setModified();
}

AString 
GiftiFile::getFileComment() const
{
    return this->metaData.get(GiftiMetaDataXmlElements::METADATA_NAME_COMMENT);
}
    
void 
GiftiFile::setFileComment(const AString& comment)
{
    if (comment.isEmpty() == false) {
        AString s = this->getFileName();
        s += "\n" + comment;
        this->setFileComment(s);
    }
}
    
void 
GiftiFile::appendToFileComment(const AString& comment)
{
    this->metaData.set(GiftiMetaDataXmlElements::METADATA_NAME_COMMENT, comment);        
}
    
/**
 * append helper for files where data are label indices.
 * The table "oldIndicesToNewIndicesTable" maps label indices from 
 * "naf" label indices to the proper indices for "this" file.
 *
void 
GiftiFile::appendLabelDataHelper(const GiftiFile& naf,
                                          const std::vector<bool>& arrayWillBeAppended,
                                          std::vector<int32_t>& oldIndicesToNewIndicesTable)
{
   oldIndicesToNewIndicesTable.clear();
   
   if ((dataAreIndicesIntoLabelTable == false) ||
       (naf.dataAreIndicesIntoLabelTable == false)) {
      return;
   }
   
   const int32_t numArrays = naf.getNumberOfDataArrays();
   if (numArrays != static_cast<int32_t>(arrayWillBeAppended.size())) {
      return;
   }
   
   const GiftiLabelTable* nltNew = naf.getLabelTable();
   const int32_t numLabelsNew = nltNew->getNumberOfLabels();
   if (numLabelsNew <= 0) {
      return;
   }
   oldIndicesToNewIndicesTable.resize(numLabelsNew, -1);
   
   //
   // Determine which labels will be appended
   //
   for (int32_t i = 0; i < numArrays; i++) {
      GiftiDataArray* nda = naf.dataArrays[i];
       if (nda->getDataType() == NiftiDataType::NIFTI_TYPE_INT32) {
         const int32_t numElem = nda->getTotalNumberOfElements();
         if (numElem >= 0) {
            int32_t* dataPtr = nda->getDataPointerInt();
            for (int32_t i = 0; i < numElem; i++) {
               const int32_t indx = dataPtr[i];
               if ((indx >= 0) && (indx < numLabelsNew)) {
                  oldIndicesToNewIndicesTable[indx] = -2;
               }
               else {
                  std::cout << "WARNING Invalid label index set to zero: " << indx 
                            << " of " << numLabelsNew << " labels." << std::endl;
                  dataPtr[i] = 0;
               }
            }
         }
      }
   }
   
   //
   // remap the label indices
   //
   GiftiLabelTable* myLabelTable = getLabelTable();
   for (int32_t i = 0; i < numLabelsNew; i++) {
      if (oldIndicesToNewIndicesTable[i] == -2) {
         int32_t indx = myLabelTable->addLabel(nltNew->getLabel(i));
         oldIndicesToNewIndicesTable[i] = indx;

          nltNew->g
         unsigned char r, g, b, a;
         nltNew->getColor(i, r, g, b, a);
         myLabelTable->setColor(indx, r, g, b, a);
      }
   }
}
*/

/**
 * add rows to this file.
 */
void 
GiftiFile::addRows(const int32_t numberOfRowsToAdd)
{
   for (int32_t i = 0; i < getNumberOfDataArrays(); i++) {
      dataArrays[i]->addRows(numberOfRowsToAdd);
   }
   setModified();
}

/**
 * reset a data array.
 */
void 
GiftiFile::resetDataArray(const int32_t arrayIndex)
{
    CaretAssertVectorIndex(this->dataArrays, arrayIndex);
   dataArrays[arrayIndex]->zeroize();
}

/**
 * remove a data array.
 */
void 
GiftiFile::removeDataArray(const int32_t arrayIndex)
{
    CaretAssertVectorIndex(this->dataArrays, arrayIndex);
   int32_t numArrays = getNumberOfDataArrays();
   if ((arrayIndex >= 0) && (arrayIndex < numArrays)) {
      delete dataArrays[arrayIndex];
      for (int32_t i = arrayIndex; i < (numArrays - 1); i++) {
         dataArrays[i] = dataArrays[i + 1];
      }
      dataArrays.resize(numArrays - 1);
   }
}      
 
/**
 * remove a data array.
 */
void 
GiftiFile::removeDataArray(const GiftiDataArray* arrayPointer)
{
    CaretAssert(arrayPointer);
   for (int32_t i = 0; i < getNumberOfDataArrays(); i++) {
      if (getDataArray(i) == arrayPointer) {
         removeDataArray(i);
         break;
      }
   }
}

/**
 * read the file.
 */
void
GiftiFile::readFile(const AString& filename)
{
    this->clear();
    this->setFileName(filename);
    
    GiftiFileSaxReader saxReader(this);
    std::unique_ptr<XmlSaxParser> parser(XmlSaxParser::createXmlParser());
    try {
        parser->parseFile(filename, &saxReader);
    }
    catch (const XmlSaxParserException& e) {
        clear();
        this->setFileName("");
        
        int lineNum = e.getLineNumber();
        int colNum  = e.getColumnNumber();
        
        std::ostringstream str;
        str << "File is not a valid GIFTI file";
        if ((lineNum >= 0) && (colNum >= 0)) {
            str << ", error parsing at line/col ("
            << e.getLineNumber()
            << "/"
            << e.getColumnNumber()
            << ")";
        }
        str << ": "
            << e.whatString().toStdString();
        throw DataFileException(filename,
                                AString::fromStdString(str.str()));
    }
    
    /*
     * If any maps are missing names, give them default names.
     */
    const int32_t numArrays = getNumberOfDataArrays();
    for (int32_t i = 0; i < numArrays; i++) {
        AString arrayName = getDataArrayName(i);
        if (arrayName.isEmpty()) {
            arrayName = ("#"
                         + AString::number(i + 1));
            setDataArrayName(i,
                             arrayName);
        }
    }
}

/**
 * write the file. 
 */
void 
GiftiFile::writeFile(const AString& filename)
{
    try {
        this->setFileName(filename);
        
        QFile::remove(filename);
        
        /*FileInformation fileInfo(filename);
        if (fileInfo.exists()) {
            //if (GiftiDataArrayFile.isFileOverwriteAllowed() == false) {
            //    throw new GiftiException(
            //                             "Overwriting of existing files is currently prohibited");
            //}
        }//*/
        
        //
        // Create a GIFTI Data Array File Writer
        //
        GiftiFileWriter giftiFileWriter(filename,
                            this->encodingForWriting);
        
        //
        // Start writing the file
        //
        int numberOfDataArrays = this->getNumberOfDataArrays();
        giftiFileWriter.start(numberOfDataArrays,
                              &this->metaData,
                              &this->labelTable);
        
        //
        // Write the data arrays
        //
        for (int i = 0; i < numberOfDataArrays; i++) {
            giftiFileWriter.writeDataArray(this->getDataArray(i));
        }
        
        //
        // Finish writing the file
        //
        giftiFileWriter.finish();
    }
    catch (const GiftiException& e) {
        throw DataFileException(filename,
                                e.whatString());
    }
    
    this->clearModified();
    
/*
   //
   // Get how the array data should be encoded
   //
   GiftiDataArray::ENCODING encoding = GiftiDataArray::ENCODING_INTERNAL_ASCII;
   switch (getFileWriteType()) {
      case FILE_FORMAT_ASCII:
         break;
      case FILE_FORMAT_BINARY:
         break;
      case FILE_FORMAT_XML:
         encoding = GiftiDataArray::ENCODING_INTERNAL_ASCII;
         break;
      case FILE_FORMAT_XML_BASE64:
         encoding = GiftiDataArray::ENCODING_INTERNAL_BASE64_BINARY;
         break;
      case FILE_FORMAT_XML_GZIP_BASE64:
         encoding = GiftiDataArray::ENCODING_INTERNAL_COMPRESSED_BASE64_BINARY;
         break;
      case FILE_FORMAT_XML_EXTERNAL_BINARY:
         encoding = GiftiDataArray::ENCODING_EXTERNAL_FILE_BINARY;
         break;
      case FILE_FORMAT_OTHER:
         break;
      case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
         break;
   }
   
   AString giftiFileVersionString = 
      AString::number(GiftiFile::getCurrentFileVersion(), 'f', 6);
   while (giftiFileVersionString.endsWith("00")) {
      giftiFileVersionString.resize(giftiFileVersionString.size() - 1);
   }
   
   stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << "\n";

   stream << "<!DOCTYPE GIFTI SYSTEM \"http://www.nitrc.org/frs/download.php/1594/gifti.dtd\">" << "\n";
   
   stream << "<" 
          << GiftiCommon::tagGIFTI << "\n"
          << "      xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n"
          << "      xsi:noNamespaceSchemaLocation=\"http://brainvis.wustl.edu/caret6/xml_schemas/GIFTI_Caret.xsd\"\n"
          << "      " << GiftiCommon::attVersion << "=\""
          << giftiFileVersionString 
          << "\"\n"
          << "      " << GiftiCommon::attNumberOfDataArrays << "=\""
          << getNumberOfDataArrays()
          << "\""
          << ">" << "\n";
   int32_t indent = 0;

   //
   // External binary file.
   //
   AString externalBinaryFileName;
   int64_t externalBinaryFileDataOffset = 0;
   std::ofstream* externalBinaryOutputStream = NULL;
   if (encoding == GiftiDataArray::ENCODING_EXTERNAL_FILE_BINARY) {
       externalBinaryFileName = getFileNameNoPath() + ".data";
       externalBinaryOutputStream = new std::ofstream(externalBinaryFileName.toLatin1().constData(),
                                                      std::ofstream::binary);
       if (externalBinaryOutputStream == NULL) {
          throw GiftiException("Unable to open " + externalBinaryFileName + " for output.");
       }
   }
   
#ifdef CARET_FLAG
   //
   // copy the Abstract File header into this file's metadata 
   //
   metaData.clear();
   AbstractFileHeaderContainer::iterator iter;
   for (iter = header.begin(); iter != header.end(); iter++) {
      //
      // Get the tag and its value
      //
      const AString tag(iter->first);
      const AString value(iter->second);
      metaData.set(tag,value);
   }
#endif // CARET_FLAG
   
   indent++;
   metaData.writeAsXML(stream, indent);
   indent--;
   
   //
   // Write the labels
   //
   indent++;
   labelTable.writeAsXML(stream, indent);
   indent--;   
   
   indent++;
   for (unsigned int32_t i = 0; i < dataArrays.size(); i++) {
#ifdef CARET_FLAG
      dataArrays[i]->setEncoding(encoding);
#endif // CARET_FLAG
      if (externalBinaryOutputStream != NULL) {
          externalBinaryFileDataOffset = externalBinaryOutputStream->tellp();
      }
      dataArrays[i]->setExternalFileInformation(externalBinaryFileName,
                                                externalBinaryFileDataOffset);
      dataArrays[i]->writeAsXML(stream, indent, externalBinaryOutputStream);
   }
   indent--;
   
   stream << "</" << GiftiCommon::tagGIFTI << ">" << "\n";
   
   if (externalBinaryOutputStream != NULL) {
      externalBinaryOutputStream->close();
   }
*/
}      

/**
 * set the number of nodes for sparse node index files (NIFTI_INTENT_NODE_INDEX).
 */
void 
GiftiFile::setNumberOfNodesForSparseNodeIndexFiles(const int32_t numNodes)
{
   numberOfNodesForSparseNodeIndexFile = numNodes;
}      

/**
 * process NIFTI_INTENT_NODE_INDEX arrays.
 */
void 
GiftiFile::procesNiftiIntentNodeIndexArrays()
{
   //
   // See if there is a node index array
   //
    GiftiDataArray* nodeIndexArray = getDataArrayWithIntent(NiftiIntentEnum::NIFTI_INTENT_NODE_INDEX);
   if (nodeIndexArray != NULL) {
      //
      // Make sure node index array is integer type and one dimensional
      //
      if (nodeIndexArray->getDataType() != NiftiDataTypeEnum::NIFTI_TYPE_INT32) {
          throw GiftiException("Data type other than \"int\" not supported for data intent  node index.");
      }
      if (nodeIndexArray->getNumberOfDimensions() < 1) {
          throw GiftiException("Dimensions other than one not supported for data intent node index.");
      }
      
      //
      // Make node index array integer
      //
       nodeIndexArray->convertToDataType(NiftiDataTypeEnum::NIFTI_TYPE_INT32);
      
      //
      // Get the node indices
      //
      const int32_t numNodeIndices = nodeIndexArray->getDimension(0);
      if (numNodeIndices <= 0) {
         throw GiftiException("Dimension is zero for data intent: "
                              + NiftiIntentEnum::toName(NiftiIntentEnum::NIFTI_INTENT_NODE_INDEX));
      }
      const int32_t zeroIndex[2] = { 0, 0 };
      const int32_t* indexData = nodeIndexArray->getDataInt32Pointer(zeroIndex);
      
      //
      // Find the true number of nodes
      //
      int32_t numNodes = numberOfNodesForSparseNodeIndexFile;
      if (numNodes <= 0) {
         int32_t minNodeIndex = 0;
         nodeIndexArray->getMinMaxValues(minNodeIndex, numNodes);
      }
      
      //
      // Check each data array
      //
      const int32_t numArrays = getNumberOfDataArrays();
      for (int32_t i = 0; i < numArrays; i++) {
         GiftiDataArray* dataArray = getDataArray(i);
          if (dataArray->getIntent() != NiftiIntentEnum::NIFTI_INTENT_NODE_INDEX) {
            if (dataArray->getNumberOfDimensions() < 1) {
               throw GiftiException("Data Array with intent \""
                                    + NiftiIntentEnum::toName(dataArray->getIntent())
                                   + " is not one-dimensional in sparse node file.");
            }
            if (dataArray->getDimension(0) != numNodeIndices) {
               throw GiftiException("Data Array with intent \""
                                   + NiftiIntentEnum::toName(dataArray->getIntent())
                                   + " has a different number of nodes than the NIFTI_INTENT_NODE_INDEX array in the file.");
            }
            
            switch (dataArray->getDataType()) {
                case NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32:
                  {
                     std::vector<float> dataFloat(numNodes, 0.0);
                     const float* readPtr = dataArray->getDataFloat32Pointer(zeroIndex);
                     for (int32_t m = 0; m < numNodeIndices; m++) {
                        dataFloat[indexData[m]] = readPtr[m];
                     }
                     std::vector<int64_t> newDim(1, numNodes);
                     dataArray->setDimensions(newDim);
                     for (int32_t n = 0; n < numNodes; n++) {
                        const int32_t indxs[2] = { n, 0 };
                        dataArray->setDataFloat32(indxs, dataFloat[n]);
                     }
                  }
                  break;
               case NiftiDataTypeEnum::NIFTI_TYPE_INT32:
                  {
                     std::vector<int32_t> dataInt(numNodes, 0);
                     const int32_t* readPtr = dataArray->getDataInt32Pointer(zeroIndex);
                     for (int32_t m = 0; m < numNodeIndices; m++) {
                        dataInt[indexData[m]] = readPtr[m];
                     }
                     std::vector<int64_t> newDim(1, numNodes);
                     dataArray->setDimensions(newDim);
                     for (int32_t n = 0; n < numNodes; n++) {
                        const int32_t indxs[2] = { n, 0 };
                        dataArray->setDataInt32(indxs, dataInt[n]);
                     }
                  }
                  break;
               case NiftiDataTypeEnum::NIFTI_TYPE_UINT8:
                  {
                     std::vector<uint8_t> dataByte(numNodes, 0);
                     const uint8_t* readPtr = dataArray->getDataUInt8Pointer(zeroIndex);
                     for (int32_t m = 0; m < numNodeIndices; m++) {
                        dataByte[indexData[m]] = readPtr[m];
                     }
                     std::vector<int64_t> newDim(1, numNodes);
                     dataArray->setDimensions(newDim);
                     for (int32_t n = 0; n < numNodes; n++) {
                        const int32_t indxs[2] = { n, 0 };
                        dataArray->setDataUInt8(indxs, dataByte[n]);
                     }
                  }
                  break;
                default:
                    CaretAssertMessage(0, "Invalid Data Type");
                    break;
            }
         }
      }
      
      //
      // Remove the node index array
      //
      removeDataArray(nodeIndexArray);      
   }
}

    /**
     * Set this object as not modified.  Object should also
     * clear the modification status of its children.
     *
     */
    void
    GiftiFile::clearModified()
    {
        DataFile::clearModified();
        metaData.clearModified();
        labelTable.clearModified();
        for (int i = 0; i < this->getNumberOfDataArrays(); i++) {
            this->getDataArray(i)->clearModified();
        }
    }
    
    /**
     * Get the modification status.  Returns true if this object or
     * any of its children have been modified.
     * @return - The modification status.
     *
     */
    bool
    GiftiFile::isModified() const
    {
        if (DataFile::isModified()) {
            return true;
        }
        if (metaData.isModified()) {
            return true;
        }
        if (labelTable.isModified()) {
            return true;
        }
        for (int i = 0; i < this->getNumberOfDataArrays(); i++) {
            if (this->getDataArray(i)->isModified()) {
                return true;
            }
        }
        return false;
    }

/**
 * Set the encoding for writing the file.
 * @param encoding
 *    New encoding.
 */
void 
GiftiFile::setEncodingForWriting(const GiftiEncodingEnum::Enum encoding)
{
    this->encodingForWriting = encoding;
}


    
/**
 * Validate the data arrays (optional for subclasses).
 */
void 
GiftiFile::validateDataArrays()
{
    // nothing 
}

AString 
GiftiFile::toString() const
{
    std::ostringstream str;
    str << "Gifti File: " << this->getFileName().toStdString() << std::endl;
    str << this->metaData.toString().toStdString() << std::endl;
    str << this->labelTable.toString().toStdString() << std::endl;
    for (int32_t i = 0; i < this->getNumberOfDataArrays(); i++) {
        const GiftiDataArray* gda = this->getDataArray(i);
        str << gda->toString().toStdString() << std::endl;
    }
    return AString::fromStdString(str.str());
}

