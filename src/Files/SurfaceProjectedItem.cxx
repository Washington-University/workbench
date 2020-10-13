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

#define __SURFACE_PROJECTED_ITEM_DEFINE__
#include "SurfaceProjectedItem.h"
#undef __SURFACE_PROJECTED_ITEM_DEFINE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DataFileException.h"
#include "SurfaceProjectionBarycentric.h"
#include "SurfaceProjectionVanEssen.h"
#include "XmlWriter.h"

#include <QXmlStreamReader>

using namespace caret;

/**
 * \class caret::SurfaceProjectedItem
 * \brief Maintains position of an item projected to a surface.
 * 
 * Multiple projections are supported and may be valid at one time.
 * (1) Barycentric projects to a surface triangle.  (2) VanEssen
 * projects to an edget of a triangle.  (3) Stereotaxic is a
 * three-dimensional coordinate.   A volume coordinate is also
 * available.
 */

/**
 * Constructor.
 *
 */
SurfaceProjectedItem::SurfaceProjectedItem()
    : CaretObjectTracksModification()
{
    this->initializeMembersSurfaceProjectedItem();
}

/**
 * Destructor
 */
SurfaceProjectedItem::~SurfaceProjectedItem()
{
    delete this->barycentricProjection;
    delete this->vanEssenProjection;
}

/**
 * Copy Constructor
 * @param Object that is copied.
 */
SurfaceProjectedItem::SurfaceProjectedItem(const SurfaceProjectedItem& o)
    : CaretObjectTracksModification(o)
{
    this->initializeMembersSurfaceProjectedItem();
    this->copyHelper(o);
}

/**
 * Assignment operator.
 */
SurfaceProjectedItem&
SurfaceProjectedItem::operator=(const SurfaceProjectedItem& o)
{
    if (this != &o) {
        CaretObject::operator=(o);
        this->copyHelper(o);
    };
    
    return *this;
}

bool SurfaceProjectedItem::operator==(const SurfaceProjectedItem& rhs) const
{
    if (structure != rhs.structure) return false;
    if (stereotaxicXYZValid != rhs.stereotaxicXYZValid) return false;
    if (volumeXYZValid != rhs.volumeXYZValid) return false;
    if (stereotaxicXYZValid)
    {
        for (int i = 0; i < 3; ++i)
        {
            if (stereotaxicXYZ[i] != rhs.stereotaxicXYZ[i]) return false;
        }
    }
    if (volumeXYZValid)
    {
        for (int i = 0; i < 3; ++i)
        {
            if (volumeXYZ[i] != rhs.volumeXYZ[i]) return false;
        }
    }
    if (*barycentricProjection != *rhs.barycentricProjection) return false;
    return (*vanEssenProjection == *rhs.vanEssenProjection);
}

/**
 * Helps with copy constructor and assignment operator.
 */
void
SurfaceProjectedItem::copyHelper(const SurfaceProjectedItem& spi)
{
    this->setStereotaxicXYZ(spi.getStereotaxicXYZ());
    this->stereotaxicXYZValid = spi.stereotaxicXYZValid;
    
    this->setVolumeXYZ(spi.getVolumeXYZ());
    this->volumeXYZValid = spi.volumeXYZValid;
    
    this->structure = spi.structure;
    
    *this->barycentricProjection = *spi.barycentricProjection;
    *this->vanEssenProjection    = *spi.vanEssenProjection;
}

/**
 * Reset to default values as if no projection of any type.
 */
void 
SurfaceProjectedItem::reset()
{
    this->stereotaxicXYZ[0] = 0.0;
    this->stereotaxicXYZ[1] = 0.0;
    this->stereotaxicXYZ[2] = 0.0;
    this->stereotaxicXYZValid = false;
    this->volumeXYZ[0] = 0.0;
    this->volumeXYZ[1] = 0.0;
    this->volumeXYZ[2] = 0.0;
    this->volumeXYZValid = false;
    this->structure = StructureEnum::INVALID;
    this->barycentricProjection->reset();
    this->vanEssenProjection->reset();
}

