#ifndef __SCENEABLE_INTERFACE__H_
#define __SCENEABLE_INTERFACE__H_

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
