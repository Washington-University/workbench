#ifndef __CIFTI_SCALAR_FILE__H_
#define __CIFTI_SCALAR_FILE__H_

/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/


#include "CaretMappableDataFile.h"
#include "EventListenerInterface.h"

namespace caret {

    class ConnectivityLoaderFile;
    class Palette;
    
    class CiftiScalarFile : public CaretMappableDataFile, public EventListenerInterface {
        
    public:
        CiftiScalarFile();
        
        virtual ~CiftiScalarFile();
        
        void receiveEvent(Event* event);
        
        /**
         * Is the file empty (contains no data)?
         *
         * @return
         *    true if the file is empty, else false.
         */
        virtual bool isEmpty() const;
        
        virtual void clear();
        
        /**
         * Read the data file.
         *
         * @param filename
         *    Name of the data file.
         * @throws DataFileException
         *    If the file was not successfully read.
         */
        virtual void readFile(const AString& filename) throw (DataFileException);
        
        /**
         * Write the data file.
         *
         * @param filename
         *    Name of the data file.
         * @throws DataFileException
         *    If the file was not successfully written.
         */
        virtual void writeFile(const AString& filename) throw (DataFileException);
        
        /**
         * @return The structure for this file.
         */
        virtual StructureEnum::Enum getStructure() const;
        
        /**
         * Set the structure for this file.
         * @param structure
         *   New structure for this file.
         */
        virtual void setStructure(const StructureEnum::Enum structure);
        
        /**
         * @return Get access to the file's metadata.
         */
        virtual GiftiMetaData* getFileMetaData();
        
        /**
         * @return Get access to unmodifiable file's metadata.
         */
        virtual const GiftiMetaData* getFileMetaData() const;
        
        /**
         * @return Is the data mappable to a surface?
         */
        virtual bool isSurfaceMappable() const;
        
        /**
         * @return Is the data mappable to a volume?
         */
        virtual bool isVolumeMappable() const;
        
        /**
         * @return The number of maps in the file.
         * Note: Caret5 used the term 'columns'.
         */
        virtual int32_t getNumberOfMaps()const ;
        
        /**
         * Get the name of the map at the given index.
         *
         * @param mapIndex
         *    Index of the map.
         * @return
         *    Name of the map.
         */
        virtual AString getMapName(const int32_t mapIndex) const;
        
        /**
         * Set the name of the map at the given index.
         *
         * @param mapIndex
         *    Index of the map.
         * @param mapName
         *    New name for the map.
         */
        virtual void setMapName(const int32_t mapIndex,
                                const AString& mapName);
        
        /**
         * Get the metadata for the map at the given index
         *
         * @param mapIndex
         *    Index of the map.
         * @return
         *    Metadata for the map (const value).
         */
        virtual const GiftiMetaData* getMapMetaData(const int32_t mapIndex) const;
        
        /**
         * Get the metadata for the map at the given index
         *
         * @param mapIndex
         *    Index of the map.
         * @return
         *    Metadata for the map.
         */
        virtual GiftiMetaData* getMapMetaData(const int32_t mapIndex);
        
        /**
         * Get the unique ID (UUID) for the map at the given index.
         *
         * @param mapIndex
         *    Index of the map.
         * @return
         *    String containing UUID for the map.
         */
        virtual AString getMapUniqueID(const int32_t mapIndex) const;
        
        /**
         * @return Is the data in the file mapped to colors using
         * a palette.
         */
        virtual bool isMappedWithPalette() const;
        
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
        virtual const DescriptiveStatistics* getMapStatistics(const int32_t mapIndex);
        
        virtual const FastStatistics* getMapFastStatistics(const int32_t mapIndex);
        
        virtual const Histogram* getMapHistogram(const int32_t mapIndex);
        
        /**
         * Get statistics describing the distribution of data
         * mapped with a color palette at the given index for
         * data within the given ranges.
         *
         * @param mapIndex
         *    Index of the map.
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
         * @return
         *    Descriptive statistics for data (will be NULL for data
         *    not mapped using a palette).
         */
        virtual const DescriptiveStatistics* getMapStatistics(const int32_t mapIndex,
                                                              const float mostPositiveValueInclusive,
                                                              const float leastPositiveValueInclusive,
                                                              const float leastNegativeValueInclusive,
                                                              const float mostNegativeValueInclusive,
                                                              const bool includeZeroValues);
        
        virtual const Histogram* getMapHistogram(const int32_t mapIndex,
                                                 const float mostPositiveValueInclusive,
                                                 const float leastPositiveValueInclusive,
                                                 const float leastNegativeValueInclusive,
                                                 const float mostNegativeValueInclusive,
                                                 const bool includeZeroValues);
        
        /**
         * Get the palette color mapping for the map at the given index.
         *
         * @param mapIndex
         *    Index of the map.
         * @return
         *    Palette color mapping for the map (will be NULL for data
         *    not mapped using a palette).
         */
        virtual PaletteColorMapping* getMapPaletteColorMapping(const int32_t mapIndex);
        
        /**
         * Get the palette color mapping for the map at the given index.
         *
         * @param mapIndex
         *    Index of the map.
         * @return
         *    Palette color mapping for the map (constant) (will be NULL for data
         *    not mapped using a palette).
         */
        virtual const PaletteColorMapping* getMapPaletteColorMapping(const int32_t mapIndex) const;
        
        /**
         * @return Is the data in the file mapped to colors using
         * a label table.
         */
        virtual bool isMappedWithLabelTable() const;
        
        /**
         * Get the label table for the map at the given index.
         *
         * @param mapIndex
         *    Index of the map.
         * @return
         *    Label table for the map (will be NULL for data
         *    not mapped using a label table).
         */
        virtual GiftiLabelTable* getMapLabelTable(const int32_t mapIndex);
        
        /**
         * Get the label table for the map at the given index.
         *
         * @param mapIndex
         *    Index of the map.
         * @return
         *    Label table for the map (constant) (will be NULL for data
         *    not mapped using a label table).
         */
        virtual const GiftiLabelTable* getMapLabelTable(const int32_t mapIndex) const;
        
        bool getSurfaceNodeColoring(const StructureEnum::Enum structure,
                                    const int32_t mapIndex,
                                    const Palette* palette,
                                    float* nodeRGBA,
                                    const int32_t numberOfNodes);
        
    private:
        CiftiScalarFile(const CiftiScalarFile&);

        CiftiScalarFile& operator=(const CiftiScalarFile&);
        
    public:

        // ADD_NEW_METHODS_HERE

    private:

        void clearPrivate();
        
        ConnectivityLoaderFile* m_ciftiFile;
        
        bool m_isSurfaceMappable;
        
        bool m_isVolumeMappable;
        
        AString m_uniqueID;
        
        bool m_isColoringValid;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CIFTI_SCALAR_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CIFTI_SCALAR_FILE_DECLARE__

} // namespace
#endif  //__CIFTI_SCALAR_FILE__H_
