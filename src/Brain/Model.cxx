/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
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


#include "Model.h"

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventManager.h"
#include "Matrix4x4.h"
#include "ModelSurface.h"
#include "OverlaySet.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "Surface.h"
#include "UserView.h"

using namespace caret;

/**
 * Constructor.
 * @param m_modelType Type of this model.
 * @param allowsYokingStatus  This model can be yoked.
 * @param allowsRotationStatus This model can be rotated.
 *
 */
Model::Model(const ModelTypeEnum::Enum modelType,
             const YokingAllowedType allowsYokingStatus,
             const RotationAllowedType allowsRotationStatus,
             Brain* brain)
    : CaretObject()
{
    m_brain = brain;
    initializeMembersModel();
    m_modelType = modelType;
    m_allowsYokingStatus = allowsYokingStatus;
    m_allowsRotationStatus   = allowsRotationStatus;
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        resetViewPrivate(i);
    }
    
    /*
     * Set this last in constructor or else resetViewPrivate() may
     * not function correctly.
     */
    m_isYokingController = (m_modelType ==
                                ModelTypeEnum::MODEL_TYPE_YOKING);
}

/**
 * Destructor
 */
Model::~Model()
{
}

void
Model::initializeMembersModel()
{
    m_isYokingController = false;
    m_defaultModelScaling = 1.0f;
}

/**
 * @return The type of model.
 */
ModelTypeEnum::Enum 
Model::getControllerType() const
{
    return m_modelType; 
}

/**
 * @return Is this a yoking model?
 */
bool 
Model::isYokingModel() const
{
    return m_isYokingController;
}

/**
 * See if this controller allows rotation.
 * 
 * @return true if this controller allows rotation, else false.
 *
 */
bool
Model::isRotationAllowed() const
{
    return (m_allowsRotationStatus == ROTATION_ALLOWED_YES);
}

/**
 * See if this controller allows yoking.
 * When Yoked, this controller will maintain the same viewpoint
 * as the user moves the mouse in any other yoked controller.
 * @return  true if this controller supports yoking, else false.
 *
 */
bool
Model::isYokeable() const
{
    return (m_allowsYokingStatus == YOKING_ALLOWED_YES);
}

/**
 * For a structure model, copy the transformations from one window of
 * the structure model to another window.
 *
 * @param controllerSource        Source structure model
 * @param windowTabNumberSource   windowTabNumber of source transformation.
 * @param windowTabNumberTarget   windowTabNumber of target transformation.
 *
 */
void
Model::copyTransformationsAndViews(
                   const Model& controllerSource,
                   const int32_t windowTabNumberSource,
                   const int32_t windowTabNumberTarget)
{
    if (this == &controllerSource) {
        if (windowTabNumberSource == windowTabNumberTarget) {
            return;
        }
    }
    
    CaretAssertArrayIndex(m_translation,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          windowTabNumberTarget);
    CaretAssertArrayIndex(controllerSource->translation,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          windowTabNumberSource);
    
    m_scaling[windowTabNumberTarget] = controllerSource.m_scaling[windowTabNumberSource];
    
    for (int i = 0; i < VIEWING_TRANSFORM_COUNT; i++) {
        const ViewingTransformIndex rmi = static_cast<ViewingTransformIndex>(i);
        m_viewingRotationMatrix[windowTabNumberTarget][rmi].setMatrix(*controllerSource.getViewingRotationMatrix(windowTabNumberSource,
                                                                                                                     rmi));
        m_translation[windowTabNumberTarget][i][0] = controllerSource.m_translation[windowTabNumberSource][i][0];
        m_translation[windowTabNumberTarget][i][1] = controllerSource.m_translation[windowTabNumberSource][i][1];
        m_translation[windowTabNumberTarget][i][2] = controllerSource.m_translation[windowTabNumberSource][i][2];
    }
}

