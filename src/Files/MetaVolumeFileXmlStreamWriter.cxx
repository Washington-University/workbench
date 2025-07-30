
/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

#define __META_VOLUME_FILE_XML_STREAM_WRITER_DECLARE__
#include "MetaVolumeFileXmlStreamWriter.h"
#undef __META_VOLUME_FILE_XML_STREAM_WRITER_DECLARE__

#include <QDir>
#include <QFile>
#include <QXmlStreamWriter>

#include "CaretAssert.h"
#include "DataFileException.h"
#include "FileInformation.h"
#include "GiftiMetaData.h"
#include "MetaVolumeFile.h"
#include "PaletteColorMapping.h"
#include "ScenePathName.h"
#include "SystemUtilities.h"
#include "VolumeFile.h"

using namespace caret;


    
/**
 * \class caret::MetaVolumeFileXmlStreamWriter 
 * \brief XML Stream reader for Meta VolumeFile
 * \ingroup Files
 */

/**
 * Constructor.
 */
MetaVolumeFileXmlStreamWriter::MetaVolumeFileXmlStreamWriter()
: MetaVolumeFileXmlStreamBase()
{
    
}

/**
 * Destructor.
 */
MetaVolumeFileXmlStreamWriter::~MetaVolumeFileXmlStreamWriter()
{
}

/**
 * Write the given MetaVolume File in XML format.  Name of the file
 * is obtained from the file.
 *
 * @param metaVolumeFile
 *     Meta Volume File written in XML format.
 * @throws
 *     DataFileException if there is an error writing the file.
 */
void
MetaVolumeFileXmlStreamWriter::writeFile(const MetaVolumeFile* metaVolumeFile)
{
    CaretAssert(metaVolumeFile);
    
    const QString filename = metaVolumeFile->getFileName();
    if (filename.isEmpty()) {
        throw DataFileException("Name for writing annotation file is empty.");
    }
    
    QFile file(filename);
    if ( ! file.open(QFile::WriteOnly)) {
        throw DataFileException(filename,
                                "Error opening for writing: "
                                + file.errorString());
    }

    QXmlStreamWriter xmlWriter(&file);
    writeFileContentToXmlStreamWriter(xmlWriter,
                                      metaVolumeFile);
    
    file.close();
    
    if (xmlWriter.hasError()) {
        throw DataFileException(filename,
                                ("Unknown error writing file "
                                 + metaVolumeFile->getFileNameNoPath()));
    }
}

/**
 * Write the meta volume files content to the XML stream.
 *
 * @param xmlWriter
 *     The XML stream writer
 * @param metaVolumeFile
 *     The meta-volume file
 */
void
MetaVolumeFileXmlStreamWriter::writeFileContentToXmlStreamWriter(QXmlStreamWriter& xmlWriter,
                                                                 const MetaVolumeFile* metaVolumeFile)
{
    CaretAssert(metaVolumeFile);
    
    const AString versionString("1");

    xmlWriter.setAutoFormatting(true);
    
    xmlWriter.writeStartDocument();
    
    xmlWriter.writeStartElement(ELEMENT_META_VOLUME_FILE);
    xmlWriter.writeAttribute(ATTRIBUTE_META_VOLUME_FILE_VERSION,
                             versionString);
    
    const GiftiMetaData* metaData = metaVolumeFile->getFileMetaData();
    if ( ! metaData->isEmpty()) {
        metaData->writeSceneFile3(xmlWriter);
    }
    
    writeMapInfoDirectory(xmlWriter,
                          metaVolumeFile);
    
    const int32_t numVolumeFiles(metaVolumeFile->getNumberOfVolumeFiles());
    for (int32_t i = 0; i < numVolumeFiles; i++) {
        const AString volumeFileName(metaVolumeFile->getVolumeFile(i)->getFileName());
        
        QDir metaVolumeDir(metaVolumeFile->getFilePath());
        const QString relativeFileName(metaVolumeDir.relativeFilePath(volumeFileName));
        xmlWriter.writeTextElement(ELEMENT_FILE, relativeFileName);
    }
    
    xmlWriter.writeEndElement();
    
    xmlWriter.writeEndDocument();
}

/**
 * Write the map info directory to the XML stream
 *
 * @param xmlWriter
 *     The XML stream writer
 * @param metaVolumeFile
 *     The meta-volume file
 */
void
MetaVolumeFileXmlStreamWriter::writeMapInfoDirectory(QXmlStreamWriter& xmlWriter,
                                                     const MetaVolumeFile* metaVolumeFile)
{
    const int32_t numMaps(metaVolumeFile->getNumberOfMaps());
    if (numMaps <= 0) {
        return;
    }
    
    xmlWriter.writeStartElement(ELEMENT_MAP_INFO);
        
    for (int32_t i = 0; i < numMaps; i++) {
        xmlWriter.writeStartElement(ELEMENT_MAP);
        xmlWriter.writeAttribute(ATTRIBUTE_INDEX, QString::number(i));
        
        /*
         * Make a copy of metadata so that we can put the palette color
         * mapping in the metadata for writing without modifying the
         * metadata's modification status.
         */
        const GiftiMetaData* mdPtr(metaVolumeFile->getMapMetaData(i));
        CaretAssert(mdPtr);
        GiftiMetaData md(*mdPtr);
        
        if (metaVolumeFile->isMappedWithPalette()) {
            PaletteColorMapping* pcm(const_cast<PaletteColorMapping*>(metaVolumeFile->getMapPaletteColorMapping(i)));
            CaretAssert(pcm);
            md.set(METADATA_NAME_PALETTE_COLOR_MAPPING,
                   pcm->encodeInXML());
        }

        if ( ! metaVolumeFile->getMapNameFromMapInfo(i).isEmpty()) {
            md.set(METADATA_NAME_MAP_NAME,
                   metaVolumeFile->getMapNameFromMapInfo(i));
        }
        
        md.writeSceneFile3(xmlWriter);
        xmlWriter.writeEndElement();
    }
    
    xmlWriter.writeEndElement();
}
