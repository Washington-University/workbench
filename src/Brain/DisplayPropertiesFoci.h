#ifndef __DISPLAY_PROPERTIES_FOCI__H_
#define __DISPLAY_PROPERTIES_FOCI__H_

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
#include "DisplayGroupEnum.h"
#include "DisplayProperties.h"
#include "FociColoringTypeEnum.h"
#include "FociDrawingTypeEnum.h"

namespace caret {

    class Brain;
    
    class DisplayPropertiesFoci : public DisplayProperties {
        
    public:
        DisplayPropertiesFoci(Brain* brain);
        
        virtual ~DisplayPropertiesFoci();

        virtual void reset();
        
        virtual void update();
        
        bool isDisplayed(const int32_t browserTabIndex) const;
        
        void setDisplayed(const int32_t browserTabIndex,
                          const bool displayStatus);
        
        bool isContralateralDisplayed(const int32_t browserTabIndex) const;
        
        void setContralateralDisplayed(const int32_t browserTabIndex,
                                       const bool contralateralDisplayStatus);
        
        DisplayGroupEnum::Enum getDisplayGroup(const int32_t browserTabIndex) const;
        
        void setDisplayGroup(const int32_t browserTabIndex,
                             const DisplayGroupEnum::Enum  displayGroup);
        
        float getFociSize() const;
        
        void setFociSize(const float pointSize);
        
        FociColoringTypeEnum::Enum getColoringType() const;
        
        void setColoringType(const FociColoringTypeEnum::Enum coloringType);
        
        FociDrawingTypeEnum::Enum getDrawingType() const;
        
        void setDrawingType(const FociDrawingTypeEnum::Enum drawingType);
        
        void setPasteOntoSurface(const int32_t browserTabIndex,
                                 const bool enabled);
        
        bool isPasteOntoSurface(const int32_t browserTabIndex) const;
        
    private:
        DisplayPropertiesFoci(const DisplayPropertiesFoci&);

        DisplayPropertiesFoci& operator=(const DisplayPropertiesFoci&);
        
        bool m_displayStatus[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        bool m_contralateralDisplayStatus[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        bool m_pasteOntoSurface[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        DisplayGroupEnum::Enum m_displayGroup[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        float m_fociSize;
        
        FociColoringTypeEnum::Enum m_coloringType;
        
        FociDrawingTypeEnum::Enum m_drawingType;
        
    };
    
#ifdef __DISPLAY_PROPERTIES_FOCI_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DISPLAY_PROPERTIES_FOCI_DECLARE__

} // namespace
#endif  //__DISPLAY_PROPERTIES_FOCI__H_
