
/*LICENSE_START*/
/*
 *  Copyright (C) 2022 Washington University School of Medicine
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

#define __HISTOLOGY_SLICES_FILE_XML_STREAM_READER_DECLARE__
#include "HistologySlicesFileXmlStreamReader.h"
#undef __HISTOLOGY_SLICES_FILE_XML_STREAM_READER_DECLARE__

#include <QFile>
#include <QFileInfo>
#include <QXmlStreamAttributes>
#include <QXmlStreamReader>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DataFileException.h"
#include "HistologySlice.h"
#include "HistologySliceImage.h"
#include "HistologySlicesFile.h"
#include "Matrix4x4.h"
#include "XmlStreamReaderHelper.h"

using namespace caret;


    
/**
 * \class caret::HistologySlicesFileXmlStreamReader
 * \brief Xml Stream Reader for Meta CZI file
 * \ingroup Files
 */

/**
 * Constructor.
 */
HistologySlicesFileXmlStreamReader::HistologySlicesFileXmlStreamReader()
: HistologySlicesFileXmlStreamBase()
{
    
}

/**
 * Destructor.
 */
HistologySlicesFileXmlStreamReader::~HistologySlicesFileXmlStreamReader()
{
}

/**
 * Read a meta CZI file content into a histology slices file
 * @param filename
 *    Name of meta czi file
 * @param histologySlicesFile
 *    Pointer to histology slices file that is read
 * @throws DataFileException if fatal error
 */
void
HistologySlicesFileXmlStreamReader::readFile(const AString& filename,
                                     HistologySlicesFile* histologySlicesFile)
{
    m_xmlStreamHelper.reset();
    
    CaretAssert(histologySlicesFile);
    if (histologySlicesFile == NULL) {
        throw DataFileException("Histology slices file is invalid (NULL).");
    }
    histologySlicesFile->clear();
    histologySlicesFile->setFileName(filename);
    
    if (filename.isEmpty()) {
        throw DataFileException("Histology slices file name is empty");
    }
    
    m_filename = filename;
    
    QFile file(m_filename);
    if ( ! file.open(QFile::ReadOnly)) {
        throw DataFileException("Unable to open for reading: "
                                + m_filename
                                + " Reason: "
                                + file.errorString());
    }
    
    QFileInfo fileInfo(m_filename);
    m_directory = fileInfo.dir();
    
    m_xmlReader.reset(new QXmlStreamReader(&file));
    m_xmlStreamHelper.reset(new XmlStreamReaderHelper(m_filename,
                                                      m_xmlReader.get()));
    
    readFileContent(histologySlicesFile);
    
    
    m_xmlStreamHelper.reset();
    
    file.close();
    
    const QString errorMessage(m_xmlReader->errorString());
    if (m_xmlReader->hasError()) {
        m_xmlReader.reset();
        throw DataFileException(errorMessage);
    }
    m_xmlReader.reset();
    
    if ( ! m_unexpectedXmlElements.empty()) {
        AString msg("These unrecognized elements were found in "
                    + filename);
        for (auto& e : m_unexpectedXmlElements) {
            msg.appendWithNewLine("   "
                                  + e);
        }
        CaretLogWarning(msg);
    }
}

/**
 * Read the histology slices file's content
 *
 * @param xmlReader
 *     The XML stream reader
 * @param histologySlicesFile
 *     Into this histology slices file
 */
void
HistologySlicesFileXmlStreamReader::readFileContent(HistologySlicesFile* histologySlicesFile)
{
    CaretAssert(histologySlicesFile);
    
    if (m_xmlReader->atEnd()) {
        m_xmlReader->raiseError("At end of file when starting to read.  Is file empty?");
        return;
    }
    
    m_xmlReader->readNextStartElement();
    if (m_xmlReader->name() != ELEMENT_META_CZI) {
        m_xmlReader->raiseError("First element is \""
                                + m_xmlReader->name().toString()
                                + "\" but should be "
                                + ELEMENT_META_CZI);
        return;
    }
    
    m_fileVersion = m_xmlStreamHelper->getRequiredIntAttributeRaiseError(ELEMENT_META_CZI,
                                                                         ATTRIBUTE_VERSION);
    if (m_xmlReader->hasError()) {
        return;
    }
    
    if (m_fileVersion > 1) {
        m_xmlReader->raiseError("File version is "
                                + AString::number(m_fileVersion)
                                + " but only version 1 is supported");
        return;
    }
    
    /*
     * Set when ending scene file element is found
     */
    bool endElementFound(false);
    
    while ( ( ! m_xmlReader->atEnd())
           && ( ! endElementFound)) {
        m_xmlReader->readNext();
        switch (m_xmlReader->tokenType()) {
            case QXmlStreamReader::StartElement:
                if (m_xmlReader->name() == ELEMENT_SLICE) {
                    const int32_t sliceNumber(m_xmlStreamHelper->getRequiredIntAttributeRaiseError(ELEMENT_SLICE,
                                                                                                   ATTRIBUTE_NUMBER));
                    if ( ! m_xmlReader->hasError()) {
                        HistologySlice* slice(readSliceElement(histologySlicesFile,
                                                               sliceNumber));
                        if (slice != NULL) {
                            histologySlicesFile->addHistologySlice(slice);
                        }
                    }
                }
                else {
                    m_unexpectedXmlElements.insert(m_xmlReader->name().toString());
                    m_xmlReader->skipCurrentElement();
                }
                break;
            case QXmlStreamReader::EndElement:
                if (m_xmlReader->name() == ELEMENT_META_CZI) {
                    endElementFound = true;
                }
                break;
            default:
                break;
        }
    }
}

