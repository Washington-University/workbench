#ifndef __PALETTE_H__
#define __PALETTE_H__

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


#include "CaretObject.h"
#include "TracksModificationInterface.h"

#include <stdint.h>

#include <QString>
#include <vector>

namespace caret {

    class PaletteScalarAndColor;

    /**
     * A color palette.
     */
    class Palette : public CaretObject, TracksModificationInterface {
        
    public:
        Palette();
        
        Palette(const Palette& p);
        
        Palette& operator=(const Palette& o);
        
        virtual ~Palette();
        
    private:
        void copyHelper(const Palette& o);
        
        void initializeMembersPalette();
        
    public:
        QString toString() const;
        
        QString getName() const;
        
        void setName(const QString& name);
        
        int32_t getNumberOfScalarsAndColors() const;
        
        PaletteScalarAndColor* getScalarAndColor(const int32_t index) const;
        
        void addScalarAndColor(
                               const float scalar,
                               const QString& colorName);
        
        void insertScalarAndColor(
                                  const PaletteScalarAndColor& psac,
                                  const int32_t insertAfterIndex);
        
        void removeScalarAndColor(const int32_t index);
        
        void getMinMax(float& minOut, float& maxOut) const;
        
        void getPaletteColor(
                             const float scalar,
                             const bool interpolateColorFlag,
                             float rgbaOut[4]) const;
        
        void setModified();
        
        void clearModified();
        
        bool isModified() const;
        
    public:
        /**Name of gray interpolate palette */
        static  const QString GRAY_INTERP_PALETTE_NAME;
        
        /**"none" color name. */
        static  const QString NONE_COLOR_NAME;
        
    private:
        /**has this object been modified. (DO NOT CLONE) */
        bool modifiedFlag;
        
        /**Name of the palette. */
        QString name;
        
        /**The scalars in the palette. */
        std::vector<PaletteScalarAndColor*> paletteScalars;
        
    };

    
#ifdef __PALETTE_DEFINE__
    const QString Palette::GRAY_INTERP_PALETTE_NAME = "Gray_Interp";
    const QString Palette::NONE_COLOR_NAME = "none";
#endif // __PALETTE_DEFINE__
} // namespace

#endif // __PALETTE_H__
