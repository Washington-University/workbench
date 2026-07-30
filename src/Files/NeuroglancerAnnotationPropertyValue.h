#ifndef __NEUROGLANCER_ANNOTATION_PROPERTY_VALUE_H__
#define __NEUROGLANCER_ANNOTATION_PROPERTY_VALUE_H__

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

#include "AString.h"
#include "NeuroglancerAnnotationBase.h"
#include "NeuroglancerAnnotationPropertyDataTypeEnum.h"

class QColor;

namespace caret {
    class NeuroglancerAnnotationLabelModel;

    class NeuroglancerAnnotationPropertyValue : public NeuroglancerAnnotationBase {
        
    public:
        NeuroglancerAnnotationPropertyValue(const AString& propertyName,
                                            const NeuroglancerAnnotationPropertyDataTypeEnum::Enum dataType,
                                            const QVariant& value,
                                            const AString& labelText,
                                            const NeuroglancerAnnotationLabelModel* labelModel);
        
        virtual ~NeuroglancerAnnotationPropertyValue();
        
        NeuroglancerAnnotationPropertyValue(const NeuroglancerAnnotationPropertyValue&) = delete;

        NeuroglancerAnnotationPropertyValue& operator=(const NeuroglancerAnnotationPropertyValue&) = delete;
        
        NeuroglancerAnnotationPropertyDataTypeEnum::Enum getDataType() const;
        
        const AString& getDescription() const;
        
        const QVariant& getValue() const;
        
        const AString& getLabelText() const;

        const NeuroglancerAnnotationLabelModel* getLabelModel() const;
        
        static AString QColorToString(const QColor& color);
        
        virtual AString toString() const override;
        
        // ADD_NEW_METHODS_HERE

    private:
        const AString m_description;
        
        const NeuroglancerAnnotationPropertyDataTypeEnum::Enum m_dataType;
        
        const QVariant m_value;
        
        const AString m_labelText;
        
        const NeuroglancerAnnotationLabelModel* m_labelModel;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __NEUROGLANCER_ANNOTATION_PROPERTY_VALUE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __NEUROGLANCER_ANNOTATION_PROPERTY_VALUE_DECLARE__

} // namespace
#endif  //__NEUROGLANCER_ANNOTATION_PROPERTY_VALUE_H__
