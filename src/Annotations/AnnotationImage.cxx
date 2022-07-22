
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
#include "GraphicsPrimitiveV3fT2f.h"
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
: AnnotationOneCoordinateShape(AnnotationTypeEnum::IMAGE,
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
: AnnotationOneCoordinateShape(obj)
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
        AnnotationOneCoordinateShape::operator=(obj);
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
GraphicsPrimitiveV3fT2f*
AnnotationImage::getGraphicsPrimitive() const
{
    if (m_graphicsPrimitive == NULL) {
        if ( ! m_imageBytesRGBA.empty()) {
            std::array<float, 4> textureBorderColorRGBA { 0.0, 0.0, 0.0, 0.0 };
            GraphicsTextureSettings textureSettings(&m_imageBytesRGBA[0],
                                                    m_imageWidth,
                                                    m_imageHeight,
                                                    1, /* number of slices */
                                                    GraphicsTextureSettings::DimensionType::FLOAT_STR_2D,
                                                    GraphicsTextureSettings::PixelFormatType::RGBA,
                                                    GraphicsTextureSettings::PixelOrigin::BOTTOM_LEFT,
                                                    GraphicsTextureSettings::WrappingType::CLAMP,
                                                    GraphicsTextureSettings::MipMappingType::ENABLED,
                                                    GraphicsTextureMagnificationFilterEnum::LINEAR,
                                                    GraphicsTextureMinificationFilterEnum::LINEAR_MIPMAP_LINEAR,
                                                    textureBorderColorRGBA);

            GraphicsPrimitiveV3fT2f* primitive = GraphicsPrimitive::newPrimitiveV3fT2f(GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLE_STRIP,
                                                                                       textureSettings);
            /*
             * A Triangle Strip (consisting of two triangles) is used
             * for drawing the image.  At this time, the XYZ coordinates
             * do not matter and they will be updated when the annotation
             * is drawn by a call to ::setVertexBounds().
             * The order of the vertices in the triangle strip is
             * Top Left, Bottom Left, Top Right, Bottom Right.  If this
             * order changes, ::setVertexBounds must be updated.
             *
             * Zeros are used for the X- and Y-coordinates.
             * The third and fourth parameters are the texture
             * S and T coordinates.
             */
            primitive->addVertex(0, 0, 0, 1);  /* Top Left */
            primitive->addVertex(0, 0, 0, 0);  /* Bottom Left */
            primitive->addVertex(0, 0, 1, 1);  /* Top Right */
            primitive->addVertex(0, 0, 1, 0);  /* Bottom Right */

            m_graphicsPrimitive.reset(primitive);
        }
    }
    
    return m_graphicsPrimitive.get();
}

void
AnnotationImage::setVertexBounds(const float bottomLeft[3],
                                 const float bottomRight[3],
                                 const float topRight[3],
                                 const float topLeft[3])
{
    GraphicsPrimitiveV3fT2f* primitive = getGraphicsPrimitive();
    if (primitive == NULL) {
        /*
         * Primitive will be invalid if when user is dragging mouse
         * to create the annotation.
         */
        return;
    }
    
    CaretAssert(primitive->getNumberOfVertices() == 4);

    primitive->replaceVertexFloatXYZ(0, topLeft);
    primitive->replaceVertexFloatXYZ(1, bottomLeft);
    primitive->replaceVertexFloatXYZ(2, topRight);
    primitive->replaceVertexFloatXYZ(3, bottomRight);
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
    AnnotationOneCoordinateShape::saveSubClassDataToScene(sceneAttributes,
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
    AnnotationOneCoordinateShape::restoreSubClassDataFromScene(sceneAttributes,
                                                                sceneClass);
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
}

