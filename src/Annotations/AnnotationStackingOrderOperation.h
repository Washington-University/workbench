#ifndef __ANNOTATION_STACKING_ORDER_OPERATION_H__
#define __ANNOTATION_STACKING_ORDER_OPERATION_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

    class Annotation;
    
    class AnnotationStackingOrderOperation : public CaretObject {
        
    public:
        enum class OrderType {
            BRING_TO_FRONT,
            BRING_FORWARD,
            SEND_TO_BACK,
            SEND_BACKWARD
        };
        
        AnnotationStackingOrderOperation(const std::vector<Annotation*>& annotations,
                                         const Annotation* selectedAnnotation);
        
        virtual ~AnnotationStackingOrderOperation();
        
        AnnotationStackingOrderOperation(const AnnotationStackingOrderOperation&) = delete;

        AnnotationStackingOrderOperation& operator=(const AnnotationStackingOrderOperation&) = delete;
        
        bool runOrdering(const OrderType orderType,
                         AString& errorMessageOut);


        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        /**
         * Sort annotations by stack order
         */
        class OrderInfo {
        public:
            OrderInfo(Annotation* annotation,
                    const int32_t stackOrder)
            : m_annotation(annotation),
            m_stackOrder(stackOrder) { }

            bool operator<(const OrderInfo& orderInfo) const {
                return m_stackOrder < orderInfo.m_stackOrder;
            }
            
            Annotation* m_annotation = NULL;
            int32_t m_stackOrder = -1;
        };
        
        bool filterAnnotations();
        
        bool validateCompatibility(const std::vector<Annotation*>& annotations,
                                   AString& errorMesssageOut);
        
        std::vector<Annotation*> m_annotations;
        
        const Annotation* m_selectedAnnotation;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_STACKING_ORDER_OPERATION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_STACKING_ORDER_OPERATION_DECLARE__

} // namespace
#endif  //__ANNOTATION_STACKING_ORDER_OPERATION_H__
