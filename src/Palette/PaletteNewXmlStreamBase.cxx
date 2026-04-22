
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

#define __PALETTE_NEW_XML_STREAM_BASE_DECLARE__
#include "PaletteNewXmlStreamBase.h"
#undef __PALETTE_NEW_XML_STREAM_BASE_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::PaletteNewXmlStreamBase 
 * \brief Base class for PaletteNew stream reader and writer
 * \ingroup Palette
 */

/**
 * Constructor.
 */
PaletteNewXmlStreamBase::PaletteNewXmlStreamBase()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
PaletteNewXmlStreamBase::~PaletteNewXmlStreamBase()
{
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
PaletteNewXmlStreamBase::toString() const
{
    return "PaletteNewXmlStreamBase";
}

