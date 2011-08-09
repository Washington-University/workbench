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
#include <QString>

namespace caret {

/**
 * Byte order.
 */
class ByteOrderEnum {

public:
    /**  ENDIAN Types */
    enum Enum {
        /**  */
        ENDIAN_BIG,
        /**  */
        ENDIAN_LITTLE
    };


    ~ByteOrderEnum();

    static ByteOrderEnum::Enum getSystemEndian();
    
    static bool isSystemLittleEndian();
    
    static bool isSystemBigEndian();
    
    static QString toName(Enum e);
    
    static Enum fromName(const QString& s, bool* isValidOut);

private:
    ByteOrderEnum(const Enum e, const QString& name);

    Enum getEnum() const;
    
    QString getName() const;
    
    static std::vector<ByteOrderEnum> enumData;

    static void initialize();

    static bool initializedFlag;

    static Enum systemEndian;
    
    Enum e;

    QString name;

    static const ByteOrderEnum* findData(const Enum e);

};

#ifdef __BYTE_ORDER_DECLARE__
    std::vector<ByteOrderEnum> ByteOrderEnum::enumData;
    bool ByteOrderEnum::initializedFlag = false;
    ByteOrderEnum::Enum ByteOrderEnum::systemEndian;
#endif // __BYTE_ORDER_DECLARE__

} // namespace

#endif // __BYTE_ORDER_H__
