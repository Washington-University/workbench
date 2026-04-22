#ifndef __PALETTE_NEW_XML_STREAM_BASE_H__
#define __PALETTE_NEW_XML_STREAM_BASE_H__

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



#include <memory>

#include "CaretObject.h"



namespace caret {

    class PaletteNewXmlStreamBase : public CaretObject {
        
    public:
        PaletteNewXmlStreamBase();
        
        virtual ~PaletteNewXmlStreamBase();
        
        PaletteNewXmlStreamBase(const PaletteNewXmlStreamBase&) = delete;

        PaletteNewXmlStreamBase& operator=(const PaletteNewXmlStreamBase&) = delete;
        

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        static const QString ELEMENT_PALETTE;
        
        static const QString ELEMENT_NEGATIVE_RANGE;
        
        static const QString ELEMENT_POSITIVE_RANGE;
        
        static const QString ELEMENT_ZERO_COLOR;
        
        static const QString ELEMENT_SCALAR_COLOR;
        
        static const QString ATTRIBUTE_NAME;
        
        static const QString ATTRIBUTE_VERSION;
        
        static const QString ATTRIBUTE_SCALAR;
        
        static const QString ATTRIBUTE_RED;
        
        static const QString ATTRIBUTE_GREEN;
        
        static const QString ATTRIBUTE_BLUE;
        
        static const int SCALAR_PRECISION;
        
        static const int RGB_PRECISION;
        
    private:
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __PALETTE_NEW_XML_STREAM_BASE_DECLARE__
     const QString PaletteNewXmlStreamBase::ELEMENT_PALETTE = "Palette";
    
     const QString PaletteNewXmlStreamBase::ELEMENT_NEGATIVE_RANGE = "NegativeRange";
    
     const QString PaletteNewXmlStreamBase::ELEMENT_POSITIVE_RANGE = "PositiveRange";
    
     const QString PaletteNewXmlStreamBase::ELEMENT_ZERO_COLOR = "ZeroColor";
    
     const QString PaletteNewXmlStreamBase::ELEMENT_SCALAR_COLOR = "ScalarColor";
    
     const QString PaletteNewXmlStreamBase::ATTRIBUTE_NAME = "Name";
    
     const QString PaletteNewXmlStreamBase::ATTRIBUTE_VERSION = "Version";
    
     const QString PaletteNewXmlStreamBase::ATTRIBUTE_SCALAR = "Scalar";
    
     const QString PaletteNewXmlStreamBase::ATTRIBUTE_RED = "Red";
    
     const QString PaletteNewXmlStreamBase::ATTRIBUTE_GREEN = "Green";
    
     const QString PaletteNewXmlStreamBase::ATTRIBUTE_BLUE = "Blue";
    
     const int PaletteNewXmlStreamBase::SCALAR_PRECISION = 5;
    
     const int PaletteNewXmlStreamBase::RGB_PRECISION = 3;
    

    
#endif // __PALETTE_NEW_XML_STREAM_BASE_DECLARE__

} // namespace
#endif  //__PALETTE_NEW_XML_STREAM_BASE_H__
