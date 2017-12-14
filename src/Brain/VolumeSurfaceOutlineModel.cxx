
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

#define __VOLUME_SURFACE_OUTLINE_MODEL_DECLARE__
#include "VolumeSurfaceOutlineModel.h"
#undef __VOLUME_SURFACE_OUTLINE_MODEL_DECLARE__

#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "SurfaceSelectionModel.h"
#include "SurfaceTypeEnum.h"
#include "VolumeSurfaceOutlineColorOrTabModel.h"

using namespace caret;


    
/**
 * \class VolumeSurfaceOutlineSelection 
 * \brief Controls display of a volume surface outline.
 *
 * Controls display of a volume surface outline.
 */
/**
 * Constructor.
 */
VolumeSurfaceOutlineModel::VolumeSurfaceOutlineModel()
: CaretObject()
{
    std::vector<SurfaceTypeEnum::Enum> validSurfaceTypes;
    validSurfaceTypes.push_back(SurfaceTypeEnum::ANATOMICAL);
    validSurfaceTypes.push_back(SurfaceTypeEnum::RECONSTRUCTION);
    validSurfaceTypes.push_back(SurfaceTypeEnum::INFLATED);
    validSurfaceTypes.push_back(SurfaceTypeEnum::VERY_INFLATED);
    
    m_displayed = false;
    m_thicknessPixelsObsolete = VolumeSurfaceOutlineModel::DEFAULT_LINE_THICKNESS_PIXELS_OBSOLETE;
    m_surfaceSelectionModel = new SurfaceSelectionModel(validSurfaceTypes);
    m_colorOrTabModel = new VolumeSurfaceOutlineColorOrTabModel();
    
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add("m_displayed", &m_displayed);
    m_sceneAssistant->add("m_thickness", &m_thicknessPixelsObsolete); // NOTE: "m_thickness" is OLD name
    m_sceneAssistant->add("m_surfaceSelectionModel", "SurfaceSelectionModel", m_surfaceSelectionModel);
    m_sceneAssistant->add("m_colorOrTabModel", "VolumeSurfaceOutlineColorOrTabModel", m_colorOrTabModel);
    
    /*
     * Millimeter thickness was added in December 2017.
     * So, we set the millimeter thickness to -1.0f so that when an older
     * scene is restored, the value will be -1.0f.  The code that draws
     * the volume surface outline see this negative value and convert
     * the old pixel thickness to this new millimeter thickness.
     * After adding to the scene assistant, default the millimeter thickness.
     */
    m_thicknessMillimeters = -1.0f;
    m_sceneAssistant->add("m_thicknessMillimeters", &m_thicknessMillimeters);
    m_thicknessMillimeters = VolumeSurfaceOutlineModel::DEFAULT_LINE_THICKNESS_MILLIMETERS;
}

/**
 * Destructor.
 */
VolumeSurfaceOutlineModel::~VolumeSurfaceOutlineModel()
{
    delete m_surfaceSelectionModel;
    delete m_colorOrTabModel;
    
    delete m_sceneAssistant;
}

/**
 * Copy the given volume surface outline model to this model.
 * @param modelToCopy
 *    Model that is copied.
 */
void 
VolumeSurfaceOutlineModel::copyVolumeSurfaceOutlineModel(VolumeSurfaceOutlineModel* modelToCopy)
{
    m_displayed = modelToCopy->m_displayed;
    m_thicknessPixelsObsolete = modelToCopy->m_thicknessPixelsObsolete;
    m_thicknessMillimeters = modelToCopy->m_thicknessMillimeters;
    m_surfaceSelectionModel->setSurface(modelToCopy->getSurface());
    
    VolumeSurfaceOutlineColorOrTabModel* colorTabToCopy = modelToCopy->getColorOrTabModel();
    m_colorOrTabModel->copyVolumeSurfaceOutlineColorOrTabModel(colorTabToCopy);
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
VolumeSurfaceOutlineModel::toString() const
{
    return "VolumeSurfaceOutlineSelection";
}

/**
 * @return Is this surface outline displayed?
 */
bool 
VolumeSurfaceOutlineModel::isDisplayed() const
{
    return m_displayed;
}

/**
 * Set the display status of the surface outline.
 * @param displayed
 *    New display status.
 */
void 
VolumeSurfaceOutlineModel::setDisplayed(const bool displayed)
{
    m_displayed = displayed;
}

/**
 * @return Thickness for drawing surface outline
 */
float
VolumeSurfaceOutlineModel::getThicknessMillimeters() const
{
    return m_thicknessMillimeters;
}

/**
 * Set the thickness for drawing surface outline
 */
void
VolumeSurfaceOutlineModel::setThicknessMillimeters(const float thickness)
{
    m_thicknessMillimeters = thickness;
}

/**
 * @return Thickness for drawing surface (OBSOLETE)
 */
float 
VolumeSurfaceOutlineModel::getThicknessPixelsObsolete() const
{
    return m_thicknessPixelsObsolete;
}

/**
 * Set the thickness for drawing the surface (OBSOLETE)
 * @param thickness
 *    New value for thickness.
 */
void 
VolumeSurfaceOutlineModel::setThicknessPixelsObsolete(const float thickness)
{
    m_thicknessPixelsObsolete = thickness;
}

/**
 * @return  The surface selector used to select the surface.
 */
SurfaceSelectionModel* 
VolumeSurfaceOutlineModel::getSurfaceSelectionModel()
{
    return m_surfaceSelectionModel;
}

/**
 * @return  Get the selected surface.
 */
const Surface* 
VolumeSurfaceOutlineModel::getSurface() const
{
    return m_surfaceSelectionModel->getSurface();
}

/**
 * @return  Get the selected surface.
 */
Surface* 
VolumeSurfaceOutlineModel::getSurface()
{
    return m_surfaceSelectionModel->getSurface();
}

/**
 * @return The model for color or tab selection.
 */
VolumeSurfaceOutlineColorOrTabModel* 
VolumeSurfaceOutlineModel::getColorOrTabModel()
{
    return m_colorOrTabModel;
}

/**
 * @return The model for color or tab selection.
 */
const VolumeSurfaceOutlineColorOrTabModel* 
VolumeSurfaceOutlineModel::getColorOrTabModel() const
{
    return m_colorOrTabModel;
}

/**
 * Create a scene for an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @return Pointer to SceneClass object representing the state of 
 *    this object.  Under some circumstances a NULL pointer may be
 *    returned.  Caller will take ownership of returned object.
 */
SceneClass* 
VolumeSurfaceOutlineModel::saveToScene(const SceneAttributes* sceneAttributes,
                                      const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "VolumeSurfaceOutlineModel",
                                            1);
    
    m_sceneAssistant->saveMembers(sceneAttributes, 
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
 *     SceneClass containing the state that was previously 
 *     saved and should be restored.
 */
void 
VolumeSurfaceOutlineModel::restoreFromScene(const SceneAttributes* sceneAttributes,
                                           const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes, 
                                     sceneClass);
}
