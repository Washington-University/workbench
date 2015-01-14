#ifndef __PALETTESCALARANDCOLOR_H__
#define __PALETTESCALARANDCOLOR_H__

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


#include "CaretObject.h"
#include "TracksModificationInterface.h"

#include <stdint.h>
#include <vector>


namespace caret {

/**
 * Contains information about the color assigned to the scalar
 * value.  A palette contains a set of these objects.
 */
class PaletteScalarAndColor : public CaretObject, TracksModificationInterface {

public:
    PaletteScalarAndColor(const float scalar,
                          const AString& colorName);

    PaletteScalarAndColor(const PaletteScalarAndColor& o);

    PaletteScalarAndColor& operator=(const PaletteScalarAndColor& o);

    virtual ~PaletteScalarAndColor();

private:
    void copyHelper(const PaletteScalarAndColor& o);

    void initializeMembersPaletteScalarAndColor();

public:
    /**
     * @return The scalar
     */
    inline float getScalar() const { return this->scalar; }

    void setScalar(const float scalar);

    /**
     * Get the name of the color.
     * @return
     *   Name of the color assigned to the scalar.
     */
    inline const AString& getColorName() const {
        return this->colorName; 
    }
    
    void setColorName(const AString& colorName);
                      
    /**
     * @return float array with red, green, blue, alpha color components ranging 0 to 1.
     */
    inline const float* getColor() const { return rgba; }
    
    void getColor(float rgbaOut[4]) const;
    
    void setColor(const float rgba[4]);
    
    AString toString() const;

    void setModified();

    void clearModified();

    bool isModified() const;

    /**
     * @return Is the color the 'none' color meaning
     * that no coloring is applied?
     */
    inline bool isNoneColor() const { return this->noneColorFlag; }

private:
    /** has this object been modified. (DO NOT CLONE) */
    bool modifiedFlag;

    /** the scalar value */
    float scalar;

    /** the color's name, use the setName() method so that none color flag is updated */
    AString colorName;
    
    /** the color's rgba components */
    float rgba[4];
    
    bool noneColorFlag;

};

} // namespace

#endif // __PALETTESCALARANDCOLOR_H__
