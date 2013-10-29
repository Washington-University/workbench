
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

#define __VOLUME_MAPPABLE_INTERFACE_DECLARE__
#include "VolumeMappableInterface.h"
#undef __VOLUME_MAPPABLE_INTERFACE_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * Get the voxel spacing for each of the spatial dimensions.
 *
 * @param spacingOut1
 *    Spacing for the first dimension (typically X).
 * @param spacingOut2
 *    Spacing for the first dimension (typically Y).
 * @param spacingOut3
 *    Spacing for the first dimension (typically Z).
 */
void
VolumeMappableInterface::getVoxelSpacing(float& spacingOut1,
                                         float& spacingOut2,
                                         float& spacingOut3) const
{
    float originX, originY, originZ;
    float x1, y1, z1;
    indexToSpace(0, 0, 0, originX, originY, originZ);
    indexToSpace(1, 1, 1, x1, y1, z1);
    spacingOut1 = x1 - originX;
    spacingOut2 = y1 - originY;
    spacingOut3 = z1 - originZ;
}

