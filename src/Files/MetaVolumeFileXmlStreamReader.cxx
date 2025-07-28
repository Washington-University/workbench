
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

#include <QFile>
#include <QXmlStreamAttributes>
#include <QXmlStreamReader>

#define __META_VOLUME_FILE_XML_STREAM_READER_DECLARE__
#include "MetaVolumeFileXmlStreamReader.h"
#undef __META_VOLUME_FILE_XML_STREAM_READER_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DataFileException.h"
#include "GiftiMetaData.h"
#include "GiftiXmlElements.h"
#include "FileInformation.h"
#include "MetaVolumeFile.h"
#include "PaletteColorMapping.h"
#include "VolumeFile.h"

using namespace caret;


    
/**
 * \class caret::MetaVolumeFileXmlStreamReader 
 * \brief XML Stream Writer for Scene File
 * \ingroup Files
 */

/**
 * Constructor.
 */
MetaVolumeFileXmlStreamReader::MetaVolumeFileXmlStreamReader()
: MetaVolumeFileXmlStreamBase()
{
    
}

/**
 * Destructor.
 */
MetaVolumeFileXmlStreamReader::~MetaVolumeFileXmlStreamReader()
{
}

/**
 * Read into the given scene file from the file with the given name
 *
 * @param filename
 *     Name of the scene file
 * @param metaVolumeFile
 *     Pointer to meta-volume file that data is read into
 * @throws
 *     Data file exception
 */
void
MetaVolumeFileXmlStreamReader::readFile(const AString& filename,
                                        MetaVolumeFile* metaVolumeFile)
{
    CaretAssert(metaVolumeFile);
    if (metaVolumeFile == NULL) {
        throw DataFileException("Meta-volume file is invalid (NULL).");
    }
    
    if (filename.isEmpty()) {
        throw DataFileException("Meta-volume file name is empty");
    }
    
    m_filename = filename;
    
    QFile file(m_filename);
    if ( ! file.open(QFile::ReadOnly)) {
        throw DataFileException("Unable to open for reading: "
                                + m_filename
                                + " Reason: "
                                + file.errorString());
    }
    
    QXmlStreamReader xmlReader(&file);
    readFileContent(FileInformation(filename).getAbsolutePath(),
                    xmlReader,
                    metaVolumeFile);

    AString errorMessage;
    if (xmlReader.hasError()) {
        errorMessage = xmlReader.errorString();
        errorMessage.appendWithNewLine("Line "
                                       + AString::number(xmlReader.lineNumber())
                                       + " column "
                                       + AString::number(xmlReader.columnNumber()));
    }

    file.close();
    
    if ( ! errorMessage.isEmpty()) {
        throw DataFileException(errorMessage);
    }
}

/**
 * Read the file's content
 *
 * @param metaVolumeFilePath
 *     Path to metavolume file
 * @param xmlReader
 *     The XML stream reader
 * @param metaVolumeFile
 *     Pointer to meta-volume file that data is read into
 */
