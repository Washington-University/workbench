
/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
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

#define __VIEWING_TRANSFORMATIONS_VOLUME_DECLARE__
#include "ViewingTransformationsVolume.h"
#undef __VIEWING_TRANSFORMATIONS_VOLUME_DECLARE__

#include "CaretAssert.h"
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

