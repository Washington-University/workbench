
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

#define __SELECTION_ITEM_NEUROGLANCER_ANNOTATION_DECLARE__
#include "SelectionItemNeuroglancerAnnotation.h"
#undef __SELECTION_ITEM_NEUROGLANCER_ANNOTATION_DECLARE__

#include "HistologySlicesFile.h"
#include "NeuroglancerAnnotation.h"
#include "NeuroglancerAnnotationsFile.h"
#include "Surface.h"
#include "VolumeFile.h"

using namespace caret;


    
/**
 * \class SelectionItemNeuroglancerAnnotation
 * \brief Contains information about the selected neuroglancer annontation.
 */
/**
 * Constructor.
 */
SelectionItemNeuroglancerAnnotation::SelectionItemNeuroglancerAnnotation()
: SelectionItem(SelectionItemDataTypeEnum::FOCUS_VOLUME)
{
    resetPrivate();
}

/**
 * Destructor.
 */
SelectionItemNeuroglancerAnnotation::~SelectionItemNeuroglancerAnnotation()
{
}

/**
 * Reset this selection item. 
 */
void 
SelectionItemNeuroglancerAnnotation::reset()
{
    SelectionItem::reset();
    resetPrivate();
}

/**
 * Reset this selection item.
 */
void
SelectionItemNeuroglancerAnnotation::resetPrivate()
{
    SelectionItem::reset();
    m_idType     = IdType::INVALID;
    m_surface    = NULL;
    m_volumeFile = NULL;
    m_histologySlicesFile = NULL;
    m_neuroAnn      = NULL;
    m_neuroAnnFile   = NULL;
    m_neuroAnnIndex = -1;
}

/**
 * @return Is this selected item valid?
 */
bool 
SelectionItemNeuroglancerAnnotation::isValid() const
{
    return (m_idType != IdType::INVALID);
}

/**
 * @return Type of neuroglancer annontation identification
 */
SelectionItemNeuroglancerAnnotation::IdType
SelectionItemNeuroglancerAnnotation::getIdType() const
{
    return m_idType;
}

/**
 * Set surface identification of a neuroglancer annontation
 * @param surface
 *    Surface on which neuroglancer annontation was identified
 * @param neuroAnnFile
 *    Neuroglancer annotation file containing annotation
 * @param neuroAnn
 *    The neuroglancer annontation
 * @param neuroAnnIndex
 *    Index of neuroglancer annontation in neuroglancer annontation file
 */
void
SelectionItemNeuroglancerAnnotation::setSurfaceSelection(const Surface* surface,
                                                         NeuroglancerAnnotationsFile* neuroAnnFile,
                                        NeuroglancerAnnotation* neuroAnn,
                                        const int32_t neuroAnnIndex)
{
    CaretAssert(surface);
    CaretAssert(neuroAnnFile);
    CaretAssert(neuroAnn);
    
    m_idType               = IdType::SURFACE;
    m_surface              = surface;
    m_neuroAnnFile             = neuroAnnFile;
    m_neuroAnn                = neuroAnn;
    m_neuroAnnIndex           = neuroAnnIndex;
}

/**
 * Set whole brain identification of a neuroglancer annontation
 * @param neuroAnnFile
 *    Neuroglancer annotation file containing annotation
 * @param neuroAnn
 *    The neuroglancer annontation
 * @param neuroAnnIndex
 *    Index of neuroglancer annontation in neuroglancer annontation file
 */
void
SelectionItemNeuroglancerAnnotation::setWholeBrainSelection(NeuroglancerAnnotationsFile* neuroAnnFile,
                                                            NeuroglancerAnnotation* neuroAnn,
                                                            const int32_t neuroAnnIndex)
{
    CaretAssert(neuroAnnFile);
    CaretAssert(neuroAnn);
    
    m_idType        = IdType::WHOLE_BRAIN;
    m_neuroAnnFile  = neuroAnnFile;
    m_neuroAnn      = neuroAnn;
    m_neuroAnnIndex = neuroAnnIndex;
}



/**
 * Set histology identification of a neuroglancer annontation
 * @param histologySlicesFile
 *    Histology file on which neuroAnn was identified
 * @param neuroAnnFile
 *    Neuroglancer annotation file containing annotation
 * @param neuroAnn
 *    The neuroglancer annontation
 * @param neuroAnnIndex
 *    Index of neuroglancer annontation in neuroglancer annontation file
 */
void
SelectionItemNeuroglancerAnnotation::setHistologySelection(HistologySlicesFile* histologySlicesFile,
                                                           NeuroglancerAnnotationsFile* neuroAnnFile,
                                          NeuroglancerAnnotation* neuroAnn,
                                          const int32_t neuroAnnIndex)
{
    CaretAssert(histologySlicesFile);
    CaretAssert(neuroAnnFile);
    CaretAssert(neuroAnn);
    
    m_idType               = IdType::HISTOLOGY;
    m_histologySlicesFile  = histologySlicesFile;
    m_neuroAnnFile             = neuroAnnFile;
    m_neuroAnn                = neuroAnn;
    m_neuroAnnIndex           = neuroAnnIndex;
}

