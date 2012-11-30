#ifndef __IDENTIFICATION_MANAGER_H__
#define __IDENTIFICATION_MANAGER_H__

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

#include <list>

#include "CaretColorEnum.h"
#include "SceneableInterface.h"
#include "StructureEnum.h"

namespace caret {

    class Brain;
    class IdentifiedItem;
    class IdentifiedItemNode;
    class SceneClassAssistant;
    
    class IdentificationManager : public SceneableInterface {
        
    public:
        IdentificationManager(Brain* brain);
        
        virtual ~IdentificationManager();
        
        void addIdentifiedItem(IdentifiedItem* item);
        
        AString getIdentificationText() const;
        
        std::vector<IdentifiedItemNode> getNodeIdentifiedItemsForSurface(const StructureEnum::Enum structure,
                                                                         const int32_t surfaceNumberOfNodes) const;
        
        void removeIdentifiedNodeItem(const StructureEnum::Enum structure,
                                      const int32_t surfaceNumberOfNodes,
                                      const int32_t nodeIndex);
        
        void removeIdentificationText();
        
        void removeAllIdentifiedItems();
        
        void removeAllIdentifiedNodes();
        
        bool isContralateralIdentificationEnabled() const;
        
        void setContralateralIdentificationEnabled(const bool enabled);
        
        bool isVolumeIdentificationEnabled() const;
        
        void setVolumeIdentificationEnabled(const bool enabled);
        
        float getIdentificationSymbolSize() const;
        
        void setIdentificationSymbolSize(const float symbolSize);
        
        float getMostRecentIdentificationSymbolSize() const;
        
        void setMostRecentIdentificationSymbolSize(const float symbolSize);
        
        CaretColorEnum::Enum getIdentificationSymbolColor() const;
        
        void setIdentificationSymbolColor(const CaretColorEnum::Enum color);
        
        CaretColorEnum::Enum getIdentificationContralateralSymbolColor() const;
        
        void setIdentificationContralateralSymbolColor(const CaretColorEnum::Enum color);
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
    private:
        IdentificationManager(const IdentificationManager&);

        IdentificationManager& operator=(const IdentificationManager&);
        
    public:

        // ADD_NEW_METHODS_HERE

    private:

        void addIdentifiedItemPrivate(IdentifiedItem* item);
        
        // ADD_NEW_MEMBERS_HERE

        SceneClassAssistant* m_sceneAssistant;
        
        Brain* m_brain;
        
        std::list<IdentifiedItem*> m_identifiedItems;
        
        AString m_previousIdentifiedItemsText;
        
        IdentifiedItem* m_mostRecentIdentifiedItem;
        
        bool m_contralateralIdentificationEnabled;
        
        bool m_volumeIdentificationEnabled;
        
        float m_identifcationSymbolSize;
        
        float m_identifcationMostRecentSymbolSize;
        
        CaretColorEnum::Enum m_identificationSymbolColor;
        
        CaretColorEnum::Enum m_identificationContralateralSymbolColor;
        
    };
    
#ifdef __IDENTIFICATION_MANAGER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __IDENTIFICATION_MANAGER_DECLARE__

} // namespace
#endif  //__IDENTIFICATION_MANAGER_H__
