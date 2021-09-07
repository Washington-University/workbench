#ifndef __IDENTIFIED_ITEM_BASE_H__
#define __IDENTIFIED_ITEM_BASE_H__

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


#include "CaretObject.h"
#include "SceneableInterface.h"

namespace caret {

    class SceneClassAssistant;
    
    class IdentifiedItemBase : public CaretObject, public SceneableInterface {
        
    public:
        IdentifiedItemBase();
        
        IdentifiedItemBase(const AString& simpleText,
                           const AString& formattedText);
        
        virtual ~IdentifiedItemBase();
        
        IdentifiedItemBase(const IdentifiedItemBase& obj);

        IdentifiedItemBase& operator=(const IdentifiedItemBase& obj);
        

        // ADD_NEW_METHODS_HERE
        
        virtual bool isValid() const;
        
        void appendText(const AString& simpleText,
                        const AString& formattedText);
        
        void clearText();
        
        AString getSimpleText() const;

        AString getFormattedText() const;
        
        virtual AString toString() const;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
    protected:

    private:
        void copyHelperIdentifiedItemBase(const IdentifiedItemBase& obj);

        void initializeMembers();
        
        // ADD_NEW_MEMBERS_HERE

        AString m_text;

        AString m_formattedText;
        
        SceneClassAssistant* m_sceneAssistant;
        
        friend class IdentificationManager;
    };
    
#ifdef __IDENTIFIED_ITEM_BASE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __IDENTIFIED_ITEM_BASE_DECLARE__

} // namespace
#endif  //__IDENTIFIED_ITEM_BASE_H__
