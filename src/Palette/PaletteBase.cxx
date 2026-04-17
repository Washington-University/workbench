
/*LICENSE_START*/
/*
 *  Copyright (C) 2026 Washington University School of Medicine
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

#define __PALETTE_BASE_DECLARE__
#include "PaletteBase.h"
#undef __PALETTE_BASE_DECLARE__

#include "CaretAssert.h"
using namespace caret;


static bool sortByName(const PaletteBase* lhs, const PaletteBase* rhs) {
    const int result(QString::compare(lhs->getName(),
                                      rhs->getName(),
                                      Qt::CaseInsensitive));
    return (result < 0);
};


    
/**
 * \class caret::PaletteBase 
 * \brief Base class for palettes
 * \ingroup Palette
 */

/**
 * Constructor.
 * @param paletteDesignType
 *    Design type of palette
 */
PaletteBase::PaletteBase(const PaletteDesignTypeEnum::Enum paletteDesignType)
: CaretObject(),
m_paletteDesignType(paletteDesignType)
{
    
}

/**
 * Destructor.
 */
PaletteBase::~PaletteBase()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
PaletteBase::PaletteBase(const PaletteBase& obj)
: CaretObject(obj),
m_paletteDesignType(obj.m_paletteDesignType)
{
    this->copyHelperPaletteBase(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
PaletteBase&
PaletteBase::operator=(const PaletteBase& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperPaletteBase(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
PaletteBase::copyHelperPaletteBase(const PaletteBase& /*obj*/)
{
    
}

/**
 * @rerturn The type of palette
 */
PaletteDesignTypeEnum::Enum
PaletteBase::getPaletteDesignType() const
{
    return m_paletteDesignType;
}

/**
 * Sort the palettes by name
 * @param palettes
 *   Palettes that are sorted
 */
void
PaletteBase::sortByName(std::vector<const PaletteBase*>& palettes)
{
    std::sort(palettes.begin(),
              palettes.end(),
              [](const PaletteBase* lhs, const PaletteBase* rhs) {
        const int result(QString::compare(lhs->getName(),
                                          rhs->getName(),
                                          Qt::CaseInsensitive));
        return (result < 0); });
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
PaletteBase::toString() const
{
    return "PaletteBase";
}