/**
 * Get the viewing rotation matrix.
 *
 * @param  windowTabNumber  
 *    Window for which rotation is requested
 * @param  viewingTransformIndex
 *    Index of the rotation matrix.  There are several rotation matrices
 *    that are used for special viewing modes.  
 * @return Pointer to the viewing rotation matrix.
 */
const Matrix4x4*
Model::getViewingRotationMatrix(const int32_t windowTabNumberIn,
                                const ViewingTransformIndex viewingTransformIndex) const
{
    /*
     * Yoking ALWAYS uses first window index.
     */
    int32_t windowTabNumber = windowTabNumberIn;
    if (m_isYokingController) {
        windowTabNumber = 0;
    }
    
    CaretAssertArrayIndex(m_viewingRotationMatrix,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          windowTabNumber);
    CaretAssertArrayIndex(m_viewingRotationMatrix, 
                          VIEWING_TRANSFORM_COUNT, 
                          viewingTransformIndex);
    return &m_viewingRotationMatrix[windowTabNumber][viewingTransformIndex];
}

/**
 * @param  windowTabNumber  
 *    Window for which rotation is requested
 * @param  viewingTransformIndex
 *    Index of the rotation matrix.  There are several rotation matrices
 *    that are used for special viewing modes.  
 * @return Pointer to the viewing rotation matrix.
 */
Matrix4x4*
Model::getViewingRotationMatrix(const int32_t windowTabNumberIn,
                                const ViewingTransformIndex viewingTransformIndex)
{
    /*
     * Yoking ALWAYS uses first window index.
     */
    int32_t windowTabNumber = windowTabNumberIn;
    if (m_isYokingController) {
        windowTabNumber = 0;
    }
    
    CaretAssertArrayIndex(m_viewingRotationMatrix,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          windowTabNumber);
    CaretAssertArrayIndex(m_viewingRotationMatrix, 
                          VIEWING_TRANSFORM_COUNT, 
                          viewingTransformIndex);
    return &m_viewingRotationMatrix[windowTabNumber][viewingTransformIndex];
}

/**
 * get the translation.
 *
 * @param  windowTabNumber  Window for which translation is requested
 * @return  The translation, an array of three floats.
 *
 */
const float*
Model::getTranslation(const int32_t windowTabNumberIn,
                      const ViewingTransformIndex viewingTransformIndex) const
{
    /*
     * Yoking ALWAYS uses first window index.
     */
    int32_t windowTabNumber = windowTabNumberIn;
    if (m_isYokingController) {
        windowTabNumber = 0;
    }
    
    CaretAssertArrayIndex(m_translation,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          windowTabNumber);
    CaretAssertArrayIndex(m_translation, 
                          VIEWING_TRANSFORM_COUNT, 
                          viewingTransformIndex);
    return &m_translation[windowTabNumber][viewingTransformIndex][0];
}

/**
 * Set the translation for ALL viewing transforms (normal, surface
 * montage, etc).  Translation passed to this method should be for
 * the normal surface viewing.  The other translations (montage left
 * opposite, etc.) will be updated appropriately.
 *
 * @param  windowTabNumber  Window for which translation is requested
 * @param  t  The translation, an array of three floats.
 *
 */
void
Model::setTranslation(const int32_t windowTabNumber,
                          const float t[3])
{
    setTranslation(windowTabNumber,
                         t[0],
                         t[1],
                         t[2]);
}

/**
 * Set the translation for ALL viewing transforms (normal, surface
 * montage, etc).  Translation passed to this method should be for
 * the normal surface viewing.  The other translations (montage left
 * opposite, etc.) will be updated appropriately.
 *
 * @param  windowTabNumber  Window for which translation is requested
 * @param  tx - The x-coordinate of the translation.
 * @param  ty - The y-coordinate of the translation.
 * @param  tz - The z-coordinate of the translation.
 *
 */
