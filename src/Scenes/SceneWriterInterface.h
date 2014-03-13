#ifndef __SCENE_WRITER_INTERFACE__H_
#define __SCENE_WRITER_INTERFACE__H_

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

#include <stdint.h>
/**
 * \class caret::SceneWriterInterface 
 * \brief Interface for classes that write scenes.
 * \ingroup Scene
 *
 * See the documentation in the class Scene for how to use the Scene system.
 */

namespace caret {

    class Scene;
    
    class SceneWriterInterface {

    protected:
        SceneWriterInterface() { }

    public:
        virtual ~SceneWriterInterface() { }
        
        /**
         * Write the given scene.
         * @param scene
         *    Scene that is written.
         * @throws ???
         *    Implementing classes may throw exceptions.
         */
        virtual void writeScene(const Scene& scene,
                                const int32_t sceneIndex) = 0;
        
    private:
        SceneWriterInterface(const SceneWriterInterface&);

        SceneWriterInterface& operator=(const SceneWriterInterface&);
        
    public:

        // ADD_NEW_METHODS_HERE

    private:

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __SCENE_WRITER_INTERFACE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SCENE_WRITER_INTERFACE_DECLARE__

} // namespace
#endif  //__SCENE_WRITER_INTERFACE__H_