void
SurfaceProjectedItem::initializeMembersSurfaceProjectedItem()
{
    this->barycentricProjection = new SurfaceProjectionBarycentric();
    this->vanEssenProjection    = new SurfaceProjectionVanEssen();
    this->reset();
}

/**
 * Unproject the item to the stereotaxic XYZ coordinates. 
 * 
 * @param sf - Surface on which unprojection takes place.
 * @param pasteOntoSurfaceFlag - place item directly on surface.
 *
 */
void
SurfaceProjectedItem::unprojectToStereotaxicXYZ(const SurfaceFile& sf,
                                             const bool isUnprojectedOntoSurface)
{
    float xyz[3];
    if (getProjectedPosition(sf, 
                             xyz, 
                             isUnprojectedOntoSurface)) {
        this->setStereotaxicXYZ(xyz);
    }
}

/**
 * Unproject the item to the volume XYZ coordinates.
 *
 * @param sf - Surface on which unprojection takes place.
 * @param pasteOntoSurfaceFlag - place item directly on surface.
 *
 */
void
SurfaceProjectedItem::unprojectToVolumeXYZ(const SurfaceFile& sf,
                                           const bool isUnprojectedOntoSurface)
{
    float xyz[3];
    if (getProjectedPosition(sf, 
                             xyz, 
                             isUnprojectedOntoSurface)) {
        this->setVolumeXYZ(xyz);
    }
}

/**
 * Get the projected position of this item.
 * The first valid of this positions is used: (1) Barycentric,
 * (2) VanEssen, (3) Stereotaxic.
 *
 * @param surfaceFile
 *     Surface File for positioning.
 * @param xyzOut
 *     Output containing the projected position.
 * @param pasteOntoSurfaceFlag
 *     Place directly on the surface.
 * @return  true if the position is valid, else false.
 *
 */
bool
SurfaceProjectedItem::getProjectedPosition(const SurfaceFile& surfaceFile,
                                           float xyzOut[3],
                                           const bool isUnprojectedOntoSurface) const
{
    return getProjectedPosition(&surfaceFile,
                                xyzOut,
                                isUnprojectedOntoSurface);
}

/**
 * Get the projected position of this item.
 * The first valid of this positions is used: (1) Barycentric,
 * (2) VanEssen, (3) Stereotaxic.
 * 
 * @param surfaceFile  
 *     Surface File for positioning.
 * @param xyzOut
 *     Output containing the projected position.
 * @param pasteOntoSurfaceFlag   
 *     Place directly on the surface.
 * @return  true if the position is valid, else false.
 *
 */
bool
SurfaceProjectedItem::getProjectedPosition(const SurfaceFile* surfaceFile,
                                           float xyzOut[3],
                                           const bool isUnprojectedOntoSurface) const
{
    bool valid = false;
    
    if (surfaceFile != NULL) {
        if ( ! valid) {
            if (this->barycentricProjection->isValid()) {
                valid = this->barycentricProjection->unprojectToSurface(*surfaceFile,
                                                                        xyzOut,
                                                                        0.0,
                                                                        isUnprojectedOntoSurface);
            }
        }
        
        if ( ! valid) {
            if (this->vanEssenProjection->isValid()) {
                valid = this->vanEssenProjection->unprojectToSurface(*surfaceFile,
                                                                     xyzOut,
                                                                     0.0,
                                                                     isUnprojectedOntoSurface);
            }
        }
    }
    
    if ( ! valid) {
        if (this->stereotaxicXYZValid) {
            this->getStereotaxicXYZ(xyzOut);
            valid = true;
        }
    }
    
    return valid;
}

