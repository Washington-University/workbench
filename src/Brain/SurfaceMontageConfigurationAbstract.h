#ifndef __SURFACE_MONTAGE_CONFIGURATION_ABSTRACT_H__
#define __SURFACE_MONTAGE_CONFIGURATION_ABSTRACT_H__

/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
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


#include "CaretObject.h"

#include "SceneableInterface.h"
#include "SurfaceMontageConfigurationTypeEnum.h"
#include "SurfaceMontageLayoutOrientationEnum.h"

namespace caret {
    class OverlaySet;
    class SceneClassAssistant;

    class SurfaceMontageConfigurationAbstract : public CaretObject, public SceneableInterface {
        
    public:
        enum SupportLayoutOrientation {
            SUPPORTS_LAYOUT_ORIENTATION_YES,
            SUPPORTS_LAYOUT_ORIENTATION_NO,
        };
        
        SurfaceMontageConfigurationAbstract(const SurfaceMontageConfigurationTypeEnum::Enum configuration,
                                            const SupportLayoutOrientation supportsLayoutOrientation);
        
        virtual ~SurfaceMontageConfigurationAbstract();
        
        SurfaceMontageConfigurationTypeEnum::Enum getConfigurationType() const;
        
        virtual void initializeSelectedSurfaces() = 0;
        
        OverlaySet* getOverlaySet();
        
        const OverlaySet* getOverlaySet() const;
        
        bool hasLayoutOrientation() const;
        
        SurfaceMontageLayoutOrientationEnum::Enum getLayoutOrientation() const;
        
        void setLayoutOrientation(const SurfaceMontageLayoutOrientationEnum::Enum layoutOrientation);
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
    private:
        SurfaceMontageConfigurationAbstract(const SurfaceMontageConfigurationAbstract&);

        SurfaceMontageConfigurationAbstract& operator=(const SurfaceMontageConfigurationAbstract&);
        
    protected:
        virtual void saveMembersToScene(const SceneAttributes* sceneAttributes,
                                                         SceneClass* sceneClass) = 0;
        
        virtual void restoreMembersFromScene(const SceneAttributes* sceneAttributes,
                                                              const SceneClass* sceneClass) = 0;

    public:

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        SceneClassAssistant* m_sceneAssistant;

        const SurfaceMontageConfigurationTypeEnum::Enum m_configurationType;
        
        const SupportLayoutOrientation m_supportsLayoutOrientation;
        
        SurfaceMontageLayoutOrientationEnum::Enum m_layoutOrientation;
        
        OverlaySet* m_overlaySet;
        
        // ADD_NEW_MEMBERS_HERE
        
        friend class ModelSurfaceMontage;

    };
    
#ifdef __SURFACE_MONTAGE_CONFIGURATION_ABSTRACT_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SURFACE_MONTAGE_CONFIGURATION_ABSTRACT_DECLARE__

} // namespace
#endif  //__SURFACE_MONTAGE_CONFIGURATION_ABSTRACT_H__
