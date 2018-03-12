#ifndef __IDENTIFIED_ITEM_H__
#define __IDENTIFIED_ITEM_H__

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
    
    class IdentifiedItem : public CaretObject, public SceneableInterface {
        
    public:
        IdentifiedItem();
        
        IdentifiedItem(const AString& text);
        
        virtual ~IdentifiedItem();
        
        IdentifiedItem(const IdentifiedItem& obj);

        IdentifiedItem& operator=(const IdentifiedItem& obj);
        

        // ADD_NEW_METHODS_HERE
        
        virtual bool isValid() const;
        
        void appendText(const AString& text);
        
        void clearText();
        
        AString getText() const;

        virtual AString toString() const;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
    protected:
        virtual void restoreMembers(const SceneAttributes* sceneAttributes,
                         const SceneClass* sceneClass);
        
        virtual void saveMembers(const SceneAttributes* sceneAttributes,
                         SceneClass* sceneClass);

    private:
        void copyHelperIdentifiedItem(const IdentifiedItem& obj);

        void initializeMembers();
        
        // ADD_NEW_MEMBERS_HERE

        AString m_text;

        SceneClassAssistant* m_sceneAssistant;
        
        friend class IdentificationManager;
    };
    
#ifdef __IDENTIFIED_ITEM_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __IDENTIFIED_ITEM_DECLARE__

} // namespace
#endif  //__IDENTIFIED_ITEM_H__
