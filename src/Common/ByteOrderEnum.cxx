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


#define __BYTE_ORDER_DECLARE__
#include "ByteOrderEnum.h"
#undef __BYTE_ORDER_DECLARE__


using namespace caret;

/**
 * Constructor.
 *
 * @param e
 *    An enumerated value.
 * @param name
 *    Name of enumberated value.
 */
ByteOrderEnum::ByteOrderEnum(
                   const Enum e,
                   const QString& name)
{
    this->e = e;
    this->name = name;
}

/**
 * Destructor.
 */
ByteOrderEnum::~ByteOrderEnum()
{
}

/**
 * Get the system byte order.
 * @return
 *    Byte order of the system.
 */

ByteOrderEnum::Enum 
ByteOrderEnum::getSystemEndian()
{
    ByteOrderEnum::initialize();
    return ByteOrderEnum::systemEndian;
}

/**
 * Is the system byte order little endian?
 * @return
 *    true if system is little endian byte order.
 */
bool 
ByteOrderEnum::isSystemLittleEndian()
{
    ByteOrderEnum::initialize();
    return (ByteOrderEnum::systemEndian == ENDIAN_LITTLE);    
}

/**
 * Is the system byte order big endian?
 * @return
 *    true if system is big endian byte order.
 */
bool 
ByteOrderEnum::isSystemBigEndian()
{
    ByteOrderEnum::initialize();
    return (ByteOrderEnum::systemEndian == ENDIAN_BIG);    
}

void
ByteOrderEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(ByteOrderEnum(ENDIAN_BIG,"ENDIAN_BIG"));
    enumData.push_back(ByteOrderEnum(ENDIAN_LITTLE,"ENDIAN_LITTLE"));
    
    uint32_t intVal = 0x00000001;
    unsigned char* c = (unsigned char*)&intVal;
    
    ByteOrderEnum::systemEndian = ByteOrderEnum::ENDIAN_BIG;
    if (*c == 0x01) systemEndian = ByteOrderEnum::ENDIAN_LITTLE;
}

/**
 * Get the enum value for this enumerated item.
 * @return the value for this enumerated item.
 */
ByteOrderEnum::Enum 
ByteOrderEnum::getEnum() const
{
    return this->e;
}

/**
 * Get the enum name for this enumerated item.
 * @return the name for this enumerated item.
 */
QString
ByteOrderEnum::getName() const
{
    return this->name;
}

/**
 * Find the data for and enumerated value.
 * @param e
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const ByteOrderEnum*
ByteOrderEnum::findData(const Enum e)
{
    initialize();
    int64_t num = enumData.size();
    for (int64_t i = 0; i < num; i++) {
        const ByteOrderEnum* d = &enumData[i];
        if (d->e == e) {
            return d;
        }
    }

    return NULL;
}

/**
 * Get a string representation of the enumerated type.
 * @param e 
 *     Enumerated value.
 * @return 
 *     String representing enumerated value.
 */
QString 
ByteOrderEnum::toName(Enum e) {
    initialize();
    
    QString s;
    
    for (std::vector<ByteOrderEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const ByteOrderEnum& d = *iter;
        if (d.e == e) {
            s = d.name;
            break;
        }
    }

    return s;
}

/**
 * Get an enumerated value corresponding to its name.
 * @param s 
 *     Name of enumerated value.
 * @param isValidOut 
 *     If not NULL, it is set indicating that a
 *     enum value exists for the input name.
 * @return 
 *     Enumerated value.
 */
ByteOrderEnum::Enum 
ByteOrderEnum::fromName(const QString& s, bool* isValidOut)
{
    initialize();
    
    bool validFlag = false;
    Enum e = ENDIAN_LITTLE;
    
    for (std::vector<ByteOrderEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const ByteOrderEnum& d = *iter;
        if (d.name == s) {
            e = d.e;
            validFlag = true;
            break;
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    return e;
}