/**
 * Get the projected position of this item.  The item is unprojected
 * to the surface and then it is placed above (below if negative)
 * the surface by the amount specified by 'distanceAboveSurface'.
 * 
 * The first valid of this positions is used: (1) Barycentric,
 * (2) VanEssen, (3) Stereotaxic.
 * 
 * @param surfaceFile  
 *     Surface File for positioning.
 * @param topologyHelper
 *     The topology helper.  This value
 * @param xyzOut
 *     Output containing the projected position.
 * @param distanceAboveSurface   
 *     Unproje
 * @return  true if the position is valid, else false.
 *
 */
bool 
SurfaceProjectedItem::getProjectedPositionAboveSurface(const SurfaceFile& surfaceFile,
                                                       const TopologyHelper* topologyHelper,
                                                       float xyzOut[3],
                                                       const float distanceAboveSurface) const
{
    bool valid = false;
    
    if (valid == false) {
        if (this->barycentricProjection->isValid()) {
            valid = this->barycentricProjection->unprojectToSurface(surfaceFile,
                                                                    topologyHelper,
                                                                    xyzOut, 
                                                                    distanceAboveSurface,
                                                                    true);
        }
    }
    
    if (valid == false) {
        if (this->vanEssenProjection->isValid()) {
            valid = this->vanEssenProjection->unprojectToSurface(surfaceFile,
                                                                 topologyHelper,
                                                                 xyzOut, 
                                                                 distanceAboveSurface,
                                                                 true);
        }
    }
    
    if (valid == false) {
        if (this->stereotaxicXYZValid) {
            this->getStereotaxicXYZ(xyzOut);
            valid = true;
        }
    }
    
    return valid;
}

/**
 * Get the stereotaxic position.
 * 
 * @return  Stereotaxic position.
 *
 */
const float*
SurfaceProjectedItem::getStereotaxicXYZ() const
{
    return this->stereotaxicXYZ;
}

/**
 * Get the Stereotaxic XYZ position.
 * @param stereotaxicXYZOut  Position placed into here.
 *
 */
void
SurfaceProjectedItem::getStereotaxicXYZ(float stereotaxicXYZOut[3]) const
{
    stereotaxicXYZOut[0] = this->stereotaxicXYZ[0];
    stereotaxicXYZOut[1] = this->stereotaxicXYZ[1];
    stereotaxicXYZOut[2] = this->stereotaxicXYZ[2];
}

/**
 * Get the validity of the Stereotaxic XYZ coordinate.
 * @return Validity of Stereotaxic XYZ coordinate.
 *
 */
bool
SurfaceProjectedItem::isStereotaxicXYZValid() const
{
    return this->stereotaxicXYZValid;
}

/**
 * Set the items stereotaxic coordinates and sets the validity
 * of the stereotaxic coordinates to true.
 * 
 * @param stereotaxicXYZ  New position.
 *
 */
void
SurfaceProjectedItem::setStereotaxicXYZ(const float stereotaxicXYZ[3])
{
    if (this->stereotaxicXYZValid) {
    }
    this->stereotaxicXYZ[0] = stereotaxicXYZ[0];
    this->stereotaxicXYZ[1] = stereotaxicXYZ[1];
    this->stereotaxicXYZ[2] = stereotaxicXYZ[2];
    this->stereotaxicXYZValid = true;

    if (this->volumeXYZValid == false) {
        this->setVolumeXYZ(stereotaxicXYZ);
    }
    
    this->setModified();
}

/**
 * Get the value of volumeXYZ
 *
 * @return the value of volumeXYZ
 *
 */
const float*
SurfaceProjectedItem::getVolumeXYZ() const
{
    /*
     * If not set, return stereotaxic coordinate
     */
    if ((volumeXYZ[0] == 0.0)
        && (volumeXYZ[1] == 0.0)
        && (volumeXYZ[2] == 0.0)) {
        const float* stereoXYZ = getStereotaxicXYZ();
        return stereoXYZ;
    }
    return this->volumeXYZ;
}

/**
 * Get the volume XYZ coordinates.
 * @param xyzOut  Volume XYZ coordinates.
 *
 */
