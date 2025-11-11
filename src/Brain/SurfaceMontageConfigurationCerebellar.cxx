
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

#define __SURFACE_MONTAGE_CONFIGURATION_CEREBELLAR_DECLARE__
#include "SurfaceMontageConfigurationCerebellar.h"
#undef __SURFACE_MONTAGE_CONFIGURATION_CEREBELLAR_DECLARE__

#include "CaretAssert.h"
#include "PlainTextStringBuilder.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "Surface.h"
#include "SurfaceSelectionModel.h"

using namespace caret;


    
/**
 * \class caret::SurfaceMontageConfigurationCerebellar 
 * \brief Surface montage configuration for cerebellum.
 * \ingroup Brain
 */

/**
 * Constructor.
 */
SurfaceMontageConfigurationCerebellar::SurfaceMontageConfigurationCerebellar(const int32_t tabIndex)
: SurfaceMontageConfigurationAbstract(SurfaceMontageConfigurationTypeEnum::CEREBELLAR_CORTEX_CONFIGURATION,
                                      SUPPORTS_LAYOUT_ORIENTATION_YES)
{
    std::vector<SurfaceTypeEnum::Enum> validSurfaceTypes;
    SurfaceTypeEnum::getAllEnumsExceptFlat(validSurfaceTypes);
    
    m_firstSurfaceSelectionModel = new SurfaceSelectionModel(StructureEnum::CEREBELLUM,
                                                             validSurfaceTypes);
    m_secondSurfaceSelectionModel = new SurfaceSelectionModel(StructureEnum::CEREBELLUM,
                                                              validSurfaceTypes);
    m_firstSurfaceEnabled = true;
    m_secondSurfaceEnabled = false;
    m_dorsalEnabled = true;
    m_ventralEnabled  = true;
    m_anteriorEnabled = true;
    m_posteriorEnabled = true;
    
    m_sceneAssistant = new SceneClassAssistant();
    
    m_sceneAssistant->add("m_firstSurfaceSelectionModel",
                          "SurfaceSelectionModel",
                          m_firstSurfaceSelectionModel);
    m_sceneAssistant->add("m_secondSurfaceSelectionModel",
                          "SurfaceSelectionModel",
                          m_secondSurfaceSelectionModel);
    
    m_sceneAssistant->add("m_firstSurfaceEnabled",
                          &m_firstSurfaceEnabled);
    m_sceneAssistant->add("m_secondSurfaceEnabled",
                          &m_secondSurfaceEnabled);
    
    m_sceneAssistant->add("m_dorsalEnabled",
                          &m_dorsalEnabled);
    m_sceneAssistant->add("m_ventralEnabled",
                          &m_ventralEnabled);
    m_sceneAssistant->add("m_anteriorEnabled",
                          &m_anteriorEnabled);
    m_sceneAssistant->add("m_posteriorEnabled",
                          &m_posteriorEnabled);
    
    std::vector<StructureEnum::Enum> supportedStructures;
    supportedStructures.push_back(StructureEnum::CEREBELLUM);
    setupOverlaySet("Cerebellar Montage",
                    tabIndex,
                    supportedStructures);
}

/**
 * Destructor.
 */
SurfaceMontageConfigurationCerebellar::~SurfaceMontageConfigurationCerebellar()
{
    delete m_firstSurfaceSelectionModel;
    delete m_secondSurfaceSelectionModel;
    
    delete m_sceneAssistant;
}

/**
 * @return Is this configuration valid?
 */
bool
SurfaceMontageConfigurationCerebellar::isValid()
{
    const bool valid = (getFirstSurfaceSelectionModel()->getSurface() != NULL);
    return valid;
}

/**
 * Initialize the selected surfaces.
 */
void
SurfaceMontageConfigurationCerebellar::initializeSelectedSurfaces()
{
    
}

/**
 * @return First surface selection model.
 */
SurfaceSelectionModel*
SurfaceMontageConfigurationCerebellar::getFirstSurfaceSelectionModel()
{
    return m_firstSurfaceSelectionModel;
}

/**
 * @return First surface selection model.
 */
const SurfaceSelectionModel*
SurfaceMontageConfigurationCerebellar::getFirstSurfaceSelectionModel() const
{
    return m_firstSurfaceSelectionModel;
}