void
Model::setTranslation(const int32_t windowTabNumberIn,
                          const float tx,
                          const float ty,
                          const float tz)
{
    /*
     * Yoking ALWAYS uses first window index.
     */
    int32_t windowTabNumber = windowTabNumberIn;
    if (m_isYokingController) {
        windowTabNumber = 0;
    }
    
    CaretAssertArrayIndex(m_translation,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          windowTabNumber);
    setTranslation(windowTabNumber,
                         VIEWING_TRANSFORM_NORMAL,
                         tx,
                         ty,
                         tz);

    setTranslation(windowTabNumber,
                         VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED,
                         tx,
                         ty,
                         tz);
    
    setTranslation(windowTabNumber,
                         VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE,
                         tx,
                         ty,
                         tz);
    
    setTranslation(windowTabNumber,
                         VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT,
                         tx,
                         ty,
                         tz);
    
    setTranslation(windowTabNumber,
                         VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE,
                         tx,
                         ty,
                         tz);
}

/**
 * Set the translation for the given viewing transform.
 *
 * @param  windowTabNumber  Window for which translation is requested
 * @param  viewingTransformIndex
 *    Index of the viewing transform.  There are several transform matrices
 *    that are used for special viewing modes.  
 * @param  tx - The x-coordinate of the translation.
 * @param  ty - The y-coordinate of the translation.
 * @param  tz - The z-coordinate of the translation.
 *
 */
void
Model::setTranslation(const int32_t windowTabNumberIn,
                      const ViewingTransformIndex viewingTransformIndex,
                      const float tx,
                      const float ty,
                      const float tz)
{
    /*
     * Yoking ALWAYS uses first window index.
     */
    int32_t windowTabNumber = windowTabNumberIn;
    if (m_isYokingController) {
        windowTabNumber = 0;
    }
    
    CaretAssertArrayIndex(m_translation,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          windowTabNumber);
    CaretAssertArrayIndex(m_viewingRotationMatrix, 
                          VIEWING_TRANSFORM_COUNT, 
                          viewingTransformIndex);
    m_translation[windowTabNumber][viewingTransformIndex][0] = tx;
    m_translation[windowTabNumber][viewingTransformIndex][1] = ty;
    m_translation[windowTabNumber][viewingTransformIndex][2] = tz;
}

/**
 * get the scaling.
 *
 * @param  windowTabNumber  Window for which scaling is requested
 * @return  Scaling value.
 *
 */
float
Model::getScaling(const int32_t windowTabNumberIn) const
{
    /*
     * Yoking ALWAYS uses first window index.
     */
    int32_t windowTabNumber = windowTabNumberIn;
    if (m_isYokingController) {
        windowTabNumber = 0;
    }
    
    CaretAssertArrayIndex(m_scaling,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          windowTabNumber);
    return m_scaling[windowTabNumber];
}

/**
 * set the scaling.
 *
 * @param  windowTabNumber  Window for which scaling is requested
 * @param  s  The scaling value.
 *
 */
void
Model::setScaling(
                   const int32_t windowTabNumberIn,
                   const float s)
{
    /*
     * Yoking ALWAYS uses first window index.
     */
    int32_t windowTabNumber = windowTabNumberIn;
    if (m_isYokingController) {
        windowTabNumber = 0;
    }
    
    CaretAssertArrayIndex(m_scaling,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          windowTabNumber);
    m_scaling[windowTabNumber] = s;
}

/**
 * Resets the view to the default view.
 * 
 * Since is resetView() is virtual and overridden by subclasses,
 * resetView() cannot be called by the constructor of this class.
 * So this methed, resetViewPrivate() is used to reset the view and
 * to initialize the views inside of the constructor.
 *
 * @param  windowTabNumber  Window for which view is requested
 * reset the view.
 */