void
MetaVolumeFileXmlStreamReader::readFileContent(const AString& metaVolumeFilePath,
                                               QXmlStreamReader& xmlReader,
                                               MetaVolumeFile* metaVolumeFile)
{
    CaretAssert(metaVolumeFile);
    
    if (xmlReader.atEnd()) {
        xmlReader.raiseError("At end of file when starting to read.  Is file empty?");
        return;
    }
    
    xmlReader.readNextStartElement();
    if (xmlReader.name() != ELEMENT_META_VOLUME_FILE) {
        xmlReader.raiseError("First element is \""
                             + xmlReader.name().toString()
                             + "\" but should be "
                             + ELEMENT_META_VOLUME_FILE);
        return;
    }
    
    const QXmlStreamAttributes atts = xmlReader.attributes();
    const auto versionAtt     = atts.value(ATTRIBUTE_META_VOLUME_FILE_VERSION);
    m_fileVersion = 1;
    if ( ! versionAtt.isEmpty()) {
        /*
         * Note: version was float in previous versions, so get as float and convert to int
         */
        m_fileVersion = static_cast<int32_t>(versionAtt.toFloat());
    }
    
    std::vector<AString> volumeFileNames;
    std::vector<AString> missingVolumeFileNames;
    
    std::map<int32_t, std::unique_ptr<GiftiMetaData>> metadataRead;
    
    /*
     * Set when ending  element is found
     */
    bool endElementFound(false);
    
    while ( ( ! xmlReader.atEnd())
           && ( ! endElementFound)) {
        xmlReader.readNext();
        switch (xmlReader.tokenType()) {
            case QXmlStreamReader::StartElement:
                if (xmlReader.name() == GiftiXmlElements::TAG_METADATA) {
                    GiftiMetaData* metaData = metaVolumeFile->getFileMetaData();
                    metaData->readSceneFile3(xmlReader);
                }
                else if (xmlReader.name() == ELEMENT_MAP_INFO) {
                    readMapInfo(xmlReader,
                                metadataRead);
                }
                else if (xmlReader.name() == ELEMENT_FILE) {
                    QString filename(xmlReader.readElementText());
                    FileInformation fileInfo(filename);
                    if ( ! fileInfo.isAbsolute()) {
                        FileInformation f(FileInformation::assembleFileComponents(metaVolumeFilePath,
                                                                                  filename));
                        filename = FileInformation::cleanPath(f.getAbsoluteFilePath());
                    }
                    
                    if (FileInformation(filename).exists()) {
                        volumeFileNames.push_back(filename);
                    }
                    else {
                        missingVolumeFileNames.push_back(filename);
                    }
                }
                else {
                    m_unexpectedXmlElements.insert(xmlReader.name().toString());
                    xmlReader.skipCurrentElement();
                }
                break;
            case QXmlStreamReader::EndElement:
                if (xmlReader.name() == ELEMENT_META_VOLUME_FILE) {
                    endElementFound = true;
                }
                break;
            default:
                break;
        }
    }
    
    if ( ! missingVolumeFileNames.empty()) {
        const AString msg("These volume files do not exist: "
                          + AString::join(missingVolumeFileNames, "\n"));
        xmlReader.raiseError(msg);
    }
    else {
        std::vector<AString> filenameErrors;
        for (const AString& filename : volumeFileNames) {
            try {
                std::unique_ptr<VolumeFile> vf(new VolumeFile());
                vf->readFile(filename);
                metaVolumeFile->addVolumeFile(vf.release());
            }
            catch (const DataFileException& dfe) {
                filenameErrors.push_back(filename
                                         + ": " + dfe.whatString());
            }
        }
        
        if ( ! filenameErrors.empty()) {
            const AString msg("Errors reading volume files: "
                              + AString::join(filenameErrors, "\n"));
            xmlReader.raiseError(msg);
            metaVolumeFile->clear();
        }
        else {
            const int32_t numMaps(metaVolumeFile->getNumberOfMaps());
            for (int32_t iMap = 0; iMap < numMaps; iMap++) {
                const auto iter(metadataRead.find(iMap));
                if (iter != metadataRead.end()) {
                    const std::unique_ptr<GiftiMetaData>& md(iter->second);
                    if (md) {
                        /*
                         * Extract the palette color mapping from the metadata that was read
                         * and then remove palette color mapping from this metadata
                         */
                        const AString pcmString(md->get(METADATA_NAME_PALETTE_COLOR_MAPPING));
                        if ( ! pcmString.isEmpty()) {
                            try {
                                metaVolumeFile->getMapPaletteColorMapping(iMap)->decodeFromStringXML(pcmString);
                                md->remove(METADATA_NAME_PALETTE_COLOR_MAPPING);
                            }
                            catch (const XmlException& e) {
                                CaretLogSevere("Error reading palette color mapping for map="
                                               + AString::number(iMap)
                                               + " in "
                                               + metaVolumeFile->getFileName());
                            }
                        }
                        
                        GiftiMetaData* mapMetaData(metaVolumeFile->getMapMetaData(iMap));
                        *mapMetaData = *md.get();
                    }
                }
            }
        }
    }
}

/**
 * Read the scene info directory
 *
 * @param xmlReader
 *     The XML stream reader
 *  @param metadataOut
 *     Output with metdata that was read for each map
 */
void
MetaVolumeFileXmlStreamReader::readMapInfo(QXmlStreamReader& xmlReader,
                                           std::map<int32_t, std::unique_ptr<GiftiMetaData>>& metadataOut)
{
    metadataOut.clear();
    
    /*
     * Set when ending  element is found
     */
    bool endElementFound(false);
    
    int32_t readingMapIndex(-1);
    while ( ( ! xmlReader.atEnd())
           && ( ! endElementFound)) {
        xmlReader.readNext();
        switch (xmlReader.tokenType()) {
            case QXmlStreamReader::StartElement:
                if (xmlReader.name() == GiftiXmlElements::TAG_METADATA) {
                    GiftiMetaData* metaData(new GiftiMetaData());
                    metaData->readSceneFile3(xmlReader);
                    CaretAssert(readingMapIndex >= 0);
                    metadataOut.emplace(readingMapIndex,
                                        metaData);
                }
                else if (xmlReader.name() == ELEMENT_MAP) {
                    const QXmlStreamAttributes atts = xmlReader.attributes();
                    readingMapIndex = atts.value(ATTRIBUTE_INDEX).toInt();
                }
                else {
                    m_unexpectedXmlElements.insert(xmlReader.name().toString());
                    xmlReader.skipCurrentElement();
                }
                break;
            case QXmlStreamReader::EndElement:
                if (xmlReader.name() == ELEMENT_MAP_INFO) {
                    endElementFound = true;
                }
                else if (xmlReader.name() == ELEMENT_MAP) {
                    readingMapIndex = -1;
                }
                break;
            default:
                break;
        }
    }
}
