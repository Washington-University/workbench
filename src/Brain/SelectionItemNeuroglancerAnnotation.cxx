
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
#include "NeuroglancerAnnotationFile.h"
#include "Surface.h"
#include "VolumeFile.h"

using namespace caret;


    
/**
 * \class SelectionItemNeuroglancerAnnotation
 * \brief Contains information about the selected neuroglancer annotation.
 */
/**
 * Constructor.
 */
SelectionItemNeuroglancerAnnotation::SelectionItemNeuroglancerAnnotation()
: SelectionItem(SelectionItemDataTypeEnum::NEUROGLANCER_ANNOTATION)
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
    m_histologySlicesFile        = NULL;
    m_neuroglancerAnnotation     = NULL;
    m_neuroglancerAnnotationFile = NULL;
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
 * @return Type of neuroglancer identification
 */
SelectionItemNeuroglancerAnnotation::IdType
SelectionItemNeuroglancerAnnotation::getIdType() const
{
    return m_idType;
}

/**
 * Set surface identification of a neuroglancer annotation
 * @param surface
 *    Surface on which neuroglancer annotation was identified
 * @param neuroglancerAnnotationFile
 *    neuroglancer annotation file containing neuroglancer annotation
 * @param neuroglancerAnnotation
 *    The neuroglancer annotation
 */
void
SelectionItemNeuroglancerAnnotation::setSurfaceSelection(const Surface* surface,
                                                         NeuroglancerAnnotationFile* neuroglancerAnnotationFile,
                                                         const NeuroglancerAnnotation* neuroglancerAnnotation)
{
    CaretAssert(surface);
    CaretAssert(neuroglancerAnnotationFile);
    CaretAssert(neuroglancerAnnotation);

    m_idType                     = IdType::SURFACE;
    m_surface                    = surface;
    m_neuroglancerAnnotationFile = neuroglancerAnnotationFile;
    m_neuroglancerAnnotation     = neuroglancerAnnotation;
}

/**
 * Set whole brain identification of a neuroglancer annotation
 * @param neuroglancerAnnotationFile
 *    neuroglancer annotation file containing neuroglancer annotation
 * @param neuroglancerAnnotation
 *    The neuroglancer annotation
 */
void
SelectionItemNeuroglancerAnnotation::setWholeBrainSelection(NeuroglancerAnnotationFile* neuroglancerAnnotationFile,
                                                            const NeuroglancerAnnotation* neuroglancerAnnotation)
{
    CaretAssert(neuroglancerAnnotationFile);
    CaretAssert(neuroglancerAnnotation);
    
    m_idType        = IdType::WHOLE_BRAIN;
    m_neuroglancerAnnotationFile = neuroglancerAnnotationFile;
    m_neuroglancerAnnotation     = neuroglancerAnnotation;
}



/**
 * Set histology identification of a neuroglancer annotation
 * @param histologySlicesFile
 *    Histology file on which neuroglancer annotation was identified
 * @param neuroglancerAnnotationFile
 *    neuroglancer annotation file containing neuroglancer annotation
 * @param neuroglancerAnnotation
 *    The neuroglancer annotation
 */
void
SelectionItemNeuroglancerAnnotation::setHistologySelection(HistologySlicesFile* histologySlicesFile,
                                                           NeuroglancerAnnotationFile* neuroglancerAnnotationFile,
                                                           const NeuroglancerAnnotation* neuroglancerAnnotation)
{
    CaretAssert(histologySlicesFile);
    CaretAssert(neuroglancerAnnotationFile);
    CaretAssert(neuroglancerAnnotation);

    m_idType               = IdType::HISTOLOGY;
    m_histologySlicesFile  = histologySlicesFile;
    m_neuroglancerAnnotationFile = neuroglancerAnnotationFile;
    m_neuroglancerAnnotation     = neuroglancerAnnotation;
}

/**
 * Set  identification of a neuroglancer annotation
 * @param neuroglancerAnnotationFile
 *    neuroglancer annotation file containing neuroglancer annotation
 * @param neuroglancerAnnotation
 *    The neuroglancer annotation
 */
void
SelectionItemNeuroglancerAnnotation::setIdentifySelection(NeuroglancerAnnotationFile* neuroglancerAnnotationFile,
                                                          const NeuroglancerAnnotation* neuroglancerAnnotation)
{
    CaretAssert(neuroglancerAnnotationFile);
    CaretAssert(neuroglancerAnnotation);
    
    m_idType               = IdType::IDENTIFY_BRAINORDINATE_DIALOG;
    m_neuroglancerAnnotationFile = neuroglancerAnnotationFile;
    m_neuroglancerAnnotation     = neuroglancerAnnotation;
}

/**
 * Set surface identification of a neuroglancer annotation
 * @param volumeMappableInterface
 *    volume on which neuroglancer annotation was identified
 * @param neuroglancerAnnotationFile
 *    neuroglancer annotation file containing neuroglancer annotation
 * @param neuroglancerAnnotation
 *    The neuroglancer annotation
 */
void
SelectionItemNeuroglancerAnnotation::setVolumeSelection(VolumeMappableInterface* volumeMappableInterface,
                                                        NeuroglancerAnnotationFile* neuroglancerAnnotationFile,
                                                        const NeuroglancerAnnotation* neuroglancerAnnotation)
{
    CaretAssert(volumeMappableInterface);
    CaretAssert(neuroglancerAnnotationFile);
    CaretAssert(neuroglancerAnnotation);

    m_idType               = IdType::VOLUME;
    m_volumeFile           = volumeMappableInterface;
    m_neuroglancerAnnotationFile = neuroglancerAnnotationFile;
    m_neuroglancerAnnotation     = neuroglancerAnnotation;
}

/**
 * @return Surface on which neuroglancer annotation was drawn.
 */
const Surface*
SelectionItemNeuroglancerAnnotation::getSurface() const
{
    return m_surface;
}

/**
 * @return Surface on which neuroglancer annotation was drawn.
 */
Surface*
SelectionItemNeuroglancerAnnotation::getSurface()
{
    return const_cast<Surface*>(m_surface);
}

/**
 * @return VolumeFile on which neuroglancer annotation was drawn.
 */
const VolumeMappableInterface*
SelectionItemNeuroglancerAnnotation::getVolumeFile() const
{
    return m_volumeFile;
}

/**
 * @return VolumeFile on which neuroglancer annotation was drawn.
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
 * @return The neuroglancer annotation that was selected.
 */
const NeuroglancerAnnotation*
SelectionItemNeuroglancerAnnotation::getNeuroglancerAnnotation() const
{
    return m_neuroglancerAnnotation;
}

/**
 * @return The neuroglancer annotation file containing neuroglancer annotation that was selected.
 */
const NeuroglancerAnnotationFile*
SelectionItemNeuroglancerAnnotation::getNeuroglancerAnnotationFile() const
{
    return m_neuroglancerAnnotationFile;
}

/**
 * @return The neuroglancer annotation file containing neuroglancer annotation that was selected.
 */
NeuroglancerAnnotationFile*
SelectionItemNeuroglancerAnnotation::getNeuroglancerAnnotationFile()
{
    return m_neuroglancerAnnotationFile;
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
    text += ("Neuroglancer Annotation File: " + ((m_neuroglancerAnnotationFile != NULL) ? m_neuroglancerAnnotationFile->getFileNameNoPath() : "INVALID") + "\n");
    text += ("Neuroglancer Annotation: " + ((m_neuroglancerAnnotation != NULL) ? m_neuroglancerAnnotation->getUniqueIdAsString() : "INVALID") + "\n");
    return text;
}
