#ifndef __GIFTI_META_DATA_ELEMENT_VALUES_H__
#define __GIFTI_META_DATA_ELEMENT_VALUES_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2023 Washington University School of Medicine
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

#include <QStringList>

#include "CaretObject.h"
#include "GiftiMetaDataElementDataTypeEnum.h"


namespace caret {

    class GiftiMetaDataElementValues : public CaretObject {
        
    public:
        static GiftiMetaDataElementDataTypeEnum::Enum getDataTypeForElement(const QString& metaDataName);
        
        static QStringList getValuesForElement(const QString& metaDataName);
        
        virtual ~GiftiMetaDataElementValues();
        
        GiftiMetaDataElementValues(const GiftiMetaDataElementValues&) = delete;

        GiftiMetaDataElementValues& operator=(const GiftiMetaDataElementValues&) = delete;
        

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        GiftiMetaDataElementValues();
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __GIFTI_META_DATA_ELEMENT_VALUES_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __GIFTI_META_DATA_ELEMENT_VALUES_DECLARE__

} // namespace
#endif  //__GIFTI_META_DATA_ELEMENT_VALUES_H__
