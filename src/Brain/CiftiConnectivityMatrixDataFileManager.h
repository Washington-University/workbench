#ifndef __CIFTI_CONNECTIVITY_MATRIX_DATA_FILE_MANAGER_H__
#define __CIFTI_CONNECTIVITY_MATRIX_DATA_FILE_MANAGER_H__

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


#include "CaretObject.h"
#include "DataFileException.h"
#include "EventListenerInterface.h"
#include "SceneableInterface.h"

namespace caret {

    class Brain;
    class SurfaceFile;
    
    class CiftiConnectivityMatrixDataFileManager
    : public CaretObject,
    public EventListenerInterface,
    public SceneableInterface
    {
        
    public:
        CiftiConnectivityMatrixDataFileManager(Brain* brain);
        
        virtual ~CiftiConnectivityMatrixDataFileManager();
        
        bool loadDataForSurfaceNode(const SurfaceFile* surfaceFile,
                                    const int32_t nodeIndex,
                                    std::vector<AString>& rowColumnInformationOut) throw (DataFileException);
        
        bool loadAverageDataForSurfaceNodes(const SurfaceFile* surfaceFile,
                                            const std::vector<int32_t>& nodeIndices) throw (DataFileException);
        
        bool loadDataForVoxelAtCoordinate(const float xyz[3],
                                          std::vector<AString>& rowColumnInformationOut) throw (DataFileException);
        
        void reset();
        
        void receiveEvent(Event* event);
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
    private:
        CiftiConnectivityMatrixDataFileManager(const CiftiConnectivityMatrixDataFileManager&);

        CiftiConnectivityMatrixDataFileManager& operator=(const CiftiConnectivityMatrixDataFileManager&);
        
    public:

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:

        // ADD_NEW_MEMBERS_HERE
        
        Brain* m_brain;

    };
    
#ifdef __CIFTI_CONNECTIVITY_MATRIX_DATA_FILE_MANAGER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CIFTI_CONNECTIVITY_MATRIX_DATA_FILE_MANAGER_DECLARE__

} // namespace
#endif  //__CIFTI_CONNECTIVITY_MATRIX_DATA_FILE_MANAGER_H__
