
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

#define __VIEWING_TRANSFORMATIONS_CEREBELLUM_DECLARE__
#include "ViewingTransformationsCerebellum.h"
#undef __VIEWING_TRANSFORMATIONS_CEREBELLUM_DECLARE__

#include "CaretAssert.h"
using namespace caret;

/**
 * \class caret::ViewingTransformationsCerebellum 
 * \brief Viewing transformations (pan/rotate/zoom) for cerebellum.
 * \ingroup Brain
 *
 * Extends ViewingTransformations with differences for cerebellum viewing.
 */

/**
 * Constructor.
 */
ViewingTransformationsCerebellum::ViewingTransformationsCerebellum()
: ViewingTransformations()
{
    
}

/**
 * Destructor.
 */
ViewingTransformationsCerebellum::~ViewingTransformationsCerebellum()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
ViewingTransformationsCerebellum::ViewingTransformationsCerebellum(const ViewingTransformationsCerebellum& obj)
: ViewingTransformations(obj)
{
    this->copyHelperViewingTransformationsCerebellum(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
ViewingTransformationsCerebellum&
ViewingTransformationsCerebellum::operator=(const ViewingTransformationsCerebellum& obj)
{
    if (this != &obj) {
        ViewingTransformations::operator=(obj);
        this->copyHelperViewingTransformationsCerebellum(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
ViewingTransformationsCerebellum::copyHelperViewingTransformationsCerebellum(const ViewingTransformationsCerebellum& /*obj*/)
{
    /* nothing to copy */
}

/**
 * Reset the view to the default view.
 */
void
ViewingTransformationsCerebellum::resetView()
{
    ViewingTransformations::resetView();
    dorsalView();
}

/**
 * set to a dorsal view.
 */
void
ViewingTransformationsCerebellum::dorsalView()
{
    m_rotationMatrix->setRotation(30.0, 0.0, 0.0);
}

/**
 * set to a ventral view.
 */
void
ViewingTransformationsCerebellum::ventralView()
{
    m_rotationMatrix->setRotation(0.0, 180.0, 180.0);
}

