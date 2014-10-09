#ifndef __XML_UTILITIES_H__
#define	__XML_UTILITIES_H__

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

#include <AString.h>

#include "CaretObject.h"
#include "XmlSaxParserException.h"

namespace caret {
    
    class XmlUtilities : public CaretObject{
        
    private:
        XmlUtilities();
        
    public:
        ~XmlUtilities();
        
        static AString encodeXmlSpecialCharacters(const AString& text);
        
        static AString decodeXmlSpecialCharacters(const AString& text);
        
        static AString createInvalidRootElementMessage(const AString& validRootElementName,
                                                       const AString& rootElementName);
        
        static AString createInvalidChildElementMessage(const AString& parentElementName,
                                                        const AString& invalidChildElementName);
        
        static AString createInvalidAttributeMessage(const AString& elementName,
                                                     const AString& attributeName,
                                                     const AString& invalidAttributeValue);
        
        static AString createInvalidVersionMessage(const float supportedVersion,
                                                   const float invalidVersion);
        
        static AString createInvalidNumberOfElementsMessage(const AString& elementName,
                                                            const int32_t correctNumberOfElements,
                                                            const int32_t numberOfElements);
        
        static void getArrayOfNumbersFromText(const AString& elementName,
                                              const AString& text,
                                              const int32_t requiredCountOfNumbers,
                                              std::vector<float>& numbersOut);
        
        static void getArrayOfNumbersFromText(const AString& elementName,
                                              const AString& text,
                                              const int32_t requiredCountOfNumbers,
                                              std::vector<int32_t>& numbersOut);
    private:

    };
    
} // namespace

#endif	/* __XML_UTILITIES_H__ */

