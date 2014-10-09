#ifndef __GIFTI_FILE_H__
#define __GIFTI_FILE_H__

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
#include <stdint.h>

#include "CaretAssert.h"
#include "DataFile.h"
#include "GiftiDataArray.h"
#include "GiftiEncodingEnum.h"
#include "GiftiLabelTable.h"
#include "NiftiEnums.h"
#include "TracksModificationInterface.h"

namespace caret { 
    
/// This abstract class defines some variables and methods used for gifti data array files.
/// While this class may be instantiated, it is best subclassed.
class GiftiFile : public DataFile {
   public:
      /// append array index values
      enum APPEND_ARRAY_INDEX {
         APPEND_ARRAY_NEW = -1,
         APPEND_ARRAY_DO_NOT_LOAD = -2
      };
      
      /// constructor
    GiftiFile(const AString& descriptiveName,
              const NiftiIntentEnum::Enum defaultDataArrayIntentIn,
              const NiftiDataTypeEnum::Enum defaultDataTypeIn,
              const AString& defaultExt,
              const bool dataAreIndicesIntoLabelTableIn);
      
      /// constructor for generic gifti data array file
      GiftiFile();
      
      // copy constructor
      GiftiFile(const GiftiFile& nndf);
      
      // destructor
      virtual ~GiftiFile();

      // assignment operator
      GiftiFile& operator=(const GiftiFile& nndf);
      
      // add a data array
      virtual void addDataArray(GiftiDataArray* nda);
            
      // add rows to this file.
      void addRows(const int32_t numberOfRowsToAdd);

      // append a data array file to this one
      virtual void append(const GiftiFile& naf);

      // append a data array file to this one but selectively load/overwrite arraysumns
      // arrayDestination is where naf's arrays should be (-1=new, -2=do not load)
      virtual void append(const GiftiFile& naf, 
                          std::vector<int32_t>& indexDestinationg);

      /// compare a file for unit testing (returns true if "within tolerance")
      virtual bool compareFileForUnitTesting(const GiftiFile* gf,
                                             const float tolerance,
                                             AString& messageOut) const;
                                     
      // Clear the gifti array data file.
      virtual void clear();
      
      virtual void clearAndKeepMetadata();
      
      // returns true if the file is isEmpty() (contains no data)
      virtual bool isEmpty() const;
      
    AString getFileComment() const;
    
    void setFileComment(const AString& comment);
    
    void appendToFileComment(const AString& comment);
    
      /// get the number of data arrays
      int32_t getNumberOfDataArrays() const { return dataArrays.size() ; }
      
      /// get a data array
      GiftiDataArray* getDataArray(const int32_t arrayNumber) { CaretAssertVectorIndex(dataArrays, arrayNumber); return dataArrays[arrayNumber]; }
      
      /// get a data array (const method)
      const GiftiDataArray* getDataArray(const int32_t arrayNumber) const { CaretAssertVectorIndex(dataArrays, arrayNumber); return dataArrays[arrayNumber]; }
      
      /// reset a data array
      virtual void resetDataArray(const int32_t arrayIndex);
      
      /// remove a data array
      virtual void removeDataArray(const GiftiDataArray* arrayPointer);
      
      /// remove a data array
      virtual void removeDataArray(const int32_t arrayIndex);
      
      // get all of the data array names
      void getAllArrayNames(std::vector<AString>& names) const;
      
      // get the specified data array's name
      AString getDataArrayName(const int32_t arrayIndex) const;

      // get the index of the data array with the specified name 
      int32_t getDataArrayWithNameIndex(const AString& n) const;
 
      // get the data array with the specified name 
      GiftiDataArray* getDataArrayWithName(const AString& n);
 
      // get the data array with the specified name 
      const GiftiDataArray* getDataArrayWithName(const AString& n) const;
 
      // get the index of the data array of the specified intent
    int32_t getDataArrayWithIntentIndex(const NiftiIntentEnum::Enum intent) const;
      
      // get the data array of the specified intent
    GiftiDataArray* getDataArrayWithIntent(const NiftiIntentEnum::Enum intent);
      
      // get the data array of the specified intent (const method)
    const GiftiDataArray* getDataArrayWithIntent(const NiftiIntentEnum::Enum intent) const;
      
