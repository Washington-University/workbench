#ifndef __DISPLAY_PROPERTIES__H_
#define __DISPLAY_PROPERTIES__H_

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
#include "SceneableInterface.h"

namespace caret {

    class Brain;
    
    class DisplayProperties : public CaretObject, public SceneableInterface {
        
    protected:
        DisplayProperties(Brain* brain);

    public:
        virtual ~DisplayProperties();
        
        Brain* getBrain();
        
        const Brain* getBrain() const;
        
        /**
         * Reset all settings to their defaults
         * and remove any data.
         */
        virtual void reset() = 0;
        
        /**
         * Update due to changes in data.
         */
        virtual void update() = 0;

        /**
         * Copy the display properties. 
         *
         * @param sourceTabIndex
         *    Index of tab from which properties are copied.
         * @param targetTabIndex
         *    Index of tab to which properties are copied.
         */
        virtual void copyDisplayProperties(const int32_t sourceTabIndex,
                                           const int32_t targetTabIndex) = 0;
        
        virtual SceneClass* saveToScene(const SceneAttributes& sceneAttributes,
                                        const AString& instanceName) = 0;
        
        virtual void restoreFromScene(const SceneAttributes& sceneAttributes,
                                      const SceneClass& sceneClass) = 0;
    private:
        DisplayProperties(const DisplayProperties&);

        DisplayProperties& operator=(const DisplayProperties&);
        
    public:
        virtual AString toString() const;
        
    protected:
        Brain* brain;
    };
    
#ifdef __DISPLAY_PROPERTIES_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DISPLAY_PROPERTIES_DECLARE__

} // namespace
#endif  //__DISPLAY_PROPERTIES__H_