/**
 * Set surface identification of a neuroAnn
 * @param volumeMappableInterface
 *    volume on which neuroglancer annontation was identified
 * @param neuroAnnFile
 *    Neuroglancer annotation file containing annotation
 * @param neuroAnn
 *    The neuroglancer annontation
 * @param neuroAnnIndex
 *    Index of neuroglancer annontation in neuroglancer annontation file
 */
void
SelectionItemNeuroglancerAnnotation::setVolumeSelection(VolumeMappableInterface* volumeMappableInterface,
                                                        NeuroglancerAnnotationsFile* neuroAnnFile,
                                       NeuroglancerAnnotation* neuroAnn,
                                       const int32_t neuroAnnIndex)
{
    CaretAssert(volumeMappableInterface);
    CaretAssert(neuroAnnFile);
    CaretAssert(neuroAnn);
    
    m_idType               = IdType::VOLUME;
    m_volumeFile           = volumeMappableInterface;
    m_neuroAnnFile             = neuroAnnFile;
    m_neuroAnn                = neuroAnn;
    m_neuroAnnIndex           = neuroAnnIndex;
}

/**
 * @return Surface on which neuroglancer annontation was drawn.
 */
const Surface*
SelectionItemNeuroglancerAnnotation::getSurface() const
{
    return m_surface;
}

/**
 * @return Surface on which neuroglancer annontation was drawn.
 */
Surface*
SelectionItemNeuroglancerAnnotation::getSurface()
{
    return const_cast<Surface*>(m_surface);
}

/**
 * @return VolumeFile on which neuroglancer annontation was drawn.
 */
const VolumeMappableInterface*
SelectionItemNeuroglancerAnnotation::getVolumeFile() const
{
    return m_volumeFile;
}

/**
 * @return VolumeFile on which neuroglancer annontation was drawn.
 */
VolumeMappableInterface*
SelectionItemNeuroglancerAnnotation::getVolumeFile()
{
    return m_volumeFile;
}

/**
 * @return The histology slices file
 */
HistologySlicesFile*
SelectionItemNeuroglancerAnnotation::getHistologySlicesFile()
{
    return m_histologySlicesFile;
}

/**
 * @return The histology slices file
 */
const HistologySlicesFile*
SelectionItemNeuroglancerAnnotation::getHistologySlicesFile() const
{
    return m_histologySlicesFile;
}

/**
 * @return The neuroglancer annontation that was selected.
 */
const NeuroglancerAnnotation* 
SelectionItemNeuroglancerAnnotation::getNeuroglancerAnnotation() const
{
    return m_neuroAnn;
}

/**
 * @return The neuroglancer annontation that was selected.
 */
NeuroglancerAnnotation* 
SelectionItemNeuroglancerAnnotation::getNeuroglancerAnnotation()
{
    return m_neuroAnn;
}

/**
 * @return The neuroglancer annotation file containing neuroglancer annontation that was selected.
 */
const NeuroglancerAnnotationsFile*
SelectionItemNeuroglancerAnnotation::getNeuroglancerAnnotationsFile() const
{
    return m_neuroAnnFile;
}

/**
 * @return The neuroglancer annotation file containing neuroglancer annontation that was selected.
 */
NeuroglancerAnnotationsFile*
SelectionItemNeuroglancerAnnotation::getNeuroglancerAnnotationsFile()
{
    return m_neuroAnnFile;
}

/**
 * return Index of selected neuroglancer annontation.
 */
int32_t 
SelectionItemNeuroglancerAnnotation::getNeuroglancerAnnotationIndex() const
{
    return m_neuroAnnIndex;
}

/**
 * Get a description of m_ object's content.
 * @return String describing m_ object's content.
 */
AString
SelectionItemNeuroglancerAnnotation::toString() const
{
    AString name = "INVALID";
    if (m_volumeFile != NULL) {
        CaretMappableDataFile* cmdf = dynamic_cast<CaretMappableDataFile*>(m_volumeFile);
        if (cmdf != NULL) {
            name = cmdf->getFileNameNoPath();
        }
    }
    
    AString text = SelectionItem::toString();
    text += ("Histology: " + ((m_histologySlicesFile != NULL) ? m_histologySlicesFile->getFileNameNoPath() : "INVALID") + "\n");
    text += ("Surface: " + ((m_surface != NULL) ? m_surface->getFileNameNoPath() : "INVALID") + "\n");
    text += ("Volume File: " + name + "\n");
    text += ("Neuroglancer File: " + ((m_neuroAnnFile != NULL) ? m_neuroAnnFile->getFileNameNoPath() : "INVALID") + "\n");
    text += ("NeuroglancerAnnotation: " + ((m_neuroAnn != NULL) ? m_neuroAnn->getFileName() : "INVALID") + "\n");
    text += ("NeuroglancerAnnotation Index: " + AString::number(m_neuroAnnIndex) + "\n");
    return text;
}
