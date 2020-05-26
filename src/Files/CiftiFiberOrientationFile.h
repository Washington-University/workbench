#ifndef __CIFTI_FIBER_ORIENTATION_FILE_H__
#define __CIFTI_FIBER_ORIENTATION_FILE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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
        
        virtual void readFile(const AString& filename);
        
        virtual void writeFile(const AString& filename);
        
        bool supportsWriting() const;
        
        void addToDataFileContentInformation(DataFileContentInformation& dataFileInformation);
        
        float getMaximumVariance() const;
        
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
