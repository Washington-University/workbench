#ifndef __EXAMPLE_SCENE_INFO_H__
#define __EXAMPLE_SCENE_INFO_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2025 Washington University School of Medicine
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



#include <memory>

#include "CaretObject.h"



namespace caret {

    class ExampleSceneInfo : public CaretObject {
        
    public:
        ExampleSceneInfo(const AString& filename,
                         const AString& name,
                         const AString& description);
        
        virtual ~ExampleSceneInfo();
        
        ExampleSceneInfo(const ExampleSceneInfo& obj);

        ExampleSceneInfo& operator=(const ExampleSceneInfo& obj);
        
        AString getFilename() const;
        
        AString getName() const;
        
        AString getDescription() const;

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void copyHelperExampleSceneInfo(const ExampleSceneInfo& obj);

        AString m_filename;
        
        AString m_name;
        
        AString m_description;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EXAMPLE_SCENE_INFO_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EXAMPLE_SCENE_INFO_DECLARE__

} // namespace
#endif  //__EXAMPLE_SCENE_INFO_H__
