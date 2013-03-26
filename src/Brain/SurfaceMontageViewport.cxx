
/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
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

#define __SURFACE_MONTAGE_VIEWPORT_DECLARE__
#include "SurfaceMontageViewport.h"
#undef __SURFACE_MONTAGE_VIEWPORT_DECLARE__

#include "CaretAssert.h"
#include "Surface.h"
#include "StructureEnum.h"

using namespace caret;


    
/**
 * \class caret::SurfaceMontageViewport 
 * \brief Viewports in a surface montage
 */

///**
// * Constructor for invalid viewport.
// */
//SurfaceMontageViewport::SurfaceMontageViewport()
//: CaretObject()
//{
//    CaretAssert(surface);
//    
//    this->surface = NULL;
//    this->structure = StructureEnum::INVALID;
//    this->viewingMatrixIndex = Model::VIEWING_TRANSFORM_NORMAL;
//    this->viewport[0] = -1;
//    this->viewport[1] = -1;
//    this->viewport[2] = -1;
//    this->viewport[3] = -1;
//}

/**
 * Constructor.
 *
 * @param surface
 *   Surface in the montage.
 * @param projectionViewType
 *   Projection view type.
 * @param x
 *   X-coordinate of viewport.
 * @param y
 *   T-coordinate of viewport.
 * @param w
 *   width of viewport.
 * @param h
 *   height of viewport.
 */
SurfaceMontageViewport::SurfaceMontageViewport(Surface* surface,
                                               const ProjectionViewTypeEnum::Enum projectionViewType,
                       const int x,
                       const int y,
                       const int w,
                       const int h)
: CaretObject()
{
    CaretAssert(surface);
    
    this->surface = surface;
    this->structure = surface->getStructure();
    this->projectionViewType = projectionViewType;
    this->viewport[0] = x;
    this->viewport[1] = y;
    this->viewport[2] = w;
    this->viewport[3] = h;
}


/**
 * Destructor.
 */
SurfaceMontageViewport::~SurfaceMontageViewport()
{
    
}

/**
 * @return true if the coordinates are inside the viewport, else false.
 * @param x
 *   X-coordinate
 * @param y
 *   Y-coordinate
 */
bool
SurfaceMontageViewport::isInside(const int32_t x,
                                 const int32_t y) const
{
    if (x < this->viewport[0]) return false;
    if (x > (this->viewport[0] + this->viewport[2])) return false;
    if (y < this->viewport[1]) return false;
    if (y > (this->viewport[1] + this->viewport[3])) return false;
    
    return true;
}

