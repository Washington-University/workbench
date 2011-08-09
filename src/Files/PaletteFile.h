#ifndef __PALETTEFILE_H__
#define __PALETTEFILE_H__

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


#include "DataFile.h"

#include <stdint.h>

#include <QString>
#include <vector>

#include "GiftiLabelTable.h"

namespace caret {

    class GiftiLabel;
    class GiftiLabelTable;
    class Palette;
    /**
     * File for storing color palettes.
     */
    class PaletteFile : public DataFile {
        
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
                      const QString& name,
                      const int32_t red,
                      const int32_t green,
                      const int32_t blue);
        
        void addColor(
                      const QString& name,
                      const int32_t rgb[]);
        
        const GiftiLabel* getColor(const int32_t index) const;
        
        const GiftiLabel* getColorByName(const QString& colorName) const;
        
        int32_t getColorIndex(const QString& colorName) const;
        
        int32_t getNumberOfPalettes() const;
        
        void addPalette(const Palette& p);
        
        Palette* getPalette(const int32_t index) const;
        
        Palette* getPaletteByName(const QString& name) const;
        
        void removePalette(const int32_t index);
        
        virtual bool isEmpty() const;
        
        QString toString() const;
        
        bool isModified() const;
        
        void clearModified();
        
        virtual void readFile(const QString& filename) throw (DataFileException);
        
        virtual void writeFile(const QString& filename) throw (DataFileException);
        
    private:
        void assignColorsToPalette(Palette& p);
        
        void addDefaultPalettes();
        
        void clearAll();
        
    private:
        /**the colors for the palettes */
        GiftiLabelTable labelTable;
        
        /**the palettes */
        std::vector<Palette*> palettes;
        
    };

} // namespace

#endif // __PALETTEFILE_H__
