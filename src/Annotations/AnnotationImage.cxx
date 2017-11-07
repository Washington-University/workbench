
/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#define __ANNOTATION_IMAGE_DECLARE__
#include "AnnotationImage.h"
#undef __ANNOTATION_IMAGE_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "GraphicsPrimitiveV3fT3f.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::AnnotationImage 
 * \brief An annotation image.
 * \ingroup Annotations
 */

/**
 * Constructor.
 *
 * @param attributeDefaultType
 *    Type for attribute defaults
 */
AnnotationImage::AnnotationImage(const AnnotationAttributesDefaultTypeEnum::Enum attributeDefaultType)
: AnnotationTwoDimensionalShape(AnnotationTypeEnum::IMAGE,
                                attributeDefaultType)
{
    initializeMembersAnnotationImage();
    
}

/**
 * Destructor.
 */
AnnotationImage::~AnnotationImage()
{
    delete m_sceneAssistant;
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
AnnotationImage::AnnotationImage(const AnnotationImage& obj)
: AnnotationTwoDimensionalShape(obj)
{
    initializeMembersAnnotationImage();
    this->copyHelperAnnotationImage(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return
 *    Reference to this object.
 */
AnnotationImage&
AnnotationImage::operator=(const AnnotationImage& obj)
{
    if (this != &obj) {
        AnnotationTwoDimensionalShape::operator=(obj);
        this->copyHelperAnnotationImage(obj);
    }
    return *this;
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void
AnnotationImage::copyHelperAnnotationImage(const AnnotationImage& obj)
{
    m_imageBytesRGBA         = obj.m_imageBytesRGBA;
    m_imageWidth             = obj.m_imageWidth;
    m_imageHeight            = obj.m_imageHeight;
}

/**
 * Set the image.
 *
 * @param imageBytesRGBA
 *     Bytes containing the image.  4 bytes per pixel with values
 *     ranging 0 to 255.  Number of elements MUST BE 
 *     (width * height * 4) 
 * @param imageWidth
 *     Width of the image.
 * @param imageHeight
 *     Height of the image.
 */
void
AnnotationImage::setImageBytesRGBA(const uint8_t* imageBytesRGBA,
                       const int32_t imageWidth,
                       const int32_t imageHeight)
{
    m_imageWidth  = imageWidth;
    m_imageHeight = imageHeight;
    
    const int32_t numBytes = (imageWidth
                              * imageHeight
                              * 4);
    
    m_imageBytesRGBA.resize(numBytes);
    
    for (int32_t i = 0; i < numBytes; i++) {
        m_imageBytesRGBA[i] = imageBytesRGBA[i];
    }
    
    setModified();
}

/**
 * @return Width of the image.
 */
int32_t
AnnotationImage::getImageWidth() const
{
    return m_imageWidth;
}

/**
 * @return Height of the image.
 */
int32_t
AnnotationImage::getImageHeight() const
{
    return m_imageHeight;
}

/**
 * @return The RGBA bytes of the image.
 * NULL if image is invalid.
 */
const uint8_t*
AnnotationImage::getImageBytesRGBA() const
{
    if (m_imageBytesRGBA.empty()) {
        return NULL;
    }
    
    return &m_imageBytesRGBA[0];
}

/**
 * Initialize members of a new instance.
 */
void
AnnotationImage::initializeMembersAnnotationImage()
{
    switch (m_attributeDefaultType) {
        case AnnotationAttributesDefaultTypeEnum::NORMAL:
            break;
        case AnnotationAttributesDefaultTypeEnum::USER:
            break;
    }
    
    m_imageBytesRGBA.clear();
    m_imageWidth  = 0;
    m_imageHeight = 0;
    
    m_sceneAssistant = new SceneClassAssistant();
    if (testProperty(Property::SCENE_CONTAINS_ATTRIBUTES)) {
    }
}

/**
 * @return Is this annotation requiring that it be kept in a fixed
 * aspect ratio?  By default, this is false.  This method may be
 * overridden by annotations that require a fixed aspect ratio
 * (such as an image annotaiton).
 */
bool
AnnotationImage::isFixedAspectRatio() const
{
    return true;
}

/**
 * @return The aspect ratio for annotations that have a fixed aspect ratio.
 * This method may be overridden by annotations that require a fixed aspect ratio
 * (such as an image annotaiton).
 *
 * If the aspect ratio is unknown return 1.  Never return zero.
 */
float
AnnotationImage::getFixedAspectRatio() const
{
    float aspectRatio = 1.0;
    
    if ((m_imageWidth > 0.0)
        && (m_imageHeight > 0.0)) {
        aspectRatio = static_cast<float>(m_imageHeight) / static_cast<float>(m_imageWidth);
        CaretAssert(aspectRatio != 0.0);
    }
    
    return aspectRatio;
}

/**
 * @return The graphics primitive for drawing the image as a texture.
 */
GraphicsPrimitiveV3fT3f*
AnnotationImage::getGraphicsPrimitive() const
{
    if (m_graphicsPrimitive == NULL) {
        if ( ! m_imageBytesRGBA.empty()) {
            GraphicsPrimitiveV3fT3f* primitive = GraphicsPrimitive::newPrimitiveV3fT3f(GraphicsPrimitive::PrimitiveType::OPENGL_QUADS,
                                                                                       &m_imageBytesRGBA[0],
                                                                                       m_imageWidth,
                                                                                       m_imageHeight);
            /*
             * Four coordinates are needed but XY values do not mattter.
             * These XY values are replaced when the image is drawn.
             * The texture (S, T) do matter.
             */
            primitive->addVertex(50, 50, 0.0, 0.0);
            primitive->addVertex(250, 50, 1.0, 0.0);
            primitive->addVertex(250, 250, 1.0, 1.0);
            primitive->addVertex(50, 250, 0.0, 1.0);
            m_graphicsPrimitive.reset(primitive);
        }
    }
    
    return m_graphicsPrimitive.get();
}

/**
 * Save subclass data to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass to which data members should be added.  Will always
 *     be valid (non-NULL).
 */
void
AnnotationImage::saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                            SceneClass* sceneClass)
{
    AnnotationTwoDimensionalShape::saveSubClassDataToScene(sceneAttributes,
                                                           sceneClass);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
}

/**
 * Restore file data from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass for the instance of a class that implements
 *     this interface.  Will NEVER be NULL.
 */
void
AnnotationImage::restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                 const SceneClass* sceneClass)
{
    AnnotationTwoDimensionalShape::restoreSubClassDataFromScene(sceneAttributes,
                                                                sceneClass);
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
}