/**
 * @return Second surface selection model.
 */
SurfaceSelectionModel*
SurfaceMontageConfigurationCerebellar::getSecondSurfaceSelectionModel()
{
    return m_secondSurfaceSelectionModel;
}

/**
 * @return Second surface selection model.
 */
const SurfaceSelectionModel*
SurfaceMontageConfigurationCerebellar::getSecondSurfaceSelectionModel() const
{
    return m_secondSurfaceSelectionModel;
}

/**
 * @return Is first surface enabled.
 */
bool
SurfaceMontageConfigurationCerebellar::isFirstSurfaceEnabled() const
{
    return m_firstSurfaceEnabled;
}

/**
 * Set first surface enabled status.
 * 
 * @param enabled
 *     New enabled status.
 */
void
SurfaceMontageConfigurationCerebellar::setFirstSurfaceEnabled(const bool enabled)
{
    m_firstSurfaceEnabled = enabled;
}

/**
 * @return Is second surface enabled.
 */
bool
SurfaceMontageConfigurationCerebellar::isSecondSurfaceEnabled() const
{
    return m_secondSurfaceEnabled;
}

/**
 * Set first surface enabled status.
 *
 * @param enabled
 *     New enabled status.
 */
void
SurfaceMontageConfigurationCerebellar::setSecondSurfaceEnabled(const bool enabled)
{
    m_secondSurfaceEnabled = enabled;
}

/**
 * @return Is dorsal enabled.
 */
bool
SurfaceMontageConfigurationCerebellar::isDorsalEnabled() const
{
    return m_dorsalEnabled;
}

/**
 * Set dorsal enabled.
 *
 * @param enabled
 *    New enabled status.
 */
void
SurfaceMontageConfigurationCerebellar::setDorsalEnabled(const bool enabled)
{
    m_dorsalEnabled = enabled;
}

/**
 * @return Is ventral enabled.
 */
bool
SurfaceMontageConfigurationCerebellar::isVentralEnabled() const
{
   return  m_ventralEnabled;
}

/**
 * Set ventral enabled.
 *
 * @param enabled
 *    New enabled status.
 */
void
SurfaceMontageConfigurationCerebellar::setVentralEnabled(const bool enabled)
{
    m_ventralEnabled = enabled;
}

/**
 * @return Is anterior enabled.
 */
bool
SurfaceMontageConfigurationCerebellar::isAnteriorEnabled() const
{
    return m_anteriorEnabled;
}

/**
 * Set anterior enabled.
 *
 * @param enabled
 *    New enabled status.
 */
void
SurfaceMontageConfigurationCerebellar::setAnteriorEnabled(const bool enabled)
{
    m_anteriorEnabled = enabled;
}

/**
 * @return Is posterior enabled.
 */
bool
SurfaceMontageConfigurationCerebellar::isPosteriorEnabled() const
{
    return m_posteriorEnabled;
}

/**
 * Set posterior enabled.
 *
 * @param enabled
 *    New enabled status.
 */
void
SurfaceMontageConfigurationCerebellar::setPosteriorEnabled(const bool enabled)
{
    m_posteriorEnabled = enabled;
}

/**
 * Update the montage viewports using the current selected surfaces and settings.
 *
 * @param surfaceMontageViewports
 *     Will be loaded with the montage viewports.
 */
