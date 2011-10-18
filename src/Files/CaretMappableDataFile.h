#ifndef __CARET_MAPPABLE_DATA_FILE__H_
#define __CARET_MAPPABLE_DATA_FILE__H_

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


#include "CaretDataFile.h"

namespace caret {

    class DescriptiveStatistics;
    class GiftiMetaData;
    class GiftiLabelTable;
    class PaletteColorMapping;
    
    /**
     * \class CaretMappableDataFile 
     * \brief A Caret data file that is mappable to surfaces and/or volumes.
     *
     * This class is essentially an interface that defines methods for
     * files that are 'mappable', as an overlay, to surfaces and/or volumes.
     * Use of a common interface simplifies selection and application
     * of these data files.
     *
     * For a GIFTI File, the number of maps is the number of data arrays
     * in the GIFTI file.  For a volume, it may be the number of time points.
     * 
     * Note that Caret5 used the term 'column'.
     */
    
    class CaretMappableDataFile : public CaretDataFile {
        
    public:
        CaretMappableDataFile(const DataFileTypeEnum::Enum dataFileType);
        
        virtual ~CaretMappableDataFile();
        
        /**
         * @return Is the data mappable to a surface?
         */
        virtual bool isSurfaceMappable() const = 0;
        
        /**
         * @return Is the data mappable to a volume?
         */
        virtual bool isVolumeMappable() const = 0;
        
        /**
         * @return The number of maps in the file.  
         * Note: Caret5 used the term 'columns'.
         */
        virtual int32_t getNumberOfMaps() const = 0;
        
        /**
         * Get the name of the map at the given index.
         * 
         * @param mapIndex
         *    Index of the map.
         * @return
         *    Name of the map.
         */
        virtual AString getMapName(const int32_t mapIndex) const = 0;

        /**
         * Find the index of the map that uses the given name.
         * 
         * @param mapName
         *    Name of the desired map.
         * @return
         *    Index of the map using the given name.  If there is more
         *    than one map with the given name, this method is likely
         *    to return the index of the first map with the name.
         */
        virtual int32_t getMapIndexFromName(const AString& mapName) = 0;
        
        /**
         * Set the name of the map at the given index.
         *
         * @param mapIndex
         *    Index of the map.
         * @param mapName
         *    New name for the map.
         */
        virtual void setMapName(const int32_t mapIndex,
                                const AString& mapName) = 0;
        
        /**
         * Get the metadata for the map at the given index
         *
         * @param mapIndex
         *    Index of the map.
         * @return
         *    Metadata for the map (const value).
         */         
        virtual const GiftiMetaData* getMapMetaData(const int32_t mapIndex) const = 0;
        
        /**
         * Get the metadata for the map at the given index
         *
         * @param mapIndex
         *    Index of the map.
         * @return
         *    Metadata for the map.
         */         
        virtual GiftiMetaData* getMapMetaData(const int32_t mapIndex) = 0;
        
        /**
         * @return Is the data in the file mapped to colors using
         * a palette.
         */
        virtual bool isMappedWithPalette() const = 0;
        
        /**
         * Get statistics describing the distribution of data
         * mapped with a color palette at the given index.
         *
         * @param mapIndex
         *    Index of the map.
         * @return
         *    Descriptive statistics for data (will be NULL for data
         *    not mapped using a palette).
         */         
        virtual const DescriptiveStatistics* getMapStatistics(const int32_t mapIndex) = 0;
        
        /**
         * Get the palette color mapping for the map at the given index.
         *
         * @param mapIndex
         *    Index of the map.
         * @return
         *    Palette color mapping for the map (will be NULL for data
         *    not mapped using a palette).
         */         
        virtual PaletteColorMapping* getMapPaletteColorMapping(const int32_t mapIndex) = 0;
        
        /**
         * Get the palette color mapping for the map at the given index.
         *
         * @param mapIndex
         *    Index of the map.
         * @return
         *    Palette color mapping for the map (constant) (will be NULL for data
         *    not mapped using a palette).
         */         
        virtual const PaletteColorMapping* getMapPaletteColorMapping(const int32_t mapIndex) const = 0;
        
        /**
         * @return Is the data in the file mapped to colors using
         * a label table.
         */
        virtual bool isMappedWithLabelTable() const = 0;
        
        /**
         * Get the label table for the map at the given index.
         *
         * @param mapIndex
         *    Index of the map.
         * @return
         *    Label table for the map (will be NULL for data
         *    not mapped using a label table).
         */         
        virtual GiftiLabelTable* getMapLabelTable(const int32_t mapIndex) = 0;
        
        /**
         * Get the label table for the map at the given index.
         *
         * @param mapIndex
         *    Index of the map.
         * @return
         *    Label table for the map (constant) (will be NULL for data
         *    not mapped using a label table).
         */         
        virtual const GiftiLabelTable* getMapLabelTable(const int32_t mapIndex) const = 0;
        
    protected:
        CaretMappableDataFile(const CaretMappableDataFile&);

        CaretMappableDataFile& operator=(const CaretMappableDataFile&);
        
    private:
        void copyCaretMappableDataFile(const CaretMappableDataFile&);
    };
    
#ifdef __CARET_MAPPABLE_DATA_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CARET_MAPPABLE_DATA_FILE_DECLARE__

} // namespace
#endif  //__CARET_MAPPABLE_DATA_FILE__H_