/**
 * Read a slice element from the XML
 * @param histologySlicesFile
 *     The histology slices file
 * @param sliceNumber
 *     Slice number from Slice element
 * @return
 *    Pointer to slice read.
 */
HistologySlice*
HistologySlicesFileXmlStreamReader::readSliceElement(HistologySlicesFile* histologySlicesFile,
                                             const int32_t sliceNumber)
{
    /*
     * Set when ending scene file element is found
     */
    bool endElementFound(false);
    
    AString histToMriWarpFileName;
    AString mriToHistWarpFileName;
    Matrix4x4 planeToMmMatrix;
    bool planeToMmMatrixValidFlag(false);
    
    std::vector<std::unique_ptr<HistologySliceImage>> images;
    
    while ( ( ! m_xmlReader->atEnd())
           && ( ! endElementFound)) {
        m_xmlReader->readNext();
        switch (m_xmlReader->tokenType()) {
            case QXmlStreamReader::StartElement:
                if (m_xmlReader->name() == ELEMENT_HIST_TO_MRI_WARP) {
                    histToMriWarpFileName = m_xmlStreamHelper->getRequiredStringAttributeRaiseError(ELEMENT_HIST_TO_MRI_WARP,
                                                                                                    ATTRIBUTE_FILE);
                    if ( ! m_xmlReader->hasError()) {
                        
                    }
                }
                else if (m_xmlReader->name() == ELEMENT_MRI_TO_HIST_WARP) {
                    mriToHistWarpFileName = m_xmlStreamHelper->getRequiredStringAttributeRaiseError(ELEMENT_MRI_TO_HIST_WARP,
                                                                                                    ATTRIBUTE_FILE);
                    if ( ! m_xmlReader->hasError()) {
                        
                    }
                }
                else if (m_xmlReader->name() == ELEMENT_PLANE_TO_MM) {
                    readMatrixFromElementText(ELEMENT_PLANE_TO_MM,
                                              MatrixType::TWO_DIM,
                                              planeToMmMatrix);
                    planeToMmMatrixValidFlag = true;
                }
                else if (m_xmlReader->name() == ELEMENT_SCENE) {
                    const QString sceneName(m_xmlStreamHelper->getRequiredStringAttributeRaiseError(ELEMENT_SCENE,
                                                                                                    ATTRIBUTE_NAME));
                    if ( ! m_xmlReader->hasError()) {
                        HistologySliceImage* image(readSceneElement(histologySlicesFile,
                                                                    sceneName));
                        if (image != NULL) {
                            std::unique_ptr<HistologySliceImage> ptr(image);
                            images.push_back(std::move(ptr));
                        }
                    }
                }
                else {
                    m_unexpectedXmlElements.insert(m_xmlReader->name().toString());
                    m_xmlReader->skipCurrentElement();
                }
                break;
            case QXmlStreamReader::EndElement:
                if (m_xmlReader->name() == ELEMENT_SLICE) {
                    endElementFound = true;
                }
                break;
            default:
                break;
        }
    }
    
    if (m_xmlReader->hasError()) {
        return NULL;
    }
    
    if (images.empty()) {
        m_xmlReader->raiseError("Slice "
                                + AString::number(sliceNumber)
                                + " contains no scenes (images)");
        return NULL;
    }
    
    const int32_t sliceIndex(histologySlicesFile->getNumberOfHistologySlices());
    mriToHistWarpFileName = makeAbsoluteFilePath(mriToHistWarpFileName);
    histToMriWarpFileName = makeAbsoluteFilePath(histToMriWarpFileName);
    HistologySlice* slice(new HistologySlice(sliceIndex,
                                             sliceNumber,
                                             mriToHistWarpFileName,
                                             histToMriWarpFileName,
                                             planeToMmMatrix,
                                             planeToMmMatrixValidFlag));
    for (auto& img : images) {
        slice->addHistologySliceImage(img.release());
    }
    
    return slice;
}

/**
 * If the filename is not absolute path, convert to absolute path using
 * the directory containing the CZI metaafile.
 * @param filename
 *    Name of file
 * @return
 *    Absolute path to file.
 */
AString
HistologySlicesFileXmlStreamReader::makeAbsoluteFilePath(const AString& filename) const
{
    if (QFileInfo(filename).isAbsolute()) {
        return filename;
    }
    
    QFileInfo fileInfo(m_directory,
                       filename);
    return fileInfo.absoluteFilePath();
}