void 
Model::resetViewPrivate(const int windowTabNumberIn)
{
    /*
     * Yoking ALWAYS uses first window index.
     */
    int32_t windowTabNumber = windowTabNumberIn;
    if (m_isYokingController) {
        windowTabNumber = 0;
    }
    
    CaretAssertArrayIndex(m_viewingRotationMatrix,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          windowTabNumber);
    setTranslation(windowTabNumber, 0.0f, 0.0f, 0.0f);
    for (int32_t i = 0; i < VIEWING_TRANSFORM_COUNT; i++) {
        m_viewingRotationMatrix[windowTabNumber][i].identity();
    }
    setScaling(windowTabNumber, m_defaultModelScaling);    
}

/**
 * Reset the view to the default view.
 *
 * @param  windowTabNumber  Window for which view is requested
 * reset the view.
 */
void
Model::resetView(const int32_t windowTabNumberIn)
{
    /*
     * Yoking ALWAYS uses first window index.
     */
    int32_t windowTabNumber = windowTabNumberIn;
    if (m_isYokingController) {
        windowTabNumber = 0;
    }
    
    resetViewPrivate(windowTabNumber);
}

/**
 * @param  windowTabNumber  Window for which view is requested
 * set to a right side view.
 *
 */
void
Model::rightView(const int32_t windowTabNumberIn)
{
    /*
     * Yoking ALWAYS uses first window index.
     */
    int32_t windowTabNumber = windowTabNumberIn;
    if (m_isYokingController) {
        windowTabNumber = 0;
    }
    
    CaretAssertArrayIndex(m_viewingRotationMatrix,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          windowTabNumber);
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_NORMAL].identity();
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_NORMAL].rotateY(-90.0);
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_NORMAL].rotateZ(-90.0);
    
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED].identity();
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED].rotateY(-90.0);
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED].rotateZ(-90.0);
    
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE].identity();
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE].rotateY(-90.0);
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE].rotateZ(-90.0);

    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT].identity();
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT].rotateY(-90.0);
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT].rotateZ(-90.0);
    
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE].identity();
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE].rotateY(-90.0);
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE].rotateZ(-90.0);
}

/**
 * set to a left side view.
 * @param  windowTabNumber  Window for which view is requested
 *
 */
void
Model::leftView(const int32_t windowTabNumberIn)
{
    /*
     * Yoking ALWAYS uses first window index.
     */
    int32_t windowTabNumber = windowTabNumberIn;
    if (m_isYokingController) {
        windowTabNumber = 0;
    }
    
    CaretAssertArrayIndex(m_viewingRotationMatrix,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          windowTabNumber);
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_NORMAL].identity();
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_NORMAL].rotateY(90.0);
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_NORMAL].rotateZ(90.0);
    
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED].identity();
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED].rotateY(90.0);
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED].rotateZ(90.0);
    
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE].identity();
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE].rotateY(90.0);
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE].rotateZ(90.0);
    
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT].identity();
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT].rotateY(90.0);
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT].rotateZ(90.0);
    
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE].identity();
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE].rotateY(90.0);
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE].rotateZ(90.0);
}

/**
 * set to a anterior view.
 * @param  windowTabNumber  Window for which view is requested
 *
 */
void
Model::anteriorView(const int32_t windowTabNumberIn)
{
    /*
     * Yoking ALWAYS uses first window index.
     */
    int32_t windowTabNumber = windowTabNumberIn;
    if (m_isYokingController) {
        windowTabNumber = 0;
    }
    
    CaretAssertArrayIndex(m_viewingRotationMatrix,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          windowTabNumber);
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_NORMAL].identity();
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_NORMAL].rotateX(-90.0);
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_NORMAL].rotateY(180.0);
    
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED].identity();
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED].rotateX(-90.0);
    
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE].identity();
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE].rotateX(-90.0);
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE].rotateY(180.0);
    
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT].identity();
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT].rotateX(-90.0);
    
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE].identity();
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE].rotateX(-90.0);
}

/**
 * set to a posterior view.
 * @param  windowTabNumber  Window for which view is requested
 *
 */
