#ifndef __BRAIN_STRUCTURE_NODE_ATTRIBUTE__H_
#define __BRAIN_STRUCTURE_NODE_ATTRIBUTE__H_

/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
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


#include "CaretObject.h"
#include "NodeIdentificationTypeEnum.h"

namespace caret {

    class BrainStructureNodeAttributes : public CaretObject {
        
    public:
        BrainStructureNodeAttributes();
        
        virtual ~BrainStructureNodeAttributes();
        
        NodeIdentificationTypeEnum::Enum getIdentificationType(const int32_t nodeNumber) const;
        
        void setIdentificationType(const int32_t nodeNumber,
                                   const NodeIdentificationTypeEnum::Enum identificationType);
        
        void setAllIdentificationNone();
        
        void update(const int32_t numberOfNodes);
        
    private:
        BrainStructureNodeAttributes(const BrainStructureNodeAttributes&);

        BrainStructureNodeAttributes& operator=(const BrainStructureNodeAttributes&);
        
    public:
        virtual AString toString() const;
        
    private:
        std::vector<NodeIdentificationTypeEnum::Enum> identificationType;
    };
    
#ifdef __BRAIN_STRUCTURE_NODE_ATTRIBUTE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_STRUCTURE_NODE_ATTRIBUTE_DECLARE__

} // namespace
#endif  //__BRAIN_STRUCTURE_NODE_ATTRIBUTE__H_