void
SurfaceMontageConfigurationCerebellar::updateSurfaceMontageViewports(std::vector<SurfaceMontageViewport>& surfaceMontageViewports)
{
    surfaceMontageViewports.clear();
    
    std::vector<SurfaceMontageViewport> anteriorViewports;
    std::vector<SurfaceMontageViewport> dorsalViewports;
    std::vector<SurfaceMontageViewport> posteriorViewports;
    std::vector<SurfaceMontageViewport> ventralViewports;
    
    int32_t numFirst = 0;
    if (m_firstSurfaceEnabled) {
        Surface* surface = m_firstSurfaceSelectionModel->getSurface();
        if (surface != NULL) {
            if (m_anteriorEnabled) {
                anteriorViewports.push_back(SurfaceMontageViewport(surface,
                                                                   surface,
                                                                   ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_ANTERIOR));
                numFirst++;
            }
            if (m_dorsalEnabled) {
                dorsalViewports.push_back(SurfaceMontageViewport(surface,
                                                                 surface,
                                                                 ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_DORSAL));
                numFirst++;
            }
            if (m_posteriorEnabled) {
                posteriorViewports.push_back(SurfaceMontageViewport(surface,
                                                                    surface,
                                                                    ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_POSTERIOR));
                numFirst++;
            }
            if (m_ventralEnabled) {
                ventralViewports.push_back(SurfaceMontageViewport(surface,
                                                                  surface,
                                                                 ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_VENTRAL));
                numFirst++;
            }
        }
    }
    
    int32_t numSecond = 0;
    if (m_secondSurfaceEnabled) {
        Surface* surface = m_secondSurfaceSelectionModel->getSurface();
        if (surface != NULL) {
            if (m_anteriorEnabled) {
                anteriorViewports.push_back(SurfaceMontageViewport(surface,
                                                                   surface,
                                                                   ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_ANTERIOR));
                numSecond++;
            }
            if (m_dorsalEnabled) {
                dorsalViewports.push_back(SurfaceMontageViewport(surface,
                                                                 surface,
                                                                 ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_DORSAL));
                numSecond++;
            }
            if (m_posteriorEnabled) {
                posteriorViewports.push_back(SurfaceMontageViewport(surface,
                                                                    surface,
                                                                    ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_POSTERIOR));
                numSecond++;
            }
            if (m_ventralEnabled) {
                ventralViewports.push_back(SurfaceMontageViewport(surface,
                                                                  surface,
                                                                  ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_VENTRAL));
                numSecond++;
            }
        }
    }
    
    std::vector<SurfaceMontageViewport> allViewports;
    allViewports.insert(allViewports.end(),
                                   dorsalViewports.begin(),
                                   dorsalViewports.end());
    allViewports.insert(allViewports.end(),
                                   ventralViewports.begin(),
                                   ventralViewports.end());
    allViewports.insert(allViewports.end(),
                                   anteriorViewports.begin(),
                                   anteriorViewports.end());
    allViewports.insert(allViewports.end(),
                                   posteriorViewports.begin(),
                                   posteriorViewports.end());
    const int32_t totalNum = numFirst + numSecond;

    CaretAssert(static_cast<int32_t>(allViewports.size()) == totalNum);
    
    if (totalNum == 1) {
        surfaceMontageViewports.insert(surfaceMontageViewports.end(),
                                       allViewports.begin(),
                                       allViewports.end());
        CaretAssert(surfaceMontageViewports.size() == 1);
        
        surfaceMontageViewports[0].setRowAndColumn(0, 0);
        
    }
    else if (totalNum == 2) {
        surfaceMontageViewports.insert(surfaceMontageViewports.end(),
                                       allViewports.begin(),
                                       allViewports.end());
        
        CaretAssert(surfaceMontageViewports.size() == 2);
        
        switch (getLayoutOrientation()) {
            case SurfaceMontageLayoutOrientationEnum::COLUMN_LAYOUT_ORIENTATION:
                surfaceMontageViewports[0].setRowAndColumn(0, 0);
                surfaceMontageViewports[1].setRowAndColumn(1, 0);
                break;
            case SurfaceMontageLayoutOrientationEnum::LANDSCAPE_LAYOUT_ORIENTATION:
                surfaceMontageViewports[0].setRowAndColumn(0, 0);
                surfaceMontageViewports[1].setRowAndColumn(0, 1);
                break;
            case SurfaceMontageLayoutOrientationEnum::LANDSCAPE_LAYOUT_ORIENTATION_TRANSPOSED:
                surfaceMontageViewports[0].setRowAndColumn(0, 0);
                surfaceMontageViewports[1].setRowAndColumn(0, 1);
                break;
            case SurfaceMontageLayoutOrientationEnum::PORTRAIT_LAYOUT_ORIENTATION:
                surfaceMontageViewports[0].setRowAndColumn(0, 0);
                surfaceMontageViewports[1].setRowAndColumn(1, 0);
                break;
            case SurfaceMontageLayoutOrientationEnum::PORTRAIT_LAYOUT_ORIENTATION_TRANSPOSED:
                surfaceMontageViewports[0].setRowAndColumn(0, 0);
                surfaceMontageViewports[1].setRowAndColumn(1, 0);
                break;
            case SurfaceMontageLayoutOrientationEnum::ROW_LAYOUT_ORIENTATION:
                surfaceMontageViewports[0].setRowAndColumn(0, 0);
                surfaceMontageViewports[1].setRowAndColumn(0, 1);
                break;
        }
    }
    else if (totalNum == 3) {
        surfaceMontageViewports.insert(surfaceMontageViewports.end(),
                                       allViewports.begin(),
                                       allViewports.end());
        
        CaretAssert(surfaceMontageViewports.size() == 3);
        
        int32_t dorsalIndex(-1);
        int32_t ventralIndex(-1);
        int32_t anteriorIndex(-1);
        int32_t posteriorIndex(-1);
        for (int32_t i = 0; i < 3; i++) {
            SurfaceMontageViewport& svp = surfaceMontageViewports[i];
            switch (svp.getProjectionViewType()) {
                case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_ANTERIOR:
                    svp.setRowAndColumn(1, 0);
                    anteriorIndex = i;
                    break;
                case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_DORSAL:
                    svp.setRowAndColumn(0, 0);
                    dorsalIndex = i;
                    break;
                case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_POSTERIOR:
                    svp.setRowAndColumn(1, 1);
                    posteriorIndex = i;
                    break;
                case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_VENTRAL:
                    svp.setRowAndColumn(0, 1);
                    ventralIndex = i;
                    break;
                default:
                    CaretAssert(0);
            }
        }
        switch (getLayoutOrientation()) {
            case SurfaceMontageLayoutOrientationEnum::COLUMN_LAYOUT_ORIENTATION:
            {
                int32_t columnIndex = 0;
                if (dorsalIndex >= 0) {
                    CaretAssertVectorIndex(surfaceMontageViewports, dorsalIndex);
                    surfaceMontageViewports[dorsalIndex].setRowAndColumn(columnIndex, 0);
                    columnIndex++;
                }
                if (ventralIndex >= 0) {
                    CaretAssertVectorIndex(surfaceMontageViewports, ventralIndex);
                    surfaceMontageViewports[ventralIndex].setRowAndColumn(columnIndex, 0);
                    columnIndex++;
                }
                if (anteriorIndex >= 0) {
                    CaretAssertVectorIndex(surfaceMontageViewports, anteriorIndex);
                    surfaceMontageViewports[anteriorIndex].setRowAndColumn(columnIndex, 0);
                    columnIndex++;
                }
                if (posteriorIndex >= 0) {
                    CaretAssertVectorIndex(surfaceMontageViewports, posteriorIndex);
                    surfaceMontageViewports[posteriorIndex].setRowAndColumn(columnIndex, 0);
                    columnIndex++;
                }
            }
                break;
            case SurfaceMontageLayoutOrientationEnum::LANDSCAPE_LAYOUT_ORIENTATION:
                break;
            case SurfaceMontageLayoutOrientationEnum::LANDSCAPE_LAYOUT_ORIENTATION_TRANSPOSED:
                break;
            case SurfaceMontageLayoutOrientationEnum::PORTRAIT_LAYOUT_ORIENTATION:
                break;
            case SurfaceMontageLayoutOrientationEnum::PORTRAIT_LAYOUT_ORIENTATION_TRANSPOSED:
                break;
            case SurfaceMontageLayoutOrientationEnum::ROW_LAYOUT_ORIENTATION:
            {
                int32_t rowIndex = 0;
                if (dorsalIndex >= 0) {
                    CaretAssertVectorIndex(surfaceMontageViewports, dorsalIndex);
                    surfaceMontageViewports[dorsalIndex].setRowAndColumn(0, rowIndex);
                    rowIndex++;
                }
                if (ventralIndex >= 0) {
                    CaretAssertVectorIndex(surfaceMontageViewports, ventralIndex);
                    surfaceMontageViewports[ventralIndex].setRowAndColumn(0, rowIndex);
                    rowIndex++;
                }
                if (anteriorIndex >= 0) {
                    CaretAssertVectorIndex(surfaceMontageViewports, anteriorIndex);
                    surfaceMontageViewports[anteriorIndex].setRowAndColumn(0, rowIndex);
                    rowIndex++;
                }
                if (posteriorIndex >= 0) {
                    CaretAssertVectorIndex(surfaceMontageViewports, posteriorIndex);
                    surfaceMontageViewports[posteriorIndex].setRowAndColumn(0, rowIndex);
                    rowIndex++;
                }
            }
                break;
        }
    }
    else if (totalNum == 4) {
        if ((numFirst == 4)
            || (numSecond == 4)){
            surfaceMontageViewports.insert(surfaceMontageViewports.end(),
                                           allViewports.begin(),
                                           allViewports.end());
            CaretAssert(surfaceMontageViewports.size() == 4);
            surfaceMontageViewports[0].setRowAndColumn(0, 0);
            surfaceMontageViewports[1].setRowAndColumn(0, 1);
            surfaceMontageViewports[2].setRowAndColumn(1, 0);
            surfaceMontageViewports[3].setRowAndColumn(1, 1);
        }
        else if (numFirst == numSecond) {
            surfaceMontageViewports.insert(surfaceMontageViewports.end(),
                                           allViewports.begin(),
                                           allViewports.end());
            CaretAssert(surfaceMontageViewports.size() == 4);
            
            surfaceMontageViewports[0].setRowAndColumn(0, 0);
            surfaceMontageViewports[1].setRowAndColumn(0, 1);
            surfaceMontageViewports[2].setRowAndColumn(1, 0);
            surfaceMontageViewports[3].setRowAndColumn(1, 1);
        }
        else {
            CaretAssert(0);
        }
        
        switch (getLayoutOrientation()) {
            case SurfaceMontageLayoutOrientationEnum::COLUMN_LAYOUT_ORIENTATION:
                surfaceMontageViewports[0].setRowAndColumn(0, 0);
                surfaceMontageViewports[2].setRowAndColumn(1, 0);
                surfaceMontageViewports[1].setRowAndColumn(2, 0);
                surfaceMontageViewports[3].setRowAndColumn(3, 0);
                break;
            case SurfaceMontageLayoutOrientationEnum::LANDSCAPE_LAYOUT_ORIENTATION:
                break;
            case SurfaceMontageLayoutOrientationEnum::LANDSCAPE_LAYOUT_ORIENTATION_TRANSPOSED:
                break;
            case SurfaceMontageLayoutOrientationEnum::PORTRAIT_LAYOUT_ORIENTATION:
                break;
            case SurfaceMontageLayoutOrientationEnum::PORTRAIT_LAYOUT_ORIENTATION_TRANSPOSED:
                break;
            case SurfaceMontageLayoutOrientationEnum::ROW_LAYOUT_ORIENTATION:
                surfaceMontageViewports[0].setRowAndColumn(0, 0);
                surfaceMontageViewports[2].setRowAndColumn(0, 1);
                surfaceMontageViewports[1].setRowAndColumn(0, 2);
                surfaceMontageViewports[3].setRowAndColumn(0, 3);
                break;
        }
    }
    else if (totalNum == 6) {
        surfaceMontageViewports.insert(surfaceMontageViewports.end(),
                                       allViewports.begin(),
                                       allViewports.end());
        CaretAssert(surfaceMontageViewports.size() == 6);
        
        switch (getLayoutOrientation()) {
            case SurfaceMontageLayoutOrientationEnum::COLUMN_LAYOUT_ORIENTATION:
                surfaceMontageViewports[0].setRowAndColumn(0, 0);
                surfaceMontageViewports[2].setRowAndColumn(1, 0);
                surfaceMontageViewports[4].setRowAndColumn(2, 0);
                surfaceMontageViewports[1].setRowAndColumn(3, 0);
                surfaceMontageViewports[3].setRowAndColumn(4, 0);
                surfaceMontageViewports[5].setRowAndColumn(5, 0);
                break;
            case SurfaceMontageLayoutOrientationEnum::LANDSCAPE_LAYOUT_ORIENTATION:
                surfaceMontageViewports[0].setRowAndColumn(0, 0);
                surfaceMontageViewports[1].setRowAndColumn(1, 0);
                surfaceMontageViewports[2].setRowAndColumn(0, 1);
                surfaceMontageViewports[3].setRowAndColumn(1, 1);
                surfaceMontageViewports[4].setRowAndColumn(0, 2);
                surfaceMontageViewports[5].setRowAndColumn(1, 2);
                break;
            case SurfaceMontageLayoutOrientationEnum::LANDSCAPE_LAYOUT_ORIENTATION_TRANSPOSED:
                surfaceMontageViewports[0].setRowAndColumn(0, 0);
                surfaceMontageViewports[1].setRowAndColumn(1, 0);
                surfaceMontageViewports[2].setRowAndColumn(0, 1);
                surfaceMontageViewports[3].setRowAndColumn(1, 1);
                surfaceMontageViewports[4].setRowAndColumn(0, 2);
                surfaceMontageViewports[5].setRowAndColumn(1, 2);
                break;
            case SurfaceMontageLayoutOrientationEnum::PORTRAIT_LAYOUT_ORIENTATION:
                surfaceMontageViewports[0].setRowAndColumn(0, 0);
                surfaceMontageViewports[1].setRowAndColumn(0, 1);
                surfaceMontageViewports[2].setRowAndColumn(1, 0);
                surfaceMontageViewports[3].setRowAndColumn(1, 1);
                surfaceMontageViewports[4].setRowAndColumn(2, 0);
                surfaceMontageViewports[5].setRowAndColumn(2, 1);
                break;
            case SurfaceMontageLayoutOrientationEnum::PORTRAIT_LAYOUT_ORIENTATION_TRANSPOSED:
                surfaceMontageViewports[0].setRowAndColumn(0, 0);
                surfaceMontageViewports[1].setRowAndColumn(0, 1);
                surfaceMontageViewports[2].setRowAndColumn(1, 0);
                surfaceMontageViewports[3].setRowAndColumn(1, 1);
                surfaceMontageViewports[4].setRowAndColumn(2, 0);
                surfaceMontageViewports[5].setRowAndColumn(2, 1);
                break;
            case SurfaceMontageLayoutOrientationEnum::ROW_LAYOUT_ORIENTATION:
                surfaceMontageViewports[0].setRowAndColumn(0, 0);
                surfaceMontageViewports[2].setRowAndColumn(0, 1);
                surfaceMontageViewports[4].setRowAndColumn(0, 2);
                surfaceMontageViewports[1].setRowAndColumn(0, 3);
                surfaceMontageViewports[3].setRowAndColumn(0, 4);
                surfaceMontageViewports[5].setRowAndColumn(0, 5);
                break;
        }
    }
    else if (totalNum == 8) {
        surfaceMontageViewports.insert(surfaceMontageViewports.end(),
                                       allViewports.begin(),
                                       allViewports.end());
        CaretAssert(surfaceMontageViewports.size() == 8);
        
        switch (getLayoutOrientation()) {
            case SurfaceMontageLayoutOrientationEnum::COLUMN_LAYOUT_ORIENTATION:
                surfaceMontageViewports[0].setRowAndColumn(0, 0);
                surfaceMontageViewports[2].setRowAndColumn(1, 0);
                surfaceMontageViewports[4].setRowAndColumn(2, 0);
                surfaceMontageViewports[6].setRowAndColumn(3, 0);
                surfaceMontageViewports[1].setRowAndColumn(4, 0);
                surfaceMontageViewports[3].setRowAndColumn(5, 0);
                surfaceMontageViewports[5].setRowAndColumn(6, 0);
                surfaceMontageViewports[7].setRowAndColumn(7, 0);
                break;
            case SurfaceMontageLayoutOrientationEnum::LANDSCAPE_LAYOUT_ORIENTATION:
                surfaceMontageViewports[0].setRowAndColumn(0, 0);
                surfaceMontageViewports[1].setRowAndColumn(0, 2);
                surfaceMontageViewports[2].setRowAndColumn(0, 1);
                surfaceMontageViewports[3].setRowAndColumn(0, 3);
                surfaceMontageViewports[4].setRowAndColumn(1, 0);
                surfaceMontageViewports[5].setRowAndColumn(1, 2);
                surfaceMontageViewports[6].setRowAndColumn(1, 1);
                surfaceMontageViewports[7].setRowAndColumn(1, 3);
                break;
            case SurfaceMontageLayoutOrientationEnum::LANDSCAPE_LAYOUT_ORIENTATION_TRANSPOSED:
                surfaceMontageViewports[0].setRowAndColumn(0, 0);
                surfaceMontageViewports[1].setRowAndColumn(1, 0); //0, 2);//s
                surfaceMontageViewports[2].setRowAndColumn(0, 1);
                surfaceMontageViewports[3].setRowAndColumn(1, 1); //0, 3); //s
                surfaceMontageViewports[4].setRowAndColumn(0, 2); //1, 0);//s
                surfaceMontageViewports[5].setRowAndColumn(1, 2);
                surfaceMontageViewports[6].setRowAndColumn(0, 3); //1, 1); //s
                surfaceMontageViewports[7].setRowAndColumn(1, 3);
                break;
            case SurfaceMontageLayoutOrientationEnum::PORTRAIT_LAYOUT_ORIENTATION:
                surfaceMontageViewports[0].setRowAndColumn(0, 0);
                surfaceMontageViewports[1].setRowAndColumn(0, 1);
                surfaceMontageViewports[2].setRowAndColumn(1, 0);
                surfaceMontageViewports[3].setRowAndColumn(1, 1);
                surfaceMontageViewports[4].setRowAndColumn(2, 0);
                surfaceMontageViewports[5].setRowAndColumn(2, 1);
                surfaceMontageViewports[6].setRowAndColumn(3, 0);
                surfaceMontageViewports[7].setRowAndColumn(3, 1);
                break;
            case SurfaceMontageLayoutOrientationEnum::PORTRAIT_LAYOUT_ORIENTATION_TRANSPOSED:
                surfaceMontageViewports[0].setRowAndColumn(0, 0);
                surfaceMontageViewports[1].setRowAndColumn(0, 1);
                surfaceMontageViewports[2].setRowAndColumn(2, 0); //1, 0);//ss
                surfaceMontageViewports[3].setRowAndColumn(2, 1); //1, 1); //s
                surfaceMontageViewports[4].setRowAndColumn(1, 0); //2, 0);//s
                surfaceMontageViewports[5].setRowAndColumn(1, 1); //2, 1); //s
                surfaceMontageViewports[6].setRowAndColumn(3, 0);
                surfaceMontageViewports[7].setRowAndColumn(3, 1);
                break;
            case SurfaceMontageLayoutOrientationEnum::ROW_LAYOUT_ORIENTATION:
                surfaceMontageViewports[0].setRowAndColumn(0, 0);
                surfaceMontageViewports[2].setRowAndColumn(0, 1);
                surfaceMontageViewports[4].setRowAndColumn(0, 2);
                surfaceMontageViewports[6].setRowAndColumn(0, 3);
                surfaceMontageViewports[1].setRowAndColumn(0, 4);
                surfaceMontageViewports[3].setRowAndColumn(0, 5);
                surfaceMontageViewports[5].setRowAndColumn(0, 6);
                surfaceMontageViewports[7].setRowAndColumn(0, 7);
                break;
        }
    }
    else if (totalNum > 0) {
        CaretAssert(0);
    }
    
//    const int32_t numViewports = static_cast<int32_t>(surfaceMontageViewports.size());
//    CaretAssert(totalNum == numViewports);
//    
//    std::cout << "Orientation: " << SurfaceMontageLayoutOrientationEnum::toName(getLayoutOrientation()) << std::endl;
//    for (int32_t i = 0; i < numViewports; i++) {
//        const SurfaceMontageViewport& svp = surfaceMontageViewports[i];
//        std::cout << qPrintable("("
//                                + AString::number(svp.getRow())
//                                + ","
//                                + AString::number(svp.getColumn())
//                                + ") "
//                                + ProjectionViewTypeEnum::toName(svp.getProjectionViewType()))
//        << std::endl;
//    }
//    std::cout << std::endl;
}