void
Model::posteriorView(const int32_t windowTabNumberIn)
{
    /*
     * Yoking ALWAYS uses first window index.
     */
    int32_t windowTabNumber = windowTabNumberIn;
    if (m_isYokingController) {
        windowTabNumber = 0;
    }
    
    CaretAssertArrayIndex(m_viewingRotationMatrix,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          windowTabNumber);
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_NORMAL].identity();
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_NORMAL].rotateX(-90.0);
    
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED].identity();
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED].rotateX(-90.0);
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED].rotateY(180.0);
    
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE].identity();
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE].rotateX(-90.0);
    
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT].identity();
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT].rotateX(-90.0);
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT].rotateY(-180.0);
    
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE].identity();
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE].rotateX(-90.0);
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE].rotateY(-180.0);
}

/**
 * set to a dorsal view.
 * @param  windowTabNumber  Window for which view is requested
 *
 */
void
Model::dorsalView(const int32_t windowTabNumberIn)
{
    /*
     * Yoking ALWAYS uses first window index.
     */
    int32_t windowTabNumber = windowTabNumberIn;
    if (m_isYokingController) {
        windowTabNumber = 0;
    }
    
    CaretAssertArrayIndex(m_viewingRotationMatrix,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          windowTabNumber);
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_NORMAL].identity();
    
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED].identity();
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED].rotateY(-180.0);
    
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE].identity();
    
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT].identity();
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT].rotateY(180.0);
    
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE].identity();
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE].rotateY(180.0);
}

/**
 * set to a ventral view.
 * @param  windowTabNumber  Window for which view is requested
 *
 */
void
Model::ventralView(const int32_t windowTabNumberIn)
{
    /*
     * Yoking ALWAYS uses first window index.
     */
    int32_t windowTabNumber = windowTabNumberIn;
    if (m_isYokingController) {
        windowTabNumber = 0;
    }
    
    CaretAssertArrayIndex(m_viewingRotationMatrix,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          windowTabNumber);
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_NORMAL].identity();
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_NORMAL].rotateY(-180.0);
    
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED].identity();
    
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE].identity();
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE].rotateY(-180.0);
    
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT].identity();
    //viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT].rotateY(-180.0);
    
    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE].identity();
    //viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE].rotateY(-180.0);
}

/**
 * Place the transformations for the given window tab into
 * the userView.
 * @param windowTabNumber
 *    Tab number for transformations.
 * @param userView
 *    View into which transformations are loaded.
 */
void 
Model::getTransformationsInUserView(const int32_t windowTabNumber,
                                                     UserView& userView) const
{
    CaretAssertArrayIndex(m_scaling, 
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, 
                          windowTabNumber);
    
    userView.setTranslation(m_translation[windowTabNumber][VIEWING_TRANSFORM_NORMAL]);
    float m[4][4];
    m_viewingRotationMatrix[windowTabNumber][0].getMatrix(m);
    userView.setRotation(m);
    userView.setScaling(m_scaling[windowTabNumber]);
}

/**
 * Apply the transformations to the given window tab from
 * the userView.
 * @param windowTabNumber
 *    Tab number whose transformations are updated.
 * @param userView
 *    View into which transformations are retrieved.
 */
void 
Model::setTransformationsFromUserView(const int32_t windowTabNumber,
                                                       const UserView& userView)
{
    CaretAssertArrayIndex(m_scaling, 
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, 
                          windowTabNumber);
    

    userView.getTranslation(m_translation[windowTabNumber][VIEWING_TRANSFORM_NORMAL]);
    float m[4][4];
    userView.getRotation(m);
    m_viewingRotationMatrix[windowTabNumber][0].setMatrix(m);
    setScaling(windowTabNumber, userView.getScaling());
}

/**
 * Get a String for use in the GUI.  Use toDescriptiveString() for
 * information about this controller's content.
 *
 * @return String for use in the GUI.
 *
 */