void
SurfaceProjectedItem::getVolumeXYZ(float xyzOut[3]) const
{
    /*
     * If not set, return stereotaxic coordinate
     */
    if ((volumeXYZ[0] == 0.0)
        && (volumeXYZ[1] == 0.0)
        && (volumeXYZ[2] == 0.0)) {
        getStereotaxicXYZ(xyzOut);
        return;
    }
    
    xyzOut[0] = this->volumeXYZ[0];
    xyzOut[1] = this->volumeXYZ[1];
    xyzOut[2] = this->volumeXYZ[2];
}

/**
 * Get the validity of the volume XYZ coordinate.
 * @return Validity of volume XYZ coordinate.
 *
 */
bool
SurfaceProjectedItem::isVolumeXYZValid() const
{
    return this->volumeXYZValid;
}

/**
 * Set the item's volume coordinates and sets the validity
 * of the volume coordinates to true.
 *
 * @param volumeXYZ new value of volumeXYZ
 *
 */
void
SurfaceProjectedItem::setVolumeXYZ(const float volumeXYZ[3])
{
    this->volumeXYZ[0] = volumeXYZ[0];
    this->volumeXYZ[1] = volumeXYZ[1];
    this->volumeXYZ[2] = volumeXYZ[2];
    this->volumeXYZValid = true;
    this->setModified();
}

/**
 * Get the structure of this projected item.
 * @return The structure.
 *
 */
StructureEnum::Enum
SurfaceProjectedItem::getStructure() const
{
    return this->structure;
}

/**
 * Set the structure of this projected item.
 * @param s - new structure.
 *
 */
void
SurfaceProjectedItem::setStructure(const StructureEnum::Enum structure)
{
    this->structure = structure;
    this->setModified();
}

/** 
 * @return the barycentric projection 
 */
SurfaceProjectionBarycentric* 
SurfaceProjectedItem::getBarycentricProjection()
{
    return this->barycentricProjection;
}

/** 
 * @return the barycentric projection 
 */
const SurfaceProjectionBarycentric* 
SurfaceProjectedItem::getBarycentricProjection() const
{
    return this->barycentricProjection;
}

/**
 * @return the Van Essen projection 
 */
SurfaceProjectionVanEssen* 
SurfaceProjectedItem::getVanEssenProjection()
{
    return this->vanEssenProjection;
}

/** 
 * @return the Van Essen projection 
 */
const SurfaceProjectionVanEssen* 
SurfaceProjectedItem::getVanEssenProjection() const
{
    return this->vanEssenProjection;
}

/**
 * Write the border to the XML Writer.
 * @param xmlWriter
 *   Writer for XML output.
 */
void 
SurfaceProjectedItem::writeAsXML(XmlWriter& xmlWriter)
{
    xmlWriter.writeStartElement(XML_TAG_SURFACE_PROJECTED_ITEM);
    xmlWriter.writeElementCharacters(XML_TAG_STRUCTURE, StructureEnum::toName(this->structure));
    if (this->stereotaxicXYZValid) {
        xmlWriter.writeElementCharacters(XML_TAG_STEREOTAXIC_XYZ, this->stereotaxicXYZ, 3);
    }
    if (this->volumeXYZValid) {
        xmlWriter.writeElementCharacters(XML_TAG_VOLUME_XYZ, this->volumeXYZ, 3);
    }
    this->barycentricProjection->writeAsXML(xmlWriter);
    this->vanEssenProjection->writeAsXML(xmlWriter);
    xmlWriter.writeEndElement();
}

