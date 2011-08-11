
#ifndef __XML_ATTRIBUTES_H__
#define	__XML_ATTRIBUTES_H__

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

#include <AString.h>
#include <vector>

#include "CaretObject.h"
#include <stdint.h>

namespace caret {
    
/**
 * Maintains attribute for XML Generic Writer.
 */
class XmlAttributes : public CaretObject{

public:
    XmlAttributes();
    
    ~XmlAttributes();
    
    void clear();
    
   void addAttribute(const AString& name, const AString& value);

   void addAttribute(const AString& name, const int32_t value);

    void addAttribute(const AString& name, const std::vector<int32_t>& value, const AString& separator);
    
    void addAttribute(const AString& name, const std::vector<int64_t>& value, const AString& separator);
    
   void addAttribute(const AString& name, const int64_t value);

    void addAttribute(const AString& name, const float value);

    void addAttribute(const AString& name, const double value);
    

    int getNumberOfAttributes() const;

    AString getName(const int index) const;

    AString getValue(const int index) const;

    int32_t getValueAsInt(const int index) const;
    
    float getValueAsFloat(const int index) const;
    
    AString getValue(const AString& name) const;
    
    int32_t getValueAsInt(const AString& name) const;
    
    float getValueAsFloat(const AString& name) const;
    
private:
   /** attribute names. */
    std::vector<AString> names;

   /** attribute values. */
    std::vector<AString> values;
};

} // namespace

#endif	/* __XML_ATTRIBUTES_H__ */

