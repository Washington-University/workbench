#ifndef __CIFTI_FIBER_ORIENTATION_FILE_H__
#define __CIFTI_FIBER_ORIENTATION_FILE_H__

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

#include "BrainConstants.h"
#include "CaretDataFile.h"
#include "DisplayGroupEnum.h"

namespace caret {

    class CiftiXML;
    class FiberOrientation;
    
    class CiftiFiberOrientationFile : public CaretDataFile {
        
    public:
        CiftiFiberOrientationFile();
        
        virtual ~CiftiFiberOrientationFile();
        
        void initializeWithTestData();
        
        int64_t getNumberOfFiberOrientations() const;
        
        FiberOrientation* getFiberOrientations(const int64_t indx);
        
        const FiberOrientation* getFiberOrientations(const int64_t indx) const;
        
        FiberOrientation* getFiberOrientationNearestCoordinate(const float xyz[3],
                                                               const float maximumDistance) const;
        
        void getVolumeSpacing(float volumeSpacingOut[3]) const;
        
        void setDisplayed(const DisplayGroupEnum::Enum displayGroup,
                          const int32_t tabIndex,
                          const bool displayed);
        
        bool isDisplayed(const DisplayGroupEnum::Enum displayGroup,
                         const int32_t tabIndex) const;
        
        virtual void clear();
        
        bool isEmpty() const;
        
        const CiftiXML* getCiftiXML() const;
        
        virtual StructureEnum::Enum getStructure() const;
        
        virtual void setStructure(const StructureEnum::Enum structure);
        
        virtual GiftiMetaData* getFileMetaData();
        
        virtual const GiftiMetaData* getFileMetaData() const;
        
        virtual void readFile(const AString& filename) throw (DataFileException);
        
        virtual void writeFile(const AString& filename) throw (DataFileException);
        
        bool supportsWriting() const;
        
        // ADD_NEW_METHODS_HERE
        
    private:
        CiftiFiberOrientationFile(const CiftiFiberOrientationFile&);

        CiftiFiberOrientationFile& operator=(const CiftiFiberOrientationFile&);
        
    private:

        void clearPrivate();
        
        CiftiXML* m_ciftiXML;
        
        GiftiMetaData* m_metadata;

        std::vector<FiberOrientation*> m_fiberOrientations;
        
        /** Display status in display group */
        bool m_displayStatusInDisplayGroup[DisplayGroupEnum::NUMBER_OF_GROUPS];
        
        /** Display status in tab */
        bool m_displayStatusInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        float m_volumeSpacing[3];
        
        // ADD_NEW_MEMBERS_HERE
    };
    
#ifdef __CIFTI_FIBER_ORIENTATION_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CIFTI_FIBER_ORIENTATION_FILE_DECLARE__

} // namespace
#endif  //__CIFTI_FIBER_ORIENTATION_FILE_H__
