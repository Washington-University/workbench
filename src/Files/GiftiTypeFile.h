#ifndef __GIFTI_TYPE_FILE_H__
#define __GIFTI_TYPE_FILE_H__

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


#include <AString.h>

#include "CaretMappableDataFile.h"
#include "StructureEnum.h"

namespace caret {

    class GiftiFile;
    class PaletteColorMapping;
    
    /// Encapsulates a GiftiFile for use by specific types of GIFTI data files.
    class GiftiTypeFile : public CaretMappableDataFile {
        
    protected:
        GiftiTypeFile(const DataFileTypeEnum::Enum dataFileType);
        
        virtual ~GiftiTypeFile();

        GiftiTypeFile(const GiftiTypeFile& s);
        
        GiftiTypeFile& operator=(const GiftiTypeFile&);
        
        /**
         * Validate the contents of the file after it
         * has been read such as correct number of 
         * data arrays and proper data types/dimensions.
         * 
         * @throws DataFileException
         *    If the file is not valid.
         */
        virtual void validateDataArraysAfterReading() throw (DataFileException) = 0;
        
        void verifyDataArraysHaveSameNumberOfRows(const int32_t minimumSecondDimension,
                                                  const int32_t maximumSecondDimension) const throw (DataFileException);

    public:
        virtual void clear();
        
        virtual void clearModified();
        
        virtual bool isModified() const;
        
        virtual bool isEmpty() const;

        virtual void readFile(const AString& filename) throw (DataFileException);
        
        virtual void writeFile(const AString& filename) throw (DataFileException);
        
        virtual AString toString() const;
        
        virtual GiftiMetaData* getFileMetaData();
        
        virtual const GiftiMetaData* getFileMetaData() const;
        
        virtual StructureEnum::Enum getStructure() const;
        
        virtual void setStructure(const StructureEnum::Enum structure);
        
        /** @return  Number of nodes in the file. */
        virtual int32_t getNumberOfNodes() const = 0;
        
        /** @return  Number of columns (data arrays) in the file. */
        virtual int32_t getNumberOfColumns() const = 0;
        
        //virtual void setNumberOfNodesAndColumns(int32_t nodes, int32_t columns) = 0;
        
        virtual AString getColumnName(const int32_t columnIndex) const;
        
        int32_t getColumnIndexFromColumnName(const AString& columnName) const;
        
        virtual void setColumnName(const int32_t columnIndex,
                                   const AString& columnName);
        
        PaletteColorMapping* getPaletteColorMapping(const int32_t columnIndex);
        
        
        
        
        virtual bool isSurfaceMappable() const;
        
        virtual bool isVolumeMappable() const;
        
        virtual int32_t getNumberOfMaps() const;
        
        virtual AString getMapName(const int32_t mapIndex) const;
        
        virtual int32_t getMapIndexFromName(const AString& mapName);
        
        virtual void setMapName(const int32_t mapIndex,
                                const AString& mapName);
        
        virtual const GiftiMetaData* getMapMetaData(const int32_t mapIndex) const;
        
        virtual GiftiMetaData* getMapMetaData(const int32_t mapIndex);
        
        virtual const DescriptiveStatistics* getMapStatistics(const int32_t mapIndex);
        
        virtual bool isMappedWithPalette() const;
        
        virtual PaletteColorMapping* getMapPaletteColorMapping(const int32_t mapIndex);
        
        virtual const PaletteColorMapping* getMapPaletteColorMapping(const int32_t mapIndex) const;
        
        virtual bool isMappedWithLabelTable() const;
        
        virtual GiftiLabelTable* getMapLabelTable(const int32_t mapIndex);
        
        virtual const GiftiLabelTable* getMapLabelTable(const int32_t mapIndex) const;        
        
        virtual AString getMapUniqueID(const int32_t mapIndex) const;
        
        virtual int32_t getMapIndexFromUniqueID(const AString& uniqueID) const;
        
    private:
        void copyHelperGiftiTypeFile(const GiftiTypeFile& gtf);
        
        void initializeMembersGiftiTypeFile();
        
    protected:
        GiftiFile* giftiFile;
    };
    
} // namespace

#endif // __GIFTI_TYPE_FILE_H__

