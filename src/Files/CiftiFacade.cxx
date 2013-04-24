
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

#define __CIFTI_FACADE_DECLARE__
#include "CiftiFacade.h"
#undef __CIFTI_FACADE_DECLARE__

using namespace caret;


    
/**
 * \class caret::CiftiFacade 
 * \brief A simpler interface (facade pattern) for CIFTI Interface access.
 * \ingroup Files
 */

/**
 * Constructor.
 *
 * @param ciftiInterface
 *    The CIFTI Interface.
 */
CiftiFacade::CiftiFacade(CiftiInterface* ciftiInterface)
: CaretObject(),
m_ciftiInterface(ciftiInterface)
{
    
}

/**
 * Destructor.
 */
CiftiFacade::~CiftiFacade()
{
    
}

/**
 * Get the mapping for the surface with the given structure for mapping
 * to brainordinates such as surface coloring or obtaining data values.
 *
 * @param mappingOut
 *    Output surface mapping.
 * @param structure
 *    Structure for the surface.
 * @return
 *    True if mapping is valid, else false.
 */
bool
CiftiFacade::getSurfaceMapForMappingDataToBrainordinates(std::vector<CiftiSurfaceMap>& mappingOut,
                                                 const StructureEnum::Enum structure) const
{
    return false;
}

/** 
 * Get the mapping for voxels  for mapping to brainordinates such as 
 * voxel coloring or obtaining data values.
 *
 * @param mappingOut
 *    Output surface mapping.
 * @param structure
 *    Structure for the surface.
 * @return
 *    True if mapping is valid, else false.
 */
bool
CiftiFacade::getVolumeMapForMappingDataToBrainordinates(std::vector<CiftiVolumeMap>& mappingOut) const
{
    
    return false;
}

/**
 * Get the metadata for the given map or series index.
 *
 * @param mapIndex
 *    Index of map or series.
 * @param metadataOut
 *    Is cleared and metadata for the map in placed into this.
 * @return
 *    True if the map index is valid (even if there is no metadata).
 */
bool
CiftiFacade::getMetadataForMapOrSeriesIndex(const int32_t mapIndex,
                                    GiftiMetaData* metadataOut)
{
    return false;
}

/**
 * Set the metadata for the given map or series index.
 *
 * @param mapIndex
 *    Index of map or series.
 * @param metadataOut
 *    Metadata that is placed using the given map or series index.
 */
void
CiftiFacade::setMetadataForMapOrSeriesIndex(const int32_t mapIndex,
                                    GiftiMetaData* metadataIn)
{
    
}

/**
 * Get the label table for the given map or series index.
 * 
 * @param mapIndex
 *    Index of map or series.
 * @return
 *    Pointer to label table or NULL if label table not valid for 
 *    the given map or series index.
 */
GiftiLabelTable*
CiftiFacade::getLabelTableForMapOrSeriesIndex(const int32_t mapIndex)
{
    return NULL;
}

/**
 * Get the palette color mapping for the given map or series index.
 *
 * @param mapIndex
 *    Index of map or series.
 * @return
 *    Pointer to palette color mapping or NULL if palette color mapping
 *    not valid for the given map or series index.
 */
PaletteColorMapping*
CiftiFacade::getPaletteColorMappingForMapOrSeriesIndex(const int32_t mapIndex)
{
    return NULL;
    
}

/**
 * Get the name for the given map or series index.
 *
 * @param mapIndex
 *    Index of map or series.
 * @return
 *    Name for given map or series index.  
 */
AString
CiftiFacade::getNameForMapOrSeriesIndex(const int32_t mapIndex) const
{
    return "";
}

/**
 * Set the name for the given map or series index.
 *
 * @param mapIndex
 *    Index of map or series.
 * @param name
 *    Name for the given map or series index.
 */
void
CiftiFacade::setNameForMapOrSeriesIndex(const int32_t mapIndex,
                                const AString name)
{
    
}

/**
 * Get the data for the given map or series index.
 *
 * @param mapIndex
 *    Index of map or series.
 * @param dataOut
 *    Data for given map or series is loaded into this.
 * @return
 *    True if output data is valid, else false.
 */
bool
CiftiFacade::getDataForMapOrSeriesIndex(const int32_t mapIndex,
                                std::vector<float>& dataOut) const
{
    return false;
}


