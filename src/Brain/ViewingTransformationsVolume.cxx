
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

#define __VIEWING_TRANSFORMATIONS_VOLUME_DECLARE__
#include "ViewingTransformationsVolume.h"
#undef __VIEWING_TRANSFORMATIONS_VOLUME_DECLARE__

#include "BrowserTabContent.h"
#include "BrainOpenGLViewportContent.h"
#include "BrainOpenGLVolumeSliceDrawing.h"
#include "CaretAssert.h"
#include "GraphicsRegionSelectionBox.h"
#include "MouseEvent.h"
#include "OverlaySet.h"
#include "ViewingTransformationToFitRegion.h"
#include "VolumeMappableInterface.h"

using namespace caret;

/**
 * \class caret::ViewingTransformationsVolume
 * \brief Viewing transformations (pan/rotate/zoom) for volume.
 * \ingroup Brain
 *
 * Extends ViewingTransformations with differences for volume viewing.
 */

/**
 * Constructor.
 */
ViewingTransformationsVolume::ViewingTransformationsVolume()
: ViewingTransformations()
{
    
}

/**
 * Destructor.
 */
ViewingTransformationsVolume::~ViewingTransformationsVolume()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
ViewingTransformationsVolume::ViewingTransformationsVolume(const ViewingTransformationsVolume& obj)
: ViewingTransformations(obj)
{
    this->copyHelperViewingTransformationsVolume(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
ViewingTransformationsVolume&
ViewingTransformationsVolume::operator=(const ViewingTransformationsVolume& obj)
{
    if (this != &obj) {
        ViewingTransformations::operator=(obj);
        this->copyHelperViewingTransformationsVolume(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
ViewingTransformationsVolume::copyHelperViewingTransformationsVolume(const ViewingTransformationsVolume& /*obj*/)
{
    
}

/**
 * Reset the view to the default view for a VOLUME.
 */
void
ViewingTransformationsVolume::resetView()
{
    ViewingTransformations::resetView();
    m_rotationMatrix->identity();
}

/**
 * Set the view of the  volume to the given bounds
 * @param viewportContent
 *    The viewport content
 * @param sliceViewPlaneSelectedInTab
 *    The slice plane selected in the tab. This may be ALL, AXIAL, CORONAL, or PARASAGITTAL
 * @param sliceViewPlaneForFitToRegion
 *    The slice plane that is fit to plane (perhaps the slice plane containing mouse).  This may be
 * AXIAL, CORONAL, or PARASAGITTAL only.  It is NEVER ALL !!!   When ALL is selected for
 * the tab, this is the particular slice plane in the ALL view.
 * @param selectionRegion
 *    The selection bounds
 * @param browserTabContent
 *    The content of the browser tab
 */
bool
ViewingTransformationsVolume::setViewToBounds(const BrainOpenGLViewportContent* viewportContent,
                                              const VolumeSliceViewPlaneEnum::Enum sliceViewPlaneSelectedInTab,
                                              const VolumeSliceViewPlaneEnum::Enum sliceViewPlaneForFitToRegion,
                                              const GraphicsRegionSelectionBox* selectionRegion,
                                              const BrowserTabContent* browserTabContent)

{
    const bool testFlag(false);
    if (testFlag) {
        const float x1=3.9, x2=74.6, y1=-17.5, y2=83.3, z1=0.0999985, z2=0.0999985;
        const float vpX1=520, vpX2=638, vpY1=352, vpY2=184;
        GraphicsRegionSelectionBox box;
        box.initialize(x1, y1, z1, vpX1, vpY1);
        box.update(x2, y2, z2, vpX2, vpY2);
        
        Vector3D translation;
        float zoom(0.0);
        ViewingTransformationToFitRegion transformFitToRegion(viewportContent,
                                                              &box,
                                                              browserTabContent);
        switch (browserTabContent->getVolumeSliceProjectionType()) {
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR:
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR_THREE:
                if ( ! transformFitToRegion.applyToMprVolume(m_translation,
                                                             sliceViewPlaneSelectedInTab,
                                                             sliceViewPlaneForFitToRegion,
                                                             translation,
                                                             zoom)) {
                    return false;
                }
                break;
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
                CaretAssert(0);
                return false;
                break;
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
                if ( ! transformFitToRegion.applyToOrthogonalVolume(m_translation,
                                                                    sliceViewPlaneSelectedInTab,
                                                                    sliceViewPlaneForFitToRegion,
                                                                    translation,
                                                                    zoom)) {
                    return false;
                }
                break;
        }

        
        resetView();
        setTranslation(translation);
        setScaling(zoom);
        
        return true;
    }
    
    
    Vector3D translation;
    float zoom(0.0);
    ViewingTransformationToFitRegion transformFitToRegion(viewportContent,
                                                          selectionRegion,
                                                          browserTabContent);
    
    switch (browserTabContent->getVolumeSliceProjectionType()) {
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR:
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR_THREE:
            if ( ! transformFitToRegion.applyToMprVolume(m_translation,
                                                         sliceViewPlaneSelectedInTab,
                                                         sliceViewPlaneForFitToRegion,
                                                         translation,
                                                         zoom)) {
                return false;
            }
            break;
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
            if ( ! transformFitToRegion.applyToObliqueVolume(m_translation,
                                                             sliceViewPlaneSelectedInTab,
                                                             sliceViewPlaneForFitToRegion,
                                                             translation,
                                                             zoom)) {
                return false;
            }
            break;
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
            if ( ! transformFitToRegion.applyToOrthogonalVolume(m_translation,
                                                                sliceViewPlaneSelectedInTab,
                                                                sliceViewPlaneForFitToRegion,
                                                                translation,
                                                                zoom)) {
                return false;
            }
            break;
    }
    
    resetView();
    setTranslation(translation);
    setScaling(zoom);
    return true;
}

