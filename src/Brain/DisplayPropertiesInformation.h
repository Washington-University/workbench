#ifndef __DISPLAY_PROPERTIES_INFORMATION__H_
#define __DISPLAY_PROPERTIES_INFORMATION__H_

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

#include "CaretColorEnum.h"
#include "DisplayProperties.h"
namespace caret {

    class DisplayPropertiesInformation : public DisplayProperties {
        
    public:
        DisplayPropertiesInformation(Brain* brain);
        
        virtual ~DisplayPropertiesInformation();
        
        void reset();
        
        void update();
        
        bool isContralateralIdentificationEnabled() const;
        
        void setContralateralIdentificationEnabled(const bool enabled);

        bool isVolumeIdentificationEnabled() const;
        
        void setVolumeIdentificationEnabled(const bool enabled);
        
        float getIdentificationSymbolSize() const;
        
        void setIdentificationSymbolSize(const float symbolSize);
        
        CaretColorEnum::Enum getIdentificationSymbolColor() const;
        
        void setIdentificationSymbolColor(const CaretColorEnum::Enum color);
        
        CaretColorEnum::Enum getIdentificationContralateralSymbolColor() const;
        
        void setIdentificationContralateralSymbolColor(const CaretColorEnum::Enum color);
        
        virtual SceneClass* saveToScene(const SceneAttributes& sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes& sceneAttributes,
                                      const SceneClass& sceneClass);
        
        virtual void copyDisplayProperties(const int32_t sourceTabIndex,
                                           const int32_t targetTabIndex);
        
    private:
        DisplayPropertiesInformation(const DisplayPropertiesInformation&);

        DisplayPropertiesInformation& operator=(const DisplayPropertiesInformation&);
        
        bool m_contralateralIdentificationEnabled;
        
        bool m_volumeIdentificationEnabled;
        
        float m_identifcationSymbolSize;
        
        CaretColorEnum::Enum m_identificationSymbolColor;
        
        CaretColorEnum::Enum m_identificationContralateralSymbolColor;
        
    };
    
#ifdef __DISPLAY_PROPERTIES_INFORMATION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DISPLAY_PROPERTIES_INFORMATION_DECLARE__

} // namespace
#endif  //__DISPLAY_PROPERTIES_INFORMATION__H_