/**
 * Read a scene element from the XML
 * @param histologySlicesFile
 *     Into this Histology Slices file
 * @param sceneName
 *     Name of scene
 * @return
 *     Pointer to scene
 */
HistologySliceImage*
HistologySlicesFileXmlStreamReader::readSceneElement(HistologySlicesFile* /*histologySlicesFile*/,
                                             const QString& sceneName)
{
    /*
     * Set when ending scene file element is found
     */
    bool endElementFound(false);
    
    AString distanceFileName;
    AString imageFileName;
    Matrix4x4 scaledToPlaneMatrix;
    bool scaledToPlaneMatrixValidFlag(false);
    
    while ( ( ! m_xmlReader->atEnd())
           && ( ! endElementFound)) {
        m_xmlReader->readNext();
        switch (m_xmlReader->tokenType()) {
            case QXmlStreamReader::StartElement:
                if (m_xmlReader->name() == ELEMENT_SCALED_TO_PLANE) {
                    readMatrixFromElementText(ELEMENT_SCALED_TO_PLANE,
                                              MatrixType::TWO_DIM,
                                              scaledToPlaneMatrix);
                    scaledToPlaneMatrixValidFlag = true;
                }
                else if (m_xmlReader->name() == ELEMENT_CZI) {
                    imageFileName = m_xmlStreamHelper->getRequiredStringAttributeRaiseError(ELEMENT_CZI,
                                                                                            ATTRIBUTE_FILE);
                    if ( ! m_xmlReader->hasError()) {
                        
                    }
                }
                else if (m_xmlReader->name() == ELEMENT_DISTANCE) {
                    distanceFileName = m_xmlStreamHelper->getRequiredStringAttributeRaiseError(ELEMENT_DISTANCE,
                                                                                          ATTRIBUTE_FILE);
                    if ( ! m_xmlReader->hasError()) {
                        
                    }
                }
                else {
                    m_unexpectedXmlElements.insert(m_xmlReader->name().toString());
                    m_xmlReader->skipCurrentElement();
                }
                break;
            case QXmlStreamReader::EndElement:
                if (m_xmlReader->name() == ELEMENT_SCENE) {
                    endElementFound = true;
                }
                break;
            default:
                break;
        }
    }
    
    if (m_xmlReader->hasError()) {
        return NULL;
    }
    
    imageFileName    = makeAbsoluteFilePath(imageFileName);
    distanceFileName = makeAbsoluteFilePath(distanceFileName);
    HistologySliceImage* image(new HistologySliceImage(sceneName,
                                                       imageFileName,
                                                       distanceFileName,
                                                       scaledToPlaneMatrix,
                                                       scaledToPlaneMatrixValidFlag));
    return image;
}

/**
 * Read a matrix from the current element's text
 * @param elementName
 *    Name of element
 * @param matrixType
 *    Matrix type
 * @param matrixOut
 *    Output containing matrix.
 */
void
HistologySlicesFileXmlStreamReader::readMatrixFromElementText(const QString& elementName,
                                                      const MatrixType matrixType,
                                                      Matrix4x4& matrixOut)
{
    const QString text(m_xmlReader->readElementText().trimmed());
    std::vector<float> matrixVector;
    AString::toNumbers(text, matrixVector);
    
    matrixOut.identity();
    
    if (matrixVector.size() == 9) {
        switch (matrixType) {
            case MatrixType::TWO_DIM:
                matrixOut.setMatrixElement(0, 0, matrixVector[0]);
                matrixOut.setMatrixElement(0, 1, matrixVector[1]);
                matrixOut.setMatrixElement(0, 2, 0.0);
                matrixOut.setMatrixElement(0, 3, matrixVector[2]);
                
                matrixOut.setMatrixElement(1, 0, matrixVector[3]);
                matrixOut.setMatrixElement(1, 1, matrixVector[4]);
                matrixOut.setMatrixElement(1, 2, 0.0);
                matrixOut.setMatrixElement(1, 3, matrixVector[5]);
                
                matrixOut.setMatrixElement(2, 0, matrixVector[6]);
                matrixOut.setMatrixElement(2, 1, matrixVector[7]);
                matrixOut.setMatrixElement(2, 2, 1.0);
                matrixOut.setMatrixElement(2, 3, matrixVector[8]);
                
                matrixOut.setMatrixElement(3, 0, 0.0);
                matrixOut.setMatrixElement(3, 1, 0.0);
                matrixOut.setMatrixElement(3, 2, 0.0);
                matrixOut.setMatrixElement(3, 3, 1.0);
                break;
            case MatrixType::THREE_DIM:
                for (int32_t iRow = 0; iRow < 3; iRow++) {
                    for (int32_t jCol = 0; jCol < 3; jCol++) {
                        const int32_t indx((iRow * 3) + jCol);
                        matrixOut.setMatrixElement(iRow, jCol,
                                                   matrixVector[indx]);
                    }
                }
                matrixOut.setMatrixElement(3, 3, 1.0);
                break;
        }
    }
    else {
        m_xmlReader->raiseError("Elment "
                                + elementName
                                + " does not contain 9 values for matrix.  Text: "
                                + text);
                                
    }
}