AString
Model::toString() const
{
       return getNameForGUI(true);
}

/**
 * Returns a descriptive string containing info about this instance.
 *
 * @return  String describing contents of this instance.
 *
 */
AString
Model::toDescriptiveString() const
{
    AString s = CaretObject::toString();
    
    return s;
}

/**
 * Get the brain that created this controller.
 * @return The brain.
 */
Brain*
Model::getBrain()
{
    return m_brain;
}

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
SceneClass* 
Model::saveToScene(const SceneAttributes* sceneAttributes,
                     const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "Model",
                                            1);

    /*
     * Do not use scene assistant to save model type since special handling
     * is needed when it is restored.
     */
    sceneClass->addEnumeratedType<ModelTypeEnum, ModelTypeEnum::Enum>("m_modelType", m_modelType);
    
    sceneClass->addFloat("m_defaultModelScaling", m_defaultModelScaling);
    
    if (m_modelType == ModelTypeEnum::MODEL_TYPE_SURFACE) {
        const ModelSurface* surfaceModel = dynamic_cast<ModelSurface*>(this);
        CaretAssert(surfaceModel);
        sceneClass->addString("surfaceName",
                              surfaceModel->getSurface()->getFileNameNoPath());
    }
                          
    
    /*
     * Get indices of tabs that are to be saved to scene.
     */ 
    const std::vector<int32_t> tabIndices = sceneAttributes->getIndicesOfTabsForSavingToScene();
    const int32_t numActiveTabs = static_cast<int32_t>(tabIndices.size()); 
    
    /*
     * Save rotation matrices
     */
    std::vector<SceneClass*> rotationClassVector;
    for (int32_t iat = 0; iat < numActiveTabs; iat++) {
        const int32_t tabIndex = tabIndices[iat];
        for (int32_t ivt = 0; ivt < VIEWING_TRANSFORM_COUNT; ivt++) {
            SceneClass* rotationSceneClass = new SceneClass(("m_viewingRotationMatrix["
                                                             + AString::number(tabIndex)
                                                             + "]["
                                                             + AString::number(ivt)
                                                             + "]"),
                                                            "Matrix4x4",
                                                            1);
            float matrix[4][4];
            m_viewingRotationMatrix[tabIndex][ivt].getMatrix(matrix);
            rotationSceneClass->addInteger("tabIndex", tabIndex);
            rotationSceneClass->addInteger("viewingTransformIndex", ivt);
            rotationSceneClass->addFloatArray("matrix", (float*)matrix, 16);
            
            rotationClassVector.push_back(rotationSceneClass);
        }
        
    }
    SceneClassArray* rotationMatrixClassArray = new SceneClassArray("m_viewingRotationMatrix",
                                                                    rotationClassVector);
    sceneClass->addChild(rotationMatrixClassArray);
    
    /*
     * Save translation
     */
    std::vector<SceneClass*> translationClassVector;
    for (int32_t iat = 0; iat < numActiveTabs; iat++) {
        const int32_t tabIndex = tabIndices[iat];
        for (int32_t ivt = 0; ivt < VIEWING_TRANSFORM_COUNT; ivt++) {
            SceneClass* translationSceneClass = new SceneClass(("m_translation["
                                                                + AString::number(tabIndex)
                                                                + "]["
                                                                + AString::number(ivt)
                                                                + "]"),
                                                               "Matrix4x4",
                                                               1);
            translationSceneClass->addInteger("tabIndex", tabIndex);
            translationSceneClass->addInteger("viewingTransformIndex", ivt);
            translationSceneClass->addFloatArray("translation", m_translation[tabIndex][ivt], 3);
            
            translationClassVector.push_back(translationSceneClass);
        }
        
    }
    SceneClassArray* translationClassArray = new SceneClassArray("m_translation",
                                                                 translationClassVector);
    sceneClass->addChild(translationClassArray);
    
    /*
     * Save scaling
     */
    std::vector<SceneClass*> scalingClassVector;
    for (int32_t iat = 0; iat < numActiveTabs; iat++) {
        const int32_t tabIndex = tabIndices[iat];
        SceneClass* scalingSceneClass = new SceneClass(("m_scaling["
                                                        + AString::number(tabIndex)
                                                        + "]"),
                                                       "Matrix4x4",
                                                       1);
        scalingSceneClass->addInteger("tabIndex", tabIndex);
        scalingSceneClass->addFloat("scaling", m_scaling[tabIndex]);
        
        scalingClassVector.push_back(scalingSceneClass);
    }
    SceneClassArray* scalingClassArray = new SceneClassArray("m_scaling",
                                                             scalingClassVector);
    sceneClass->addChild(scalingClassArray);
    
    /*
     * Save the overlays (except for yoking)
     */
    if (m_modelType != ModelTypeEnum::MODEL_TYPE_YOKING) {
        std::vector<SceneClass*> overlaySetClassVector;
        for (int32_t iat = 0; iat < numActiveTabs; iat++) {
            const int32_t tabIndex = tabIndices[iat];
            SceneClass* overlaySetClass = new SceneClass(("modelOverlay["
                                                          + AString::number(iat)
                                                          + "]"),
                                                         "OverlaySet",
                                                         1);
            overlaySetClass->addInteger("tabIndex",
                                        tabIndex);
            overlaySetClass->addChild(getOverlaySet(tabIndex)->saveToScene(sceneAttributes, 
                                                                           "overlaySet"));
            overlaySetClassVector.push_back(overlaySetClass);
        }
        SceneClassArray* overlaySetClassArray = new SceneClassArray("m_overlaySet",
                                                                    overlaySetClassVector);
        sceneClass->addChild(overlaySetClassArray);
    }
    
    /*
     * Save information specific to the type of model
     */
    saveModelSpecificInformationToScene(sceneAttributes,                                         
                                        sceneClass);
    
    return sceneClass;
}

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
void 
Model::restoreFromScene(const SceneAttributes* sceneAttributes,
                          const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }

    /*
     *  This model was created by the parent scene class.
     *  The model type in the scene should match what was saved.
     *  If not, a serious (programming) error has occurred.
     */
    const ModelTypeEnum::Enum savedModelType = sceneClass->getEnumeratedTypeValue<ModelTypeEnum, ModelTypeEnum::Enum>("m_modelType", 
                                                                                         ModelTypeEnum::MODEL_TYPE_INVALID);
    if (savedModelType == ModelTypeEnum::MODEL_TYPE_INVALID) {
        CaretLogSevere("Non-matching model type when restoring scene: "
                             + ModelTypeEnum::toName(savedModelType));
        return;
    }
    
    if (savedModelType != m_modelType) {
        return;
    }
    
    if (m_modelType == ModelTypeEnum::MODEL_TYPE_SURFACE) {
        const AString surfaceName = sceneClass->getStringValue("surfaceName",
                                                               "NOT-FOUND");
        const ModelSurface* surfaceModel = dynamic_cast<ModelSurface*>(this);
        CaretAssert(surfaceModel);        
        if (surfaceName != surfaceModel->getSurface()->getFileNameNoPath()) {
            /*
             * Exit as this is not the surface for restoring (name does not match)
             */
            return;
        }
    }
    
    /*
     * Reset ALL transformation to defaults
     */
    for (int32_t iTab = 0; iTab < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; iTab++) {
        resetView(iTab);
    }
    
    /*
     * Restore scaling
     */
    m_defaultModelScaling = sceneClass->getFloatValue("m_defaultModelScaling",
                                                      1.0);
    
    /*
     * Restore scaling
     */
    const SceneClassArray* scalingClassArray = sceneClass->getClassArray("m_scaling");
    if (scalingClassArray != NULL) {
        const int32_t numSavedScaling = scalingClassArray->getNumberOfArrayElements();
        for (int32_t ism = 0; ism < numSavedScaling; ism++) {
            const SceneClass* scalingClass = scalingClassArray->getClassAtIndex(ism);
            const int32_t tabIndex = scalingClass->getIntegerValue("tabIndex", -1);
            if (tabIndex >= 0) {
                m_scaling[tabIndex] = scalingClass->getFloatValue("scaling", 1.0);
            }
        }
    }
    
    /*
     * Restore translation
     */
    const SceneClassArray* translationClassArray = sceneClass->getClassArray("m_translation");
    if (translationClassArray != NULL) {
        const int32_t numSavedTanslations = translationClassArray->getNumberOfArrayElements();
        for (int32_t ism = 0; ism < numSavedTanslations; ism++) {
            const SceneClass* translationClass = translationClassArray->getClassAtIndex(ism);
            const int32_t tabIndex = translationClass->getIntegerValue("tabIndex", -1);
            const int32_t viewingTransformIndex = translationClass->getIntegerValue("viewingTransformIndex", -1);
            if ((tabIndex >= 0)
                && (viewingTransformIndex >= 0)) {
                if (translationClass->getFloatArrayValue("translation", 
                                                     m_translation[tabIndex][viewingTransformIndex], 
                                                         3) != 3) {
                    setTranslation(tabIndex, (ViewingTransformIndex)viewingTransformIndex, 0.0, 0.0, 0.0);
                }
            }
        }
    }
    
    /*
     * Restore rotation matrices
     */
    const SceneClassArray* rotationMatrixClassArray = sceneClass->getClassArray("m_viewingRotationMatrix");
    if (rotationMatrixClassArray != NULL) {
        const int32_t numSavedMatrices = rotationMatrixClassArray->getNumberOfArrayElements();
        for (int32_t ism = 0; ism < numSavedMatrices; ism++) {
            const SceneClass* rotationMatrixClass = rotationMatrixClassArray->getClassAtIndex(ism);
            const int32_t tabIndex = rotationMatrixClass->getIntegerValue("tabIndex", -1);
            const int32_t viewingTransformIndex = rotationMatrixClass->getIntegerValue("viewingTransformIndex", -1);
            if ((tabIndex >= 0)
                && (viewingTransformIndex >= 0)) {
                float matrix[4][4];
                if (rotationMatrixClass->getFloatArrayValue("matrix", 
                                                        (float*)matrix, 
                                                            16) == 16) {
                    m_viewingRotationMatrix[tabIndex][viewingTransformIndex].setMatrix(matrix);
                }
                else {
                    m_viewingRotationMatrix[tabIndex][viewingTransformIndex].identity();
                }
            }
        }
    }
    
    /*
     * Restore the overlays (except for yoking)
     */
    if (m_modelType != ModelTypeEnum::MODEL_TYPE_YOKING) {
        const SceneClassArray* overlaySetClassArray = sceneClass->getClassArray("m_overlaySet");
        if (overlaySetClassArray != NULL) {
            const int32_t numSavedOverlaySets = overlaySetClassArray->getNumberOfArrayElements();
            for (int32_t i = 0; i < numSavedOverlaySets; i++) {
                const SceneClass* overlaySceneClass = overlaySetClassArray->getClassAtIndex(i);
                const int32_t tabIndex = overlaySceneClass->getIntegerValue("tabIndex",
                                                                            -1);
                const SceneClass* overlayClass = overlaySceneClass->getClass("overlaySet");
                if ((tabIndex >= 0) 
                    && (overlayClass != NULL)) {
                    getOverlaySet(tabIndex)->restoreFromScene(sceneAttributes, 
                                                              overlayClass);
                }
            }
        }
    }
    
    /*
     * Restore any information specific to type of model
     */
    restoreModelSpecificInformationFromScene(sceneAttributes, 
                                             sceneClass);
}




