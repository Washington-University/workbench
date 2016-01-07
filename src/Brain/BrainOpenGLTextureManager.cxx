
/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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

#define __BRAIN_OPEN_G_L_TEXTURE_MANAGER_DECLARE__
#include "BrainOpenGLTextureManager.h"
#undef __BRAIN_OPEN_G_L_TEXTURE_MANAGER_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"

using namespace caret;


    
/**
 * \class caret::BrainOpenGLTextureManager 
 * \brief Manages allocated OpenGL Textures.
 * \ingroup Brain
 *
 * This texture manager helps out with tracking OpenGL
 * textures.  There are instance in which textures need
 * to be deleted such as when QGLWidget::renderPixmap()
 * is used to capture an image of the graphics region.
 *
 * Note that the FTGL text rendering code also uses
 * texture and this texture manager does not interact
 * in any way with FTGL.
 */

/**
 * Constructor.
 */
BrainOpenGLTextureManager::BrainOpenGLTextureManager()
: CaretObject()
{
}

/**
 * Destructor.
 */
BrainOpenGLTextureManager::~BrainOpenGLTextureManager()
{
    deleteAllTextureNames();
}

/**
 * Create a texture name for use by the given data pointer.
 * If the data pointer already has an associated texture name,
 * the texture name is deleted and a new texture name is returned.
 *
 * @param dataPointer
 *     Pointer to the data.
 * @return 
 *     New texture name for the data.
 */
GLuint
BrainOpenGLTextureManager::createTextureNameForData(const void* dataPointer)
{
    GLuint textureName = getTextureNameForData(dataPointer);
    if (textureName > 0) {
        CaretAssertMessage(0, "Texture name already exists for data, will be replaced.");
        glDeleteTextures(1, &textureName);
        textureName = 0;
    }
    
    glGenTextures(1, &textureName);
    std::cout << "Created texture name: " << textureName << std::endl;
    
    m_dataPointerToTextureNameMap.insert(std::make_pair(dataPointer,
                                                        textureName));
    
    return textureName;
}

/**
 * Get the texture name associated with the given data pointer.
 *
 * @param dataPointer
 *    Pointer to the data.
 * @return
 *    New name for texture associated with the pointer.
 */
GLuint
BrainOpenGLTextureManager::getTextureNameForData(const void* dataPointer)
{
    GLuint textureName = 0;
    
    TextureNameContainer::iterator iter = m_dataPointerToTextureNameMap.find(dataPointer);
    if (iter != m_dataPointerToTextureNameMap.end()) {
        textureName = iter->second;
        
        /*
         * It is possible that OpenGL may delete a texture
         * so ensure the name is still valid.
         */
        if ( ! glIsTexture(textureName)) {
            textureName = 0;
            m_dataPointerToTextureNameMap.erase(dataPointer);
        }
    }
    
    return textureName;
}

/**
 * Delete all texture names.
 */
void
BrainOpenGLTextureManager::deleteAllTextureNames()
{
    for (TextureNameContainer::iterator iter = m_dataPointerToTextureNameMap.begin();
         iter != m_dataPointerToTextureNameMap.end();
         iter++) {
        GLuint textureID = iter->second;
        if (glIsTexture(textureID) == GL_TRUE) {
            glDeleteTextures(1, &textureID);
        }
        else {
            
//            CaretLogSevere("Attempting to delete an invalid texture name: "
//                           + QString::number(textureID));
        }
    }
    
    m_dataPointerToTextureNameMap.clear();
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
BrainOpenGLTextureManager::toString() const
{
    return "BrainOpenGLTextureManager";
}

