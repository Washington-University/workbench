
/*LICENSE_START*/
/*
 * Copyright 2014 Washington University,
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

#define __EVENT_PALETTE_GET_BY_NAME_DECLARE__
#include "EventPaletteGetByName.h"
#undef __EVENT_PALETTE_GET_BY_NAME_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventTypeEnum.h"
#include "Palette.h"

using namespace caret;


    
/**
 * \class caret::EventPaletteGetByName 
 * \brief Find a palette by name
 * \ingroup Files
 */

/**
 * Constructor.
 */
EventPaletteGetByName::EventPaletteGetByName(const AString& paletteName)
: Event(EventTypeEnum::EVENT_PALETTE_GET_BY_NAME),
m_paletteName(paletteName),
m_palette(NULL)
{
}

/**
 * Destructor.
 */
EventPaletteGetByName::~EventPaletteGetByName()
{
}

/**
 * @return Name of desired palette.
 */
AString
EventPaletteGetByName::getPaletteName() const
{
    return m_paletteName;
}

/**
 * @return Palette that was found (NULL if no matching palette was found).
 */
Palette*
EventPaletteGetByName::getPalette() const
{
    return m_palette;
}

/**
 * Set the palette that matches by name.
 *
 * @param palette
 *    Palette that matches name of desired palette.
 */
void
EventPaletteGetByName::setPalette(Palette* palette)
{
    CaretAssert(palette);
    
    if (palette->getName() != m_paletteName) {
        CaretAssertMessage(0, "Palette name does not match!");
        CaretLogSevere("Palette name does not match!");
        return;
    }
    
    if (m_palette != NULL) {
        CaretLogWarning("More that one palette with name "
                        + m_paletteName);
    }
    
    m_palette = palette;
}