/**
 * Save members to the given scene class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 * @param sceneClass
 *     sceneClass to which information is added.
 */
void
SurfaceMontageConfigurationCerebellar::saveMembersToScene(const SceneAttributes* sceneAttributes,
                                                   SceneClass* sceneClass)
{
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
}

/**
 * Restore information specific to the type of model from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass from which information is restored.
 */
void
SurfaceMontageConfigurationCerebellar::restoreMembersFromScene(const SceneAttributes* sceneAttributes,
                                     const SceneClass* sceneClass)
{
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
SurfaceMontageConfigurationCerebellar::toString() const
{
    PlainTextStringBuilder tb;
    getDescriptionOfContent(tb);
    return tb.getText();
}

/**
 * Get a text description of the instance's content.
 *
 * @param descriptionOut
 *    Description of the instance's content.
 */
void
SurfaceMontageConfigurationCerebellar::getDescriptionOfContent(PlainTextStringBuilder& descriptionOut) const
{
    descriptionOut.addLine("Cerebellar Montage: ");
    
    descriptionOut.pushIndentation();
    
    const Surface* firstSurface = getFirstSurfaceSelectionModel()->getSurface();
    if (firstSurface != NULL) {
        if (isFirstSurfaceEnabled()) {
            descriptionOut.addLine("Surface:");
            descriptionOut.pushIndentation();
            firstSurface->getDescriptionOfContent(descriptionOut);
            descriptionOut.popIndentation();
        }
    }
    
    const Surface* secondSurface = getSecondSurfaceSelectionModel()->getSurface();
    if (secondSurface != NULL) {
        if (isSecondSurfaceEnabled()) {
            descriptionOut.addLine("Surface:");
            descriptionOut.pushIndentation();
            secondSurface->getDescriptionOfContent(descriptionOut);
            descriptionOut.popIndentation();
        }
    }
    
    AString viewsMsg = "Selected Views: ";
    
    if (isAnteriorEnabled()) {
        viewsMsg += " Anterior";
    }
    if (isDorsalEnabled()) {
        viewsMsg += " Dorsal";
    }
    if (isPosteriorEnabled()) {
        viewsMsg += " Posterior";
    }
    if (isVentralEnabled()) {
        viewsMsg += " Ventral";
    }
    
    descriptionOut.addLine(viewsMsg);
    
    descriptionOut.popIndentation();
}

/**
 * Get all surfaces displayed in this configuration.
 *
 * @param surfaceOut
 *    Will contain all displayed surfaces upon exit.
 */
void
SurfaceMontageConfigurationCerebellar::getDisplayedSurfaces(std::vector<Surface*>& surfacesOut) const
{
    surfacesOut.clear();
    
    const Surface* firstSurface = getFirstSurfaceSelectionModel()->getSurface();
    if (firstSurface != NULL) {
        if (isFirstSurfaceEnabled()) {
            surfacesOut.push_back(const_cast<Surface*>(firstSurface));
        }
    }
    
    const Surface* secondSurface = getSecondSurfaceSelectionModel()->getSurface();
    if (secondSurface != NULL) {
        if (isSecondSurfaceEnabled()) {
            if (secondSurface != firstSurface) {
                surfacesOut.push_back(const_cast<Surface*>(secondSurface));
            }
        }
    }
}
/**
 * Copy the given configuration to this configurtion.
 *
 * @param configuration.
 *    Configuration that is copied.
 */
void
SurfaceMontageConfigurationCerebellar::copyConfiguration(SurfaceMontageConfigurationAbstract* configuration)
{
    SurfaceMontageConfigurationCerebellar* cerebellarConfiguration = dynamic_cast<SurfaceMontageConfigurationCerebellar*>(configuration);
    CaretAssert(cerebellarConfiguration);
    
    SurfaceMontageConfigurationAbstract::copyConfiguration(configuration);
    
    m_firstSurfaceSelectionModel->setSurface(cerebellarConfiguration->m_firstSurfaceSelectionModel->getSurface());
    m_secondSurfaceSelectionModel->setSurface(cerebellarConfiguration->m_secondSurfaceSelectionModel->getSurface());
    
    m_firstSurfaceEnabled = cerebellarConfiguration->m_firstSurfaceEnabled;
    m_secondSurfaceEnabled = cerebellarConfiguration->m_secondSurfaceEnabled;
    
    m_dorsalEnabled = cerebellarConfiguration->m_dorsalEnabled;
    m_ventralEnabled = cerebellarConfiguration->m_ventralEnabled;
    m_posteriorEnabled = cerebellarConfiguration->m_posteriorEnabled;
    m_anteriorEnabled = cerebellarConfiguration->m_anteriorEnabled;
}


