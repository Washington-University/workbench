#ifndef __PALETTEFILE_H__
#define __PALETTEFILE_H__

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


#include "CaretDataFile.h"

#include <stdint.h>

#include <AString.h>
#include <vector>

#include "GiftiLabelTable.h"

namespace caret {

    class GiftiLabel;
    class GiftiLabelTable;
    class GiftiMetaData;
    class Palette;
    class PaletteColorMapping;
    
    /**
     * File for storing color palettes.
     */
    class PaletteFile : public CaretDataFile {
        
    public:
        PaletteFile();
        
        virtual ~PaletteFile();
        
    public:
        PaletteFile(const PaletteFile& o);
        
        PaletteFile& operator=(const PaletteFile& o);
        
        
    private:
        void initializeMembersPaletteFile();
        
    public:
        GiftiLabelTable* getLabelTable();
        
        void clear();
        
        void addColor(const GiftiLabel& pc);
        
        void addColor(
                      const AString& name,
                      const int32_t red,
                      const int32_t green,
                      const int32_t blue);
        
        void addColor(
                      const AString& name,
                      const int32_t rgb[]);
        
        void addPaletteScalarAndColor(Palette& palette,
                                      const float scalar,
                                      const int32_t red,
                                      const int32_t green,
                                      const int32_t blue);

        void addPaletteScalarAndColorFloat(Palette& palette,
                                           const float scalar,
                                           const float red,
                                           const float green,
                                           const float blue);
        
        const GiftiLabel* getColor(const int32_t index) const;
        
        const GiftiLabel* getColorByName(const AString& colorName) const;
        
        int32_t getColorIndex(const AString& colorName) const;
        
        int32_t getNumberOfPalettes() const;
        
        void addPalette(const Palette& p);
        
        Palette* getPalette(const int32_t index) const;
        
        Palette* getPaletteByName(const AString& name) const;
        
        void removePalette(const int32_t index);
        
        virtual bool isEmpty() const;
        
        AString toString() const;
        
        bool isModified() const;
        
        void clearModified();
        
        virtual void readFile(const AString& filename);
        
        virtual void writeFile(const AString& filename);
        
        virtual StructureEnum::Enum getStructure() const;
        
        virtual void setStructure(const StructureEnum::Enum structure);
        
        DataFileTypeEnum::Enum getDataFileType() const;
        
        virtual GiftiMetaData* getFileMetaData();
        
        virtual const GiftiMetaData* getFileMetaData() const;
        
        static void setDefaultPaletteColorMapping(PaletteColorMapping* paletteColorMapping,
                                                  const DataFileTypeEnum::Enum& dataFileType,
                                                  const AString& fileName,
                                                  const AString& dataName,
                                                  const float* data,
                                                  const int32_t numberOfDataElements);
    private:
        void assignColorsToPalette(Palette& p);
        
        void addDefaultPalettes();
        
        void clearAll();
        
    private:
        /**the colors for the palettes */
        GiftiLabelTable labelTable;
        
        /**the palettes */
        std::vector<Palette*> palettes;
        
        GiftiMetaData* metadata;
        
    };

} // namespace

#endif // __PALETTEFILE_H__