void SurfaceProjectedItem::readBorderFileXML1(QXmlStreamReader& xml)
{
    reset();
    CaretAssert(xml.isStartElement() && xml.name() == "SurfaceProjectedItem");
    bool haveStructure = false, haveVanEssen = false, haveStereo = false, haveVolume = false;//track the barycentric projection for being specified more than once by its valid flag
    for (xml.readNext(); !xml.atEnd() && !xml.isEndElement(); xml.readNext())
    {
        switch (xml.tokenType())
        {
            case QXmlStreamReader::StartElement:
            {
                QStringRef name = xml.name();
                if (name == "Structure")
                {
                    if (haveStructure) throw DataFileException("multiple Structure elements in one SurfaceProjectedItem element");
                    QString structString = xml.readElementText();//sets error on unexpected child element
                    if (xml.hasError()) throw DataFileException("XML parsing error in Structure: " + xml.errorString());
                    bool ok = false;
                    structure = StructureEnum::fromName(structString, &ok);
                    if (!ok) CaretLogWarning("unrecognized string in Structure: " + structString);//HACK: this is what the SAX reader did, don't look at me
                    haveStructure = true;
                } else if (name == "ProjectionBarycentric") {
                    if (barycentricProjection->isValid()) throw DataFileException("multiple ProjectionBarycentric elements in one SurfaceProjectedItem element");
                    barycentricProjection->readBorderFileXML1(xml);
                } else if (name == "VanEssenProjection") {
                    if (haveVanEssen) throw DataFileException("multiple VanEssenProjection elements in one SurfaceProjectedItem element");
                    CaretLogFine("found Van Essen projection in border file, ignoring");
                    xml.readElementText(QXmlStreamReader::SkipChildElements);//HACK: border files never use this projection type, so don't try to parse it
                    if (xml.hasError()) throw DataFileException("XML parsing error in VanEssenProjection: " + xml.errorString());
                    haveVanEssen = true;
                } else if (name == "StereotaxicXYZ") {
                    if (haveStereo) throw DataFileException("multiple StereotaxicXYZ elements in one SurfaceProjectedItem element");
                    CaretLogFine("found stereotaxic coordinates in border file, ignoring");
                    xml.readElementText(QXmlStreamReader::SkipChildElements);//HACK: ditto
                    if (xml.hasError()) throw DataFileException("XML parsing error in StereotaxicXYZ: " + xml.errorString());
                    haveStereo = true;
                } else if (name == "VolumeXYZ") {
                    if (haveVolume) throw DataFileException("multiple VolumeXYZ elements in one SurfaceProjectedItem element");
                    CaretLogFine("found volume coordinates in border file, ignoring");
                    xml.readElementText(QXmlStreamReader::SkipChildElements);//HACK: ditto
                    if (xml.hasError()) throw DataFileException("XML parsing error in VolumeXYZ: " + xml.errorString());
                    haveVolume = true;
                } else {
                    throw DataFileException("unexpected element in SurfaceProjectedItem: " + name.toString());
                }
                break;
            }
            default:
                break;
        }
    }
    if (xml.hasError()) throw DataFileException("XML parsing error in SurfaceProjectedItem: " + xml.errorString());
    CaretAssert(xml.isEndElement() && xml.name() == "SurfaceProjectedItem");
    if (!haveStructure) throw DataFileException("SurfaceProjectedItem is missing Structure element");
}

/**
 * Set the status to unmodified.
 */
void 
SurfaceProjectedItem::clearModified()
{
    CaretObjectTracksModification::clearModified();
    this->barycentricProjection->clearModified();
    this->vanEssenProjection->clearModified();
}

/**
 * Is the object modified?
 * @return true if modified, else false.
 */
bool 
SurfaceProjectedItem::isModified() const
{
    if (CaretObjectTracksModification::isModified()) {
        return true;
    }
    if (this->barycentricProjection->isModified()) {
        return true;
    }
    if (this->vanEssenProjection->isModified()) {
        return true;
    }
    
    return false;
}

/**
 * @return True if a projection (barycentric, vanessen)
 * is valid.  Otherwise, false.
 */
bool
SurfaceProjectedItem::hasValidProjection() const
{
    if (barycentricProjection->isValid()) {
        return true;
    }
    if (vanEssenProjection->isValid()) {
        return true;
    }
    return false;
}