      // get the comment for a data array
      AString getDataArrayComment(const int32_t arrayIndex) const;
      
      // set the name of a data array
      void setDataArrayName(const int32_t arrayIndex, const AString& name);
      
      // set the comment for a data array
      void setDataArrayComment(const int32_t arrayIndex, const AString& comm);
      
      // append to the comment for a data array
      void appendToDataArrayComment(const int32_t arrayIndex, const AString& comm);
      
      // prepend to the comment for a data array
      void prependToDataArrayComment(const int32_t arrayIndex, const AString& comm);
      
      // check for data arrays with the same name (returns true if there are any)
      bool checkForDataArraysWithSameName(std::vector<AString>& multipleDataArrayNames) const;
      
      // get the metadata
      GiftiMetaData* getMetaData() { return &metaData; }
      
      // get the metadata (const method)
      const GiftiMetaData* getMetaData() const { return &metaData; }
      
      /// get the label table 
      GiftiLabelTable* getLabelTable() { return &labelTable; }
      
      /// get the label table 
      const GiftiLabelTable* getLabelTable() const { return &labelTable; }
            
      /// get the current version for GiftiFiles
      static float getCurrentFileVersion() { return 1.0; }
      
      /// get the default data array intent
      NiftiIntentEnum::Enum getDefaultDataArrayIntent() const { return defaultDataArrayIntent; }
      
      /// get the default data array intent
      void setDefaultDataArrayIntent(const NiftiIntentEnum::Enum newIntent);
      
      /// set the number of nodes for sparse node index files (NIFTI_INTENT_NODE_INDEX)
      void setNumberOfNodesForSparseNodeIndexFiles(const int32_t numNodes);
      
    // read the XML file 
    virtual void readFile(const AString& filename);
    
    // write the XML file
    virtual void writeFile(const AString& filename);
    
    bool getReadMetaDataOnlyFlag() const { return false; }
    
    /** @return The encoding used to write the file. */
    GiftiEncodingEnum::Enum getEncodingForWriting() const { return this->encodingForWriting; }
    
    void setEncodingForWriting(const GiftiEncodingEnum::Enum encoding);
    
    virtual void clearModified();
    
    virtual bool isModified() const;
    
    virtual AString toString() const;
    
   protected:
      // append helper for files where data are label indices
      //void appendLabelDataHelper(const GiftiFile& naf,
      //                           const std::vector<bool>& arrayWillBeAppended,
      //                           std::vector<int32_t>& oldIndicesToNewIndicesTable);
                                 
      // copy helper
      void copyHelperGiftiFile(const GiftiFile& nndf);
      
      
      // process NIFTI_INTENT_NODE_INDEX arrays
      void procesNiftiIntentNodeIndexArrays();

      // validate the data arrays (optional for subclasses)
      virtual void validateDataArrays();

      /// the data arrays
      std::vector<GiftiDataArray*> dataArrays;
      
      /// the label table
      GiftiLabelTable labelTable;
      
      /// the file's metadata
      GiftiMetaData metaData;
      
      GiftiEncodingEnum::Enum encodingForWriting;
    
      /// the default data type
      NiftiDataTypeEnum::Enum defaultDataType;
      
      /// default data array intent for this file
      NiftiIntentEnum::Enum defaultDataArrayIntent;
      
      /// data arrays contain indices into label table
      bool dataAreIndicesIntoLabelTable;
      
    AString descriptiveName;
    AString defaultExtension;
    
      /// number of nodes in sparse node index files (NIFTI_INTENT_NODE_INDEX array)
      int32_t numberOfNodesForSparseNodeIndexFile;
      
    /** The default encoding for writing a GIFTI file. */
    static GiftiEncodingEnum::Enum defaultEncodingForWriting;
    
      /*!!!! be sure to update copyHelperGiftiFile if new member added !!!!*/
   
   // 
   // friends
   //
};

#ifdef __GIFTI_FILE_MAIN__
    GiftiEncodingEnum::Enum GiftiFile::defaultEncodingForWriting = GiftiEncodingEnum::GZIP_BASE64_BINARY;
#endif // __GIFTI_FILE_MAIN__
    

} // namespace 

#endif // __GIFTI_FILE_H__

