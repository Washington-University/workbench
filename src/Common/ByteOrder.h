#ifndef __BYTE_ORDER_H__
#define __BYTE_ORDER_H__

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



#include <stdint.h>

#include <vector>
#include <string>

namespace caret {

/**
 * Byte order.
 */
class ByteOrder {

public:
    /**  ENDIAN Types */
    enum Enum {
        /**  */
        ENDIAN_BIG,
        /**  */
        ENDIAN_LITTLE
    };


    ~ByteOrder();

    static ByteOrder::Enum getSystemEndian();
    
    static bool isSystemLittleEndian();
    
    static bool isSystemBigEndian();
    
    static std::string toName(Enum e);
    
    static Enum fromName(const std::string& s, bool* isValidOut);

private:
    ByteOrder(const Enum e, const std::string& name);

    Enum getEnum() const;
    
    std::string getName() const;
    
    static std::vector<ByteOrder> enumData;

    static void initialize();

    static bool initializedFlag;

    static Enum systemEndian;
    
    Enum e;

    std::string name;

    static const ByteOrder* findData(const Enum e);

};

#ifdef __BYTE_ORDER_DECLARE__
    std::vector<ByteOrder> ByteOrder::enumData;
    bool ByteOrder::initializedFlag = false;
    ByteOrder::Enum ByteOrder::systemEndian;
#endif // __BYTE_ORDER_DECLARE__

} // namespace

#endif // __BYTE_ORDER_H__
