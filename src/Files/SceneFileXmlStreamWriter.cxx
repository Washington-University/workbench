
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

#define __SCENE_FILE_XML_STREAM_WRITER_DECLARE__
#include "SceneFileXmlStreamWriter.h"
#undef __SCENE_FILE_XML_STREAM_WRITER_DECLARE__

#include <QFile>
#include <QXmlStreamWriter>

#include "CaretAssert.h"
#include "DataFileException.h"
#include "FileInformation.h"
#include "GiftiMetaData.h"
#include "Scene.h"
#include "SceneFile.h"
#include "SceneInfoXmlStreamWriter.h"
#include "ScenePathName.h"
#include "SceneXmlStreamWriter.h"

using namespace caret;


    
/**
 * \class caret::SceneFileXmlStreamWriter 
 * \brief XML Stream reader for Scene File
 * \ingroup Files
 */

/**
 * Constructor.
 */
SceneFileXmlStreamWriter::SceneFileXmlStreamWriter()
: SceneFileXmlStreamBase()
{
    
}

/**
 * Destructor.
 */
SceneFileXmlStreamWriter::~SceneFileXmlStreamWriter()
{
}

/**
 * Write the given Scene File in XML format.  Name of the file
 * is obtained from the file.
 *
 * @param sceneFile
 *     Scene File written in XML format.
 * @throws
 *     DataFileException if there is an error writing the file.
 */
void
SceneFileXmlStreamWriter::writeFile(const SceneFile* sceneFile)
{
    CaretAssert(sceneFile);
    
    const QString filename = sceneFile->getFileName();
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
                                      sceneFile,
                                      filename);
    
    file.close();
    
    if (xmlWriter.hasError()) {
        throw DataFileException(filename,
                                ("Unknown error writing file "
                                 + sceneFile->getFileNameNoPath()));
    }
}

/**
 * Write the scene files content to the XML stream.
 *
 * @param xmlWriter
 *     The XML stream writer
 * @param sceneFile
 *     The scene file
 * @param sceneFileName
 *     Name of the file.
 */
void
SceneFileXmlStreamWriter::writeFileContentToXmlStreamWriter(QXmlStreamWriter& xmlWriter,
                                                            const SceneFile* sceneFile,
                                                            const AString& sceneFileName)
{
    CaretAssert(sceneFile);
    
    const AString versionString = AString::number(sceneFile->getSceneFileVersionForWriting());

    xmlWriter.setAutoFormatting(true);
    
    xmlWriter.writeStartDocument();
    
    xmlWriter.writeStartElement(ELEMENT_SCENE_FILE);
    xmlWriter.writeAttribute(ATTRIBUTE_SCENE_FILE_VERSION,
                             versionString);
    
    const GiftiMetaData* metaData = sceneFile->getFileMetaData();
    if ( ! metaData->isEmpty()) {
        metaData->writeSceneFile3(xmlWriter);
    }
    
    writeSceneInfoDirectory(xmlWriter,
                            sceneFile,
                            sceneFileName);
    
    const int32_t numberOfScenes = sceneFile->getNumberOfScenes();
    for (int32_t sceneIndex = 0; sceneIndex < numberOfScenes; sceneIndex++) {
        SceneXmlStreamWriter sceneXmlWriter;
        sceneXmlWriter.writeXML(&xmlWriter,
                                sceneFile->getSceneAtIndex(sceneIndex),
                                sceneIndex,
                                sceneFileName);
    }
    
    xmlWriter.writeEndElement();
    
    xmlWriter.writeEndDocument();
}

/**
 * Write the scene info directory to the XML stream
 *
 * @param xmlWriter
 *     The XML stream writer
 * @param sceneFile
 *     The scene file
 * @param sceneFileName
 *     Name of the file.
 */
void
SceneFileXmlStreamWriter::writeSceneInfoDirectory(QXmlStreamWriter& xmlWriter,
                                                  const SceneFile* sceneFile,
                                                  const AString& sceneFileName)
{
    xmlWriter.writeStartElement(ELEMENT_SCENE_FILE_INFO_DIRECTORY);
    
    xmlWriter.writeTextElement(ELEMENT_SCENE_FILE_BALSA_STUDY_ID,
                               sceneFile->getBalsaStudyID());
    xmlWriter.writeTextElement(ELEMENT_SCENE_FILE_BALSA_STUDY_TITLE,
                               sceneFile->getBalsaStudyTitle());
    
    AString relativeBasePath("");
    switch (sceneFile->getBasePathType()) {
        case SceneFileBasePathTypeEnum::AUTOMATIC:
            break;
        case SceneFileBasePathTypeEnum::CUSTOM:
        {
            /*
             * Write base path as a path RELATIVE to the scene file
             * but only when base path type is CUSTOM
             * Note: we do not use FileInformation::getCanonicalFilePath()
             * because it returns an empty string if the file DOES NOT exist
             * and this may occur since the file may be new and has not
             * been closed.
             */
            if ( ! sceneFile->getBalsaCustomBaseDirectory().isEmpty()) {
                const AString baseDirAbsPath = FileInformation(sceneFile->getBalsaCustomBaseDirectory()).getAbsoluteFilePath();
                const AString sceneFileAbsPath = FileInformation(sceneFileName).getAbsoluteFilePath();
                ScenePathName basePathName("basePathName",
                                           baseDirAbsPath);
                
                relativeBasePath = basePathName.getRelativePathToSceneFile(sceneFileAbsPath);
            }
        }
            break;
    }
    xmlWriter.writeTextElement(ELEMENT_SCENE_FILE_BALSA_BASE_DIRECTORY,
                               relativeBasePath);
    
    
    xmlWriter.writeTextElement(ELEMENT_SCENE_FILE_BALSA_EXTRACT_TO_DIRECTORY,
                               sceneFile->getBalsaExtractToDirectoryName());
    xmlWriter.writeTextElement(ELEMENT_SCENE_FILE_BALSA_BASE_PATH_TYPE,
                               SceneFileBasePathTypeEnum::toName(sceneFile->getBasePathType()));
    
    const int32_t numberOfScenes = sceneFile->getNumberOfScenes();
    for (int32_t sceneIndex = 0; sceneIndex < numberOfScenes; sceneIndex++) {
        SceneInfoXmlStreamWriter infoXmlWriter;
        infoXmlWriter.writeXML(&xmlWriter,
                               sceneFile->getSceneAtIndex(sceneIndex)->getSceneInfo(),
                               sceneIndex);
    }
    
    xmlWriter.writeEndElement();
}
