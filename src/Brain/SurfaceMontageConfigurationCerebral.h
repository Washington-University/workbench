#ifndef __SURFACE_MONTAGE_CONFIGURATION_CEREBRAL_H__
#define __SURFACE_MONTAGE_CONFIGURATION_CEREBRAL_H__

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


#include "SurfaceMontageConfigurationAbstract.h"

namespace caret {
    class SceneClassAssistant;
    class SurfaceSelectionModel;
    
    class SurfaceMontageConfigurationCerebral : public SurfaceMontageConfigurationAbstract {
        
    public:
        SurfaceMontageConfigurationCerebral();
        
        virtual ~SurfaceMontageConfigurationCerebral();
        
        SurfaceSelectionModel* getLeftFirstSurfaceSelectionModel();
        
        const SurfaceSelectionModel* getLeftFirstSurfaceSelectionModel() const;
        
        SurfaceSelectionModel* getLeftSecondSurfaceSelectionModel();
        
        const SurfaceSelectionModel* getLeftSecondSurfaceSelectionModel() const;
        
        SurfaceSelectionModel* getRightFirstSurfaceSelectionModel();
        
        const SurfaceSelectionModel* getRightFirstSurfaceSelectionModel() const;
        
        SurfaceSelectionModel* getRightSecondSurfaceSelectionModel();

        const SurfaceSelectionModel* getRightSecondSurfaceSelectionModel() const;
        
        bool isLeftEnabled() const;
        
        void setLeftEnabled(const bool enabled);
        
        bool isRightEnabled() const;
        
        void setRightEnabled(const bool enabled);
        
        bool isLateralEnabled() const;
        
        void setLateralEnabled(const bool enabled);
        
        bool isMedialEnabled() const;
        
        void setMedialEnabled(const bool enabled);
        
        bool isFirstSurfaceEnabled() const;
        
        void setFirstSurfaceEnabled(const bool enabled);
        
        bool isSecondSurfaceEnabled() const;
        
        void setSecondSurfaceEnabled(const bool enabled);
        
        virtual void initializeSelectedSurfaces();
        
        virtual bool isValid();
        
        virtual void updateSurfaceMontageViewports(std::vector<SurfaceMontageViewport>& surfaceMontageViewports);
        
    private:
        SurfaceMontageConfigurationCerebral(const SurfaceMontageConfigurationCerebral&);

        SurfaceMontageConfigurationCerebral& operator=(const SurfaceMontageConfigurationCerebral&);
        
    public:
        virtual AString toString() const;
        

        // ADD_NEW_METHODS_HERE

    protected:
        virtual void saveMembersToScene(const SceneAttributes* sceneAttributes,
                                        SceneClass* sceneClass);
        
        virtual void restoreMembersFromScene(const SceneAttributes* sceneAttributes,
                                             const SceneClass* sceneClass);
        
    private:
        SceneClassAssistant* m_sceneAssistant;

        SurfaceSelectionModel* m_leftFirstSurfaceSelectionModel;
        
        SurfaceSelectionModel* m_leftSecondSurfaceSelectionModel;
        
        SurfaceSelectionModel* m_rightFirstSurfaceSelectionModel;
        
        SurfaceSelectionModel* m_rightSecondSurfaceSelectionModel;
        
        bool m_leftEnabled;
        
        bool m_rightEnabled;
        
        bool m_firstSurfaceEnabled;
        
        bool m_secondSurfaceEnabled;
        
        bool m_lateralEnabled;
        
        bool m_medialEnabled;
        
        // ADD_NEW_MEMBERS_HERE

        friend class ModelSurfaceMontage;
        
    };
    
#ifdef __SURFACE_MONTAGE_CONFIGURATION_CEREBRAL_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SURFACE_MONTAGE_CONFIGURATION_CEREBRAL_DECLARE__

} // namespace
#endif  //__SURFACE_MONTAGE_CONFIGURATION_CEREBRAL_H__
