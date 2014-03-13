#ifndef __SCENEABLE_INTERFACE__H_
#define __SCENEABLE_INTERFACE__H_

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


/**
 * \class caret::SceneableInterface 
 * \brief Interface that must be implemented by classes saved to scenes.
 * \ingroup Scene
 *
 * See the documentation in the class Scene for how to use the Scene system.
 */

#include "AString.h"

namespace caret {

    class SceneAttributes;
    class SceneClass;
    
    class SceneableInterface {
        
    protected:
        SceneableInterface() { }
        
    public:
        virtual ~SceneableInterface() { }

        /**
         * Create a scene for an instance of a class.
         *
         * @param sceneAttributes
         *    Attributes for the scene.  Scenes may be of different types
         *    (full, generic, etc) and the attributes should be checked when
         *    saving the scene.
         *
         * @param instanceName
         *    Name of the class' instance.
         *
         * @return Pointer to SceneClass object representing the state of 
         *    this object.  Under some circumstances a NULL pointer may be
         *    returned.  Caller will take ownership of returned object.
         */
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName) = 0;
        
        /**
         * Restore the state of an instance of a class.
         * 
         * @param sceneAttributes
         *    Attributes for the scene.  Scenes may be of different types
         *    (full, generic, etc) and the attributes should be checked when
         *    restoring the scene.
         *
         * @param sceneClass
         *     sceneClass for the instance of a class that implements
         *     this interface.  May be NULL for some types of scenes.
         */
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass) = 0;
        
    protected:
        /**
         * Copy constructor.  Implemented only to prevent compilation
         * warnings for classes that implement this interface and have
         * a copy constructor.
         */
        SceneableInterface(const SceneableInterface&) { }

    private:
        SceneableInterface& operator=(const SceneableInterface&);
        
    };
    
#ifdef __SCENEABLE_INTERFACE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SCENEABLE_INTERFACE_DECLARE__

} // namespace
#endif  //__SCENEABLE_INTERFACE__H_
