#ifndef __IDENTIFIED_ITEM_NODE_H__
#define __IDENTIFIED_ITEM_NODE_H__

/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/


#include "IdentifiedItem.h"
#include "StructureEnum.h"

namespace caret {

    class IdentifiedItemNode : public IdentifiedItem {
        
    public:
        IdentifiedItemNode(const AString& text,
                           const StructureEnum::Enum structure,
                           const StructureEnum::Enum contralateralStructure,
                           const int32_t surfaceNumberOfNodes,
                           const int32_t nodeIndex);
        
        virtual ~IdentifiedItemNode();
        
        IdentifiedItemNode(const IdentifiedItemNode& obj);

        IdentifiedItemNode& operator=(const IdentifiedItemNode& obj);
        
        // ADD_NEW_METHODS_HERE

        AString getText() const;
        
        StructureEnum::Enum getStructure() const;
        
        StructureEnum::Enum getContralateralStructure() const;
        
        int32_t getSurfaceNumberOfNodes() const;
        
        int32_t getNodeIndex() const;
        
        const float* getSymbolRGB() const;
        
        const float* getContralateralSymbolRGB() const;
        
        const float getSymbolSize() const;
        
        void setSymbolRGB(const float* rgb);
        
        void setContralateralSymbolRGB(const float* rgb);
        
        void setSymbolSize(const float symbolSize);
        
        virtual AString toString() const;
        
    private:
        void copyHelperIdentifiedItemNode(const IdentifiedItemNode& obj);

        // ADD_NEW_MEMBERS_HERE
        
        const AString m_text;

        StructureEnum::Enum m_structure;
        
        StructureEnum::Enum m_contralateralStructure;
        
        int32_t m_surfaceNumberOfNodes;
        
        int32_t m_nodeIndex;
        
        float m_symbolRGB[3];
        
        float m_contralateralSymbolRGB[3];
        
        float m_symbolSize;
    };
    
#ifdef __IDENTIFIED_ITEM_NODE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __IDENTIFIED_ITEM_NODE_DECLARE__

} // namespace
#endif  //__IDENTIFIED_ITEM_NODE_H__
