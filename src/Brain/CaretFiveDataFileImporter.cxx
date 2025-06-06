
/*LICENSE_START*/
/*
 *  Copyright (C) 2025 Washington University School of Medicine
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

#define __CARET_FIVE_DATA_FILE_IMPORTER_DECLARE__
#include "CaretFiveDataFileImporter.h"
#undef __CARET_FIVE_DATA_FILE_IMPORTER_DECLARE__

#include <QDataStream>
#include <QDir>
#include <QFile>
#include <QXmlStreamReader>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretFiveFileTypeEnum.h"
#include "DataFileException.h"
#include "FileInformation.h"
#include "FociFile.h"
#include "Focus.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "GiftiMetaData.h"
#include "LabelFile.h"
#include "MetricFile.h"
#include "SpecFile.h"
#include "Surface.h"
#include "SurfaceProjectionBarycentric.h"
#include "SurfaceProjectionVanEssen.h"

using namespace caret;


    
/**
 * \class caret::CaretFiveDataFileImporter 
 * \brief Import Caret5 data files into Workbench format
 * \ingroup Files
 */

/**
 * Constructor.
 */
CaretFiveDataFileImporter::CaretFiveDataFileImporter()
: CaretObject()
{
    m_areaColorsLabelTable.reset(new GiftiLabelTable());
    m_cellColorsLabelTable.reset(new GiftiLabelTable());
    m_contourCellColorsLabelTable.reset(new GiftiLabelTable());
    m_fociColorsLabelTable.reset(new GiftiLabelTable());
}

/**
 * Destructor.
 */
CaretFiveDataFileImporter::~CaretFiveDataFileImporter()
{
    /*
     * If caller did not 'take' the files,
     * we need to delete them.
     */
    for (CaretDataFile* cdf : m_importedFiles) {
        delete cdf;
    }
    m_importedFiles.clear();
}

/**
 * @return Take the files that were imported.  Caller is responsible for
 * destroying the returned files.  This can be called one time.
 */
std::vector<CaretDataFile*> 
CaretFiveDataFileImporter::takeImportedFiles()
{
    /*
     * Place surface and volume files before any
     * other types of files
     */
    std::vector<CaretDataFile*> structureFiles;
    std::vector<CaretDataFile*> dataFiles;
    for (CaretDataFile* cdf : m_importedFiles) {
        if ((cdf->getDataFileType() == DataFileTypeEnum::SURFACE)
            || (cdf->getDataFileType() == DataFileTypeEnum::VOLUME)) {
            structureFiles.push_back(cdf);
        }
        else {
            dataFiles.push_back(cdf);
        }
    }
    std::vector<CaretDataFile*> filesOut(structureFiles.begin(),
                                         structureFiles.end());
    filesOut.insert(filesOut.end(),
                    dataFiles.begin(),
                    dataFiles.end());
    
    m_importedFiles.clear();
    
    return filesOut;
}

/**
 * @return True if there were errors
 * Even though there were errors files may have been converted successfully.
 */
bool
CaretFiveDataFileImporter::hasErrors() const
{
    return ( ! m_errorMessage.isEmpty());
}

/**
 * @return Errors during file import.
 */
AString
CaretFiveDataFileImporter::getErrorMessage() const
{
    return m_errorMessage;
}

/**
 * @return True if there were warnings
 */
bool
CaretFiveDataFileImporter::hasWarnings() const
{
    return ( ! m_warningMessage.isEmpty());
}

/**
 * @return Warnings during file import.
 */
AString
CaretFiveDataFileImporter::getWarningMessage() const
{
    return m_warningMessage;
}

/**
 * Open a file and return the file encoding
 * @param filename
 *    Name of file
 * @param fileOut
 *    File object that is return open if successful
 * @param encodingOut
 *    Ouput with encoding if sucessful
 * @return
 *    True if sucessful, else false.
 */
bool
CaretFiveDataFileImporter::openFileAndGetEncoding(const AString& filename,
                                                  QFile& fileOut,
                                                  FileEncoding& encodingOut)
{
    encodingOut = FileEncoding::INVALID;
    
    bool fileTypeValidFlag(false);
    const CaretFiveFileTypeEnum::Enum caretFiveFileType(CaretFiveFileTypeEnum::fromFilename(filename,
                                                                                            &fileTypeValidFlag));
    
    if (caretFiveFileType == CaretFiveFileTypeEnum::INVALID) {
        addError(filename,
                 "Filename extension does not match a caret file type");
        return false;
    }
    
    fileOut.setFileName(filename);
    if ( ! fileOut.exists()) {
        addError(filename,
                 "File does not exist");
        return false;
    }
    if ( ! fileOut.open(QFile::ReadOnly)) {
        addError(filename, "Unable to open file for reading");
        return false;
    }
    
    FunctionResultValue<FileEncoding> encodingResult(determineFileEncoding(fileOut));
    if (encodingResult.isError()) {
        addError(filename,
                 encodingResult.getErrorMessage());
        return false;
    }
    
    encodingOut = encodingResult.getValue();

    /*
     * Return to beginning of file
     */
    fileOut.seek(0);
    
    return true;
}


/**
 * Import a Caret5 data file and return its Workbench equivalent
 * @param outputDirectory
 *    Output dfirectory for new files
 * @param convertedFileNamePrefix
 *    Prefix that is added to the name of converted data files
 * @param filename
 *    Name of data file
 * @param auxiliaryFilename
 *    Name of auxiliary file.  May be empty.
 * @return
 *    Function result with Caret6 file or error.
 *
 * Some files need have an auxilieary file that is needed for successful conversion.
 *   All border, cell, and foci files need a color file.
 *   Coordinate file needs a topology file.
 */
void
CaretFiveDataFileImporter::importFile(const AString& outputDirectory,
                                      const AString& convertedFileNamePrefix,
                                      const AString& filename,
                                      const AString& auxiliaryFilename)
{
    m_outputDirectory         = outputDirectory.trimmed();
    m_convertedFileNamePrefix = convertedFileNamePrefix;
    
    bool filenameValidFlag(false);
    const CaretFiveFileTypeEnum::Enum caretFiveFileType(CaretFiveFileTypeEnum::fromFilename(filename, &filenameValidFlag));
    
    if (caretFiveFileType == CaretFiveFileTypeEnum::INVALID) {
        addError(filename,
                 "Filename extension does not match a caret file type");
        return;
    }

    importDataFile(caretFiveFileType,
                   StructureEnum::INVALID,
                   filename,
                   auxiliaryFilename);
}

/**
 * Import a data file
 * @param caretFiveFileType
 *    Type of Caret5 file
 * @param structure
 *    Structure needed by some files
 * @param filename
 *    Name of file
 * @param auxiliaryFilename
 *    Name of auxiliary file.
 * @return
 *    True if successful, else false
 *
 * Some files need have an auxilieary file that is needed for successful conversion.
 *   All border, cell, and foci files need a color file.
 *   Coordinate file needs a topology file.
 */
bool
CaretFiveDataFileImporter::importDataFile(const CaretFiveFileTypeEnum::Enum caretFiveFileType,
                                          const StructureEnum::Enum structure,
                                          const AString& filename,
                                          const AString& auxiliaryFilename)
{
    QFile file;
    FileEncoding fileEncoding = FileEncoding::INVALID;
    if ( ! openFileAndGetEncoding(filename,
                                  file,
                                  fileEncoding)) {
        return false;
    }
    
    bool conversionNotSupportedFlag(false);
    
    FunctionResultValue<CaretDataFile*> convertResult(NULL,
                                                      ("Conversion not implemented for Caret5 file type "
                                                       + CaretFiveFileTypeEnum::toGuiName(caretFiveFileType)),
                                                      false);
    switch (caretFiveFileType) {
        case CaretFiveFileTypeEnum::INVALID:
            CaretAssert(0);
            conversionNotSupportedFlag = true;
            break;
        case CaretFiveFileTypeEnum::AREACOLOR:
            addWarning(filename,
                       "Area Color Files not used by Workbench.  "
                       "Conversion of Paint Files will attempt to use Area Color Files during Paint File conversion.");
            file.close();
            return false;
            break;
        case CaretFiveFileTypeEnum::AREALESTIMATION:
            conversionNotSupportedFlag = true;
            break;
        case CaretFiveFileTypeEnum::BORDERCOLOR:
            addWarning(filename,
                       "Border Color Files not used by Workbench.  "
                       "Conversion of Border Files will attempt to use Border Color Files during Border File conversion.");
            file.close();
            return false;
            break;
        case CaretFiveFileTypeEnum::BORDER:
            conversionNotSupportedFlag = true;
            break;
        case CaretFiveFileTypeEnum::BORDERPROJECTION:
            conversionNotSupportedFlag = true;
            break;
        case CaretFiveFileTypeEnum::CELLCOLOR:
            addWarning(filename,
                       "Cell Color Files not used by Workbench.  "
                       "Conversion of Cell Files will attempt to use Cell Color Files during Cell File conversion.");
            file.close();
            return false;
            break;
        case CaretFiveFileTypeEnum::CELL:
            convertResult = convertCellOrFociFile(caretFiveFileType,
                                                  fileEncoding,
                                                  file,
                                                  auxiliaryFilename);
            break;
        case CaretFiveFileTypeEnum::CELLPROJECTION:
            convertResult = convertCellOrFociProjectionFile(caretFiveFileType,
                                                            fileEncoding,
                                                            file,
                                                            auxiliaryFilename);
            break;
        case CaretFiveFileTypeEnum::CONTOURCELLCOLOR:
            addWarning(filename,
                       "Contour Cell Color Files not used by Workbench.  "
                       "Conversion of Contour Cell Files will attempt to use Contour Cell Color Files during Contour Cell File conversion.");
            file.close();
            return false;
            break;
        case CaretFiveFileTypeEnum::CONTOURCELL:
            /*
             * Note: conversion of the contour cell file will work
             * (file format is same as cell file) but Workbench
             * does not support contours so there is no way to
             * display the contour cells.
             *
             * convertResult = convertCellOrFociFile(caretFiveFileType,
             *                                       fileEncoding,
             *                                       file,
             *                                       auxiliaryFilename);
             */
            conversionNotSupportedFlag = true;
            break;
        case CaretFiveFileTypeEnum::CONTOUR:
            conversionNotSupportedFlag = true;
            break;
        case CaretFiveFileTypeEnum::COORDINATE:
            convertResult = convertCoordinateFile(caretFiveFileType,
                                                  fileEncoding,
                                                  file,
                                                  auxiliaryFilename);
            break;
        case CaretFiveFileTypeEnum::FOCICOLOR:
            addWarning(filename,
                       "Foci Color Files not used by Workbench.  "
                       "Conversion of Foci Files will attempt to use Foci Color Files during Foci File conversion.");
            file.close();
            return false;
            break;
        case CaretFiveFileTypeEnum::FOCI:
            convertResult = convertCellOrFociFile(caretFiveFileType,
                                                  fileEncoding,
                                                  file,
                                                  auxiliaryFilename);
            break;
        case CaretFiveFileTypeEnum::FOCIPROJECTION:
            convertResult = convertCellOrFociProjectionFile(caretFiveFileType,
                                                            fileEncoding,
                                                            file,
                                                            auxiliaryFilename);
            break;
        case CaretFiveFileTypeEnum::LATLON:
            conversionNotSupportedFlag = true;
            break;
        case CaretFiveFileTypeEnum::MATRIX:
            conversionNotSupportedFlag = true;
            break;
        case CaretFiveFileTypeEnum::METRIC:
            convertResult = convertMetricFile(fileEncoding,
                                              structure,
                                              true, /* metric file */
                                              file);
            break;
        case CaretFiveFileTypeEnum::PAINT:
            conversionNotSupportedFlag = true;
            break;
        case CaretFiveFileTypeEnum::SECTION:
            conversionNotSupportedFlag = true;
            break;
        case CaretFiveFileTypeEnum::SHAPE:
            /* Shape and metric are same format */
            convertResult = convertMetricFile(fileEncoding,
                                              structure,
                                              false, /* shape file */
                                              file);
            break;
        case CaretFiveFileTypeEnum::SPEC:
            convertResult = convertSpecFile(fileEncoding,
                                            file);
            break;
        case CaretFiveFileTypeEnum::TOPOLOGY:
            addWarning(filename,
                       "Topology Files not used by Workbench.  "
                       "Conversion of Coordinate Files will attempt to use Topology Files during Coordinate File conversion.");
            break;
        case CaretFiveFileTypeEnum::VOLUME:
            conversionNotSupportedFlag = true;
            break;
    }

    if (conversionNotSupportedFlag) {
        addWarningUnsupportedFileType(filename);
        return false;
    }
    
    if (convertResult.isOk()) {
        CaretDataFile* cdf(convertResult.getValue());
        if (cdf != NULL) {
            setOutputFileName(cdf, file);
            m_importedFiles.push_back(cdf);
        }
        return true;
    }
    else {
        addError(filename,
                 convertResult.getErrorMessage());
    }
    return false;
}

/**
 * Caret format data files are in Caret ASCII format, Caret Binary format, or Caret XML
 * Caret also supported GIFTI, NIFTI, AFNI, Analyze and other formats.
 * @return Encoding of the caret5 data file
 * @param file
 *    QFile for reading data from file
 */
FunctionResultValue<CaretFiveDataFileImporter::FileEncoding>
CaretFiveDataFileImporter::determineFileEncoding(QFile& file) const
{
    
    if (file.fileName().endsWith(".gii")) {
        return FunctionResultValue<FileEncoding>(FileEncoding::GIFTI);
    }
    else if (file.fileName().endsWith(".nii")
             || file.fileName().endsWith(".nii.gz")
             || file.fileName().endsWith(".hdr")
             || file.fileName().endsWith(".HEAD")) {
        return FunctionResultValue<FileEncoding>(FileEncoding::VOLUME);
    }
    const int64_t maxLineLength(100);
    const QByteArray firstBytes(file.readLine(maxLineLength));
    const QString firstLine(firstBytes);
    
    if (firstLine.startsWith("BeginHeader")) {
        bool done(false);
        while ( ! done) {
            if (file.atEnd()) {
                return FunctionResultValue<FileEncoding>(FileEncoding::INVALID,
                                                         "Error reading file, did  not find 'EndHeader' caret ascii/binary file",
                                                         false);
            }
            
            const std::pair<AString, AString> nameValue(readCaretHeaderLine(file, 0));
            const AString name(nameValue.first);
            const AString value(nameValue.second);
            if (name == "encoding") {
                if (value == "ASCII") {
                    return FunctionResultValue<FileEncoding>(FileEncoding::ASCII);
                }
                else if (value == "BINARY") {
                    return FunctionResultValue<FileEncoding>(FileEncoding::BINARY);
                }
                return FunctionResultValue<FileEncoding>(FileEncoding::INVALID,
                                                         "Invalid encoding=" + value,
                                                         false);
            }
            else if (name == "EndHeader") {
                /*
                 * If encoding was not found, assume ASCII
                 */
                return FunctionResultValue<FileEncoding>(FileEncoding::ASCII);
            }
        }
    }
    else if (firstLine.startsWith("<?xml")) {
        return FunctionResultValue<FileEncoding>(FileEncoding::XML);
    }
    
    return FunctionResultValue<FileEncoding>(FileEncoding::INVALID,
                                             "Unrecognized file enconding, first line in file: "  + firstLine,
                                             false);
}

/**
 * Read a Caret Five file header into GIFTI metadata
 * @return
 *    Gifti Metadata containing header data
 * @param file
 *    The file
 */
FunctionResultValue<GiftiMetaData*>
CaretFiveDataFileImporter::readCaretFileHeader(QFile& file)
{
    const int32_t maxLength(0); /* zero is no maximum length */
    
    GiftiMetaData* metaData(new GiftiMetaData());
    
    bool done(false);
    while ( ! done) {
        if (file.atEnd()) {
            done = true;
        }
        else {
            std::pair<AString, AString> line(readCaretHeaderLine(file, maxLength));
            if (line.first == "EndHeader") {
                done = true;
            }
            else {
                metaData->set(line.first,
                              line.second);
            }
        }
    }
    
    return FunctionResultValue<GiftiMetaData*>(metaData);
}

/**
 * Read one line from a Caret format file header
 * @param file
 *    The file
 * @param maxLength
 *    Maximum number of characters to read (zero is unlimited)
 * @return
 *    A pair with the name of the header element and its value
 */
std::pair<AString, AString>
CaretFiveDataFileImporter::readCaretHeaderLine(QFile& file,
                                               const int32_t maxLength) const
{
    AString name, value;
    
    const QByteArray bytes(file.readLine(maxLength));
    const QString line(bytes);
    const int32_t separatorIndex(line.indexOf(" "));
    if (separatorIndex > 0) {
        name = line.left(separatorIndex).trimmed();
        value = line.mid(separatorIndex + 1).trimmed();
    }
    else {
        name = line.trimmed();
    }
    
    return std::make_pair(name, value);
}

/**
 * Convert a Caret5 cell, contour cell, or foci file to a Workbench foci file
 * @param fileType,
 *    Type of Caret5 file
 * @param fileEncoding
 *    The Caret5 file encoding
 * @param file
 *    The file
 * @param colorFileNameHint
 *    Hint at name of file containing colors
 */
FunctionResultValue<CaretDataFile*>
CaretFiveDataFileImporter::convertCellOrFociFile(const CaretFiveFileTypeEnum::Enum fileType,
                                                 const FileEncoding fileEncoding,
                                                 QFile& file,
                                                 const AString& colorFileNameHint)
{
    std::unique_ptr<FociFile> fociFile(new FociFile());
    
    std::unique_ptr<GiftiLabelTable> colorsTable(new GiftiLabelTable());
    
    switch (fileType) {
        case CaretFiveFileTypeEnum::CELL:
            *colorsTable = *m_cellColorsLabelTable.get();
            break;
        case CaretFiveFileTypeEnum::CONTOURCELL:
            *colorsTable = *m_contourCellColorsLabelTable.get();
            break;
        case CaretFiveFileTypeEnum::FOCI:
            *colorsTable = *m_fociColorsLabelTable.get();
            break;
        default:
            return error("Unsupported file type for reading cell/foci: "
                         + file.fileName());
            break;
    }

    AString errorMessage;
    if (fileEncoding == XML) {
        AString fileTag;
        switch (fileType) {
            case CaretFiveFileTypeEnum::CELL:
                fileTag = "Cell_File";
                break;
            case CaretFiveFileTypeEnum::CONTOURCELL:
                fileTag = "Contour_Cell_File";
                break;
            case CaretFiveFileTypeEnum::FOCI:
                fileTag = "Foci_File";
                break;
            default:
                return error("Unsupported file type for reading cell/foci: "
                             + file.fileName());
                break;
        }
        
        QXmlStreamReader xml(&file);
        
         if (xml.readNextStartElement()) {
            if (xml.name() == fileTag) {
                xml.readNext();
                    while ( ( ! xml.atEnd())
                           && ( ! xml.hasError())) {
                        if (xml.isStartElement()) {
                            if (xml.name() == "FileHeader") {
                                FunctionResult result(readXmlFileHeader(xml, 
                                                                        fociFile.get()));
                                if (result.isError()) {
                                    return error("Unsupported file type for reading cell/foci: "
                                                 + file.fileName());
                                }
                            }
                            else if (xml.name() == "CellData") {
                                FunctionResult result(readXmlCellOrFocus(xml,
                                                                         fociFile.get()));
                                if (result.isError()) {
                                    return error("Unsupported file type for reading cell/foci: "
                                                 + file.fileName());
                                }
                            }
                            else if (xml.name() == "CellStudyInfo") {
                                /* Ignore this content */
                                xml.skipCurrentElement();
                            }
                        }
                        else if (xml.isEndElement()) {
                        }
                        
                        xml.readNext();
                    }
            }
            else {
                xml.raiseError("First XML tag should be "
                               + fileTag
                               + " but is "
                               + xml.name().toString()
                               + " for file "
                               + file.fileName());
            }
        }
        
        if (xml.hasError()) {
            errorMessage = xml.errorString();
        }
        else {
            const bool haveValidColorsFlag((colorsTable != NULL)
                                           ? (colorsTable->getNumberOfLabels() > 0)
                                           : false);
            if ( ! haveValidColorsFlag) {
                AString colorFileName(colorFileNameHint);
                FunctionResultString colorFileResult(findAuxiliaryFile(fileType,
                                                                       file.fileName(),
                                                                       colorFileName));
                if (colorFileResult.isOk()) {
                    FunctionResult readResult(readColorFile(colorFileResult.getValue(),
                                                                 fileType,
                                                                 colorsTable.get()));

                    if (readResult.isOk()) {
                    }
                    else {
                        addWarning(colorFileResult.getValue(),
                                   "Failed to read this color file for cell/foci file import: "
                                   + readResult.getErrorMessage());
                    }
                }
                else {
                }
            }
            
            /*
             * Copy to foci name label table
             */
            GiftiLabelTable* fociNameLabelTable(fociFile->getNameColorTable());
            CaretAssert(fociNameLabelTable);
            *fociNameLabelTable = *colorsTable;
            
            /*
             * Copy to foci class color table
             */
            GiftiLabelTable* fociClassLabelTable(fociFile->getClassColorTable());
            CaretAssert(fociClassLabelTable);
            *fociClassLabelTable = *colorsTable;

            return FunctionResultValue<CaretDataFile*>(fociFile.release(), "", true);
        }
    }
    else {
        errorMessage = "Only XML format Cell/Foci files are convertible at this time";
    }
    
    return error(errorMessage);
}

/**
 * Convert a Caret5 cell, contour cell, or foci projection file to a Workbench foci file
 * @param fileType,
 *    Type of Caret5 file
 * @param fileEncoding
 *    The Caret5 file encoding
 * @param file
 *    The file
 * @param colorFileNameHint
 *    Hint at name of file containing colors
 */
FunctionResultValue<CaretDataFile*>
CaretFiveDataFileImporter::convertCellOrFociProjectionFile(const CaretFiveFileTypeEnum::Enum fileType,
                                                           const FileEncoding fileEncoding,
                                                           QFile& file,
                                                           const AString& colorFileNameHint)
{
    std::unique_ptr<FociFile> fociFile(new FociFile());
    
    std::unique_ptr<GiftiLabelTable> colorsTable(new GiftiLabelTable());
    
    switch (fileType) {
        case CaretFiveFileTypeEnum::CELLPROJECTION:
            *colorsTable = *m_cellColorsLabelTable.get();
            break;
        case CaretFiveFileTypeEnum::FOCIPROJECTION:
            *colorsTable = *m_fociColorsLabelTable.get();
            break;
        default:
            return error("Unsupported file type for reading cell/foci: "
                         + file.fileName());
            break;
    }
    
    AString errorMessage;
    if (fileEncoding == XML) {
        AString fileTag;
        switch (fileType) {
            case CaretFiveFileTypeEnum::CELLPROJECTION:
                fileTag = "Cell_Projection_File";
                break;
            case CaretFiveFileTypeEnum::FOCIPROJECTION:
                fileTag = "Foci_Projection_File";
                break;
            default:
                return error("Unsupported file type for reading cell/foci projection: "
                             + file.fileName());
                break;
        }
        
        QXmlStreamReader xml(&file);
        
        if (xml.readNextStartElement()) {
            if (xml.name() == fileTag) {
                xml.readNext();
                while ( ( ! xml.atEnd())
                       && ( ! xml.hasError())) {
                    if (xml.isStartElement()) {
                        if (xml.name() == "FileHeader") {
                            FunctionResult result(readXmlFileHeader(xml,
                                                                    fociFile.get()));
                            if (result.isError()) {
                                return error("Unsupported file type for reading cell/foci: "
                                             + file.fileName());
                            }
                        }
                        else if (xml.name() == "CellProjection") {
                            FunctionResult result(readXmlCellOrFocusProjection(xml,
                                                                               fociFile.get()));
                            if (result.isError()) {
                                return error("Unsupported file type for reading cell/foci: "
                                             + file.fileName());
                            }
                        }
                        else if (xml.name() == "CellStudyInfo") {
                            /* Ignore this content */
                            xml.skipCurrentElement();
                        }
                    }
                    else if (xml.isEndElement()) {
                    }
                    
                    xml.readNext();
                }
            }
            else {
                xml.raiseError("First XML tag should be "
                               + fileTag
                               + " but is "
                               + xml.name().toString()
                               + " for file "
                               + file.fileName());
            }
        }
        
        if (xml.hasError()) {
            errorMessage = xml.errorString();
        }
        else {
            const bool haveValidColorsFlag((colorsTable != NULL)
                                           ? (colorsTable->getNumberOfLabels() > 0)
                                           : false);
            if ( ! haveValidColorsFlag) {
                AString colorFileName(colorFileNameHint);
                FunctionResultString colorFileResult(findAuxiliaryFile(fileType,
                                                                       file.fileName(),
                                                                       colorFileName));
                if (colorFileResult.isOk()) {
                    FunctionResult readResult(readColorFile(colorFileResult.getValue(),
                                                            fileType,
                                                            colorsTable.get()));
                    
                    if (readResult.isOk()) {
                    }
                    else {
                        addWarning(colorFileResult.getValue(),
                                   "Failed to read this color file for cell/foci file import: "
                                   + readResult.getErrorMessage());
                    }
                }
                else {
                }
            }
            
            /*
             * Copy to foci name label table
             */
            GiftiLabelTable* fociNameLabelTable(fociFile->getNameColorTable());
            CaretAssert(fociNameLabelTable);
            *fociNameLabelTable = *colorsTable;
            
            /*
             * Copy to foci class color table
             */
            GiftiLabelTable* fociClassLabelTable(fociFile->getClassColorTable());
            CaretAssert(fociClassLabelTable);
            *fociClassLabelTable = *colorsTable;
            
            return FunctionResultValue<CaretDataFile*>(fociFile.release(), "", true);
        }
    }
    else {
        errorMessage = "Only XML format Cell/Foci files are convertible at this time";
    }
    
    return error(errorMessage);
}

/**
 * Read a Caret5 cell, contour cell color, or foci color file to a GIFTI label file
 * @param fileType,
 *    Type of Caret5 file
 * @param fileEncoding
 *    The Caret5 file encoding
 * @param file
 *    The file
 */
FunctionResultValue<CaretDataFile*>
CaretFiveDataFileImporter::readColorFileIntoLabelFile(const CaretFiveFileTypeEnum::Enum fileType,
                                                      const FileEncoding fileEncoding,
                                                      QFile& file)
{
    std::unique_ptr<LabelFile> labelFile(new LabelFile());
    GiftiLabelTable* labelTable(labelFile->getLabelTable());
    
    AString errorMessage;
    if (fileEncoding == XML) {
        AString fileTag;
        switch (fileType) {
            case CaretFiveFileTypeEnum::CELLCOLOR:
                fileTag = "Cell_Color_File";
                break;
            case CaretFiveFileTypeEnum::CONTOURCELLCOLOR:
                fileTag = "Contour_Cell_Color_File";
                break;
            case CaretFiveFileTypeEnum::FOCICOLOR:
                fileTag = "Foci_Color_File";
                break;
            default:
                return error("Unsupported file type for reading cell/foci colors: "
                             + file.fileName());
                break;
        }
        
        QXmlStreamReader xml(&file);
        
        if (xml.readNextStartElement()) {
            if (xml.name() == fileTag) {
                xml.readNext();
                while ( ( ! xml.atEnd())
                       && ( ! xml.hasError())) {
                    if (xml.isStartElement()) {
                        if (xml.name() == "FileHeader") {
                            /*
                             * No metadata for a lable table so skip header
                             */
                            xml.skipCurrentElement();
                        }
                        else if (xml.name() == "Color") {
                            FunctionResult result(readXmlColor(xml,
                                                               labelTable));
                            if (result.isError()) {
                                return error("Unsupported file type for reading cell/foci: "
                                             + file.fileName());
                            }
                        }
                    }
                    else if (xml.isEndElement()) {
                    }
                    
                    xml.readNext();
                }
            }
            else {
                xml.raiseError("First XML tag should be "
                               + fileTag
                               + " but is "
                               + xml.name().toString()
                               + " for file "
                               + file.fileName());
            }
        }
        
        if (xml.hasError()) {
            errorMessage = xml.errorString();
        }
        else {
            return FunctionResultValue<CaretDataFile*>(labelFile.release(), "", true);
        }
    }
    else {
        errorMessage = "Only XML format Cell/Foci Color _files are convertible at this time";
    }
    
    return error(errorMessage);
}

/**
 * Read content of a color file into a label table
 * @param filename
 *    Name of the file
 * @param fileType
 *    Type of file
 * @param labelTable
 *    The label table
 */
FunctionResult
CaretFiveDataFileImporter::readColorFile(const AString& filename,
                                         const CaretFiveFileTypeEnum::Enum fileType,
                                         GiftiLabelTable* labelTable)
{
    CaretAssert(labelTable);
    
    QFile file(filename);
    if ( ! file.exists()) {
        return FunctionResult::error("Color file does not exist: "
                                     + filename);
    }
    if ( ! file.open(QFile::ReadOnly)) {
        return FunctionResult::error("Unable to open color file for reading: "
                                     + filename);
    }
    
    FunctionResultValue<FileEncoding> encodingResult(determineFileEncoding(file));
    if (encodingResult.isError()) {
        FunctionResult::error(encodingResult.getErrorMessage());
    }
    
    const FileEncoding fileEncoding(encodingResult.getValue());
    
    /*
     * Return to beginning of file
     */
    file.seek(0);
    AString errorMessage;
    if (fileEncoding == XML) {
        AString fileTag;
        switch (fileType) {
            case CaretFiveFileTypeEnum::CELLCOLOR:
                fileTag = "Cell_Color_File";
                break;
            case CaretFiveFileTypeEnum::CONTOURCELLCOLOR:
                fileTag = "Contour_Cell_Color_File";
                break;
            case CaretFiveFileTypeEnum::FOCICOLOR:
                fileTag = "Foci_Color_File";
                break;
            default:
                return error("Unsupported file type for reading cell/foci colors: "
                             + file.fileName());
                break;
        }
        
        QXmlStreamReader xml(&file);
        
        if (xml.readNextStartElement()) {
            if (xml.name() == fileTag) {
                xml.readNext();
                while ( ( ! xml.atEnd())
                       && ( ! xml.hasError())) {
                    if (xml.isStartElement()) {
                        if (xml.name() == "FileHeader") {
                            /*
                             * No metadata for a lable table so skip header
                             */
                            xml.skipCurrentElement();
                        }
                        else if (xml.name() == "Color") {
                            FunctionResult result(readXmlColor(xml,
                                                               labelTable));
                            if (result.isError()) {
                                return error("Unsupported file type for reading cell/foci: "
                                             + file.fileName());
                            }
                        }
                    }
                    else if (xml.isEndElement()) {
                    }
                    
                    xml.readNext();
                }
            }
            else {
                xml.raiseError("First XML tag should be "
                               + fileTag
                               + " but is "
                               + xml.name().toString()
                               + " for file "
                               + file.fileName());
            }
        }
        
        if (xml.hasError()) {
            errorMessage = xml.errorString();
        }
        else {
            return FunctionResult::ok();
        }
    }
    else {
        errorMessage = "Only XML format Cell/Foci Color _files are convertible at this time";
    }
    return FunctionResult::error("readColorFile not finished");
}


/**
 * Convert a Caret5 Coordinate file to a Workbench surface file
 * @param fileType,
 *    Type of Caret5 file
 * @param fileEncoding
 *    The Caret5 file encoding
 * @param file
 *    The file
 * @param topologyFileNameHint
 *    Hint at name of file containing topology
 */
FunctionResultValue<CaretDataFile*>
CaretFiveDataFileImporter::convertCoordinateFile(const CaretFiveFileTypeEnum::Enum fileType,
                                                 const FileEncoding fileEncoding,
                                                 QFile& file,
                                                 const AString& topologyFileNameHint)
{
    std::unique_ptr<Surface> surface(new Surface());
    
    AString errorMessage;
    if (fileEncoding == BINARY) {
        FunctionResultValue<GiftiMetaData*> headerResult(readCaretFileHeader(file));
        if (headerResult.isError()) {
            return error(headerResult.getErrorMessage());
        }
        
        std::unique_ptr<GiftiMetaData> metadata(headerResult.getValue());
        
        AString topoFileName(topologyFileNameHint);
        if (topoFileName.isEmpty()) {
            topoFileName = metadata->get("topo_file");
        }
        const FunctionResultString topoNameResult(findAuxiliaryFile(fileType, file.fileName(), topoFileName));
        if (topoNameResult.isOk()) {
            topoFileName = topoNameResult.getValue();
        }
        if (topoFileName.isEmpty()) {
            return error("No topology file found for coordinate file "
                         + file.fileName());
        }
        
        StructureEnum::Enum structure(StructureEnum::INVALID);
        const AString structureName(metadata->get("structure"));
        if (structureName.isEmpty()) {
            return error("No structure in coordinate file "
                         + file.fileName());
        }
        else if (structureName.toLower().contains("left")) {
            structure = StructureEnum::CORTEX_LEFT;
        }
        else if (structureName.toLower().contains("right")) {
            structure = StructureEnum::CORTEX_RIGHT;
        }
        else {
            return error("Structure is neither left nor right in coord file: "
                         + file.fileName());
        }
        std::vector<int32_t> triangles;
        const FunctionResult topoResult(readTopologyFile(topoFileName, triangles));
        if (topoResult.isError()) {
            return error(topoResult.getErrorMessage());
        }
        const int32_t numTriangles(triangles.size() / 3);
        
        QDataStream dataStream(&file);
        dataStream.setVersion(QDataStream::Qt_4_3);
        int numCoords;
        dataStream >> numCoords;
        if (numCoords > 0) {
            surface->setNumberOfNodesAndTriangles(numCoords, numTriangles);
            surface->setStructure(structure);
            surface->getFileMetaData()->set("Imported Caret 5 Coord", file.fileName());
            surface->getFileMetaData()->set("Imported Caret 5 Topo", topoFileName);
            
            for (int i = 0; i < numCoords; i++) {
                float x, y, z;
                dataStream >> x >> y >> z;
                surface->setCoordinate(i, x, y, z);
            }
            
            const int32_t numTriangles(triangles.size() / 3);
            for (int32_t i = 0; i < numTriangles; i++) {
                const int32_t i3(i * 3);
                surface->setTriangle(i, triangles[i3], triangles[i3+1], triangles[i3+2]);
            }
            
            surface->computeNormals();
            surface->flipNormals();

            return FunctionResultValue<CaretDataFile*>(surface.release(),
                                                       "",
                                                       true);
        }
        else {
            error("Coord file contains no coordinates: "
                  + file.fileName());
        }
    }

    return error("Encoding "
                 + toFileEncodingName(fileEncoding)
                 + " not supported for "
                 + file.fileName());
}

/**
 * Convert a Caret5 Metric file to a Workbench GIFTI Functional file
 * @param fileEncoding
 *    The Caret5 file encoding
 * @param structure
 *    Structure for which metric file is being read
 * @param metricFlag
 *    True if converting metric file, else shape file
 * @param file
 *    The file
 */
FunctionResultValue<CaretDataFile*>
CaretFiveDataFileImporter::convertMetricFile(const FileEncoding fileEncoding,
                                             const StructureEnum::Enum structure,
                                             const bool metricFlag,
                                             QFile& file)
{
    std::unique_ptr<MetricFile> metricFile(new MetricFile());
    
    AString errorMessage;
    if (fileEncoding == BINARY) {
        FunctionResultValue<GiftiMetaData*> headerResult(readCaretFileHeader(file));
        if (headerResult.isError()) {
            return error(headerResult.getErrorMessage());
        }
        
        std::unique_ptr<GiftiMetaData> metadata(headerResult.getValue());

        metricFile->setStructure(structure);
        
        int32_t numberOfNodes(0);
        int32_t numberOfColumns(0);
        bool done(false);
        while ( ! done) {
            const int32_t maxLength(999999);
            std::pair<AString, AString> tagLineText(readCaretHeaderLine(file, maxLength));
            const AString tagName(tagLineText.first);
            const AString value(tagLineText.second);
            if (tagName == "tag-number-of-nodes") {
                numberOfNodes = value.toInt();
            }
            else if (tagName == "tag-number-of-columns") {
                numberOfColumns = value.toInt();
            }
            else if (tagName == "tag-column-name") {
                
            }
            else if (tagName == "tag-BEGIN-DATA") {
                done = true;
            }
            
            if (file.atEnd()) {
                addError(file.fileName(),
                         "Reached end of metric file without finding \"tag-BEGIN-DATA\"");
                return FunctionResultValue<CaretDataFile*>(NULL,
                                                           "Reached end of metric file without finding \"tag-BEGIN-DATA\"",
                                                           false);
            }
        }
        
        QDataStream dataStream(&file);
        dataStream.setVersion(QDataStream::Qt_4_3);
        
        
        if ((numberOfNodes > 0)
            && (numberOfColumns > 0)) {
            metricFile->setNumberOfNodesAndColumns(numberOfNodes,
                                                   numberOfColumns);
            
            for (int32_t i = 0; i < numberOfNodes; i++) {
                for (int32_t j = 0; j < numberOfColumns; j++) {
                    float value;
                    dataStream >> value;
                    metricFile->setValue(i, j, value);
                }
            }
            
            AString paletteName;
            if (metricFlag) {
                paletteName = "ROY-BIG-BL";
            }
            else {
                paletteName = "Gray_Interp";
            }
            const int32_t numMaps(metricFile->getNumberOfMaps());
            for (int32_t i = 0; i < numMaps; i++) {
                PaletteColorMapping* pcm(metricFile->getPaletteColorMapping(i));
                pcm->setSelectedPaletteName(paletteName);
            }
            
            return FunctionResultValue<CaretDataFile*>(metricFile.release(),
                                                       "",
                                                       true);
        }
        else {
            return error("Metric file has invalid number of nodes="
                  + AString::number(numberOfNodes)
                         + " and/or number of columns="
                         + AString::number(numberOfColumns)
                         + " for "
                         + file.fileName());
        }
    }
    
    return error("Encoding "
                 + toFileEncodingName(fileEncoding)
                 + " not supported for "
                 + file.fileName());
}

/**
 * Read a topology file's triangles
 * @param filename
 *    Name of file
 * @param trianglesOut
 *    Triangles read from topology file
 * @return
 *    Result of reading topology
 */
FunctionResult
CaretFiveDataFileImporter::readTopologyFile(const AString& filename,
                                            std::vector<int32_t>& trianglesOut)
{
    trianglesOut.clear();
    
    QFile file(filename);
    if ( ! file.exists()) {
        return FunctionResult::error("Topology file does not exist: "
                                     + filename);
    }
    if ( ! file.open(QFile::ReadOnly)) {
        return FunctionResult::error("Unable to open topology file for reading: "
                                      + filename);
    }
    
    FunctionResultValue<FileEncoding> encodingResult(determineFileEncoding(file));
    if (encodingResult.isError()) {
        FunctionResult::error(encodingResult.getErrorMessage());
    }
    
    const FileEncoding fileEncoding(encodingResult.getValue());
    
    if (fileEncoding == INVALID) {
        FunctionResult::error("Encoding is unknown for topology file"
                              + filename);
    }
    
    /*
     * Return to beginning of file
     */
    file.seek(0);

    if (fileEncoding == FileEncoding::BINARY) {
        /*
         * Skip over and ignore header
         */
        FunctionResultValue<GiftiMetaData*> headerResult(readCaretFileHeader(file));
        if (headerResult.isError()) {
            return FunctionResult::error(headerResult.getErrorMessage());
        }
        std::unique_ptr<GiftiMetaData> metadata(headerResult.getValue());
        
        /*
         * First line after header should be verion of topo file as below:
         *     tag-version 1
         */
        const int32_t maxLength(1024);
        std::pair<AString, AString> versionLineText(readCaretHeaderLine(file, maxLength));
        int32_t topoFileVersion(0);
        if (versionLineText.first == "tag-version") {
            topoFileVersion = versionLineText.second.toInt();
        }
        
        switch (topoFileVersion) {
            case 1:
            {
                QDataStream dataStream(&file);
                int32_t numTriangles;
                dataStream >> numTriangles;
                if ((numTriangles > 0)
                    && (numTriangles < 9999999)) {
                    trianglesOut.reserve(numTriangles * 3);

                    for (int32_t i = 0; i < numTriangles; i++) {
                        int32_t t1, t2, t3;
                        dataStream >> t1 >> t2 >> t3;
                        trianglesOut.push_back(t1);
                        trianglesOut.push_back(t2);
                        trianglesOut.push_back(t3);
                    }

                    return FunctionResult::ok();
                }
                else {
                    return FunctionResult::error("Invalid or too many triangles="
                                                 + AString::number(numTriangles)
                                                 + " in topo file "
                                                 + file.fileName());
                }
            }
                break;
            default:
                return FunctionResult::error("Unrecongized/unsupported version="
                                             + AString::number(topoFileVersion)
                                             + " for reading topo file "
                                             + file.fileName());
        }
    }

    return unsupportedFileEncodingError(fileEncoding,
                                        file);
}

/**
 * Convert a spec file
 * @param fileEcoding
 *    Encoding of the file
 * @param file
 *    The file
 * @return
 *    FunctionResult with converted file or errort
 */
FunctionResultValue<CaretDataFile*>
CaretFiveDataFileImporter::convertSpecFile(const FileEncoding fileEncoding,
                                           QFile& file)
{
    if (fileEncoding != FileEncoding::ASCII) {
        if (fileEncoding == FileEncoding::XML) {
            QXmlStreamReader xml(&file);
            
            if (xml.readNextStartElement()) {
                if (xml.name() == "CaretSpecFile") {
                    return error("The Spec File appears to be a Workbench format spec file (not a Caret5 spec file).");
                }
            }
        }
        return unsupportedFileEncodingError(fileEncoding,
                                            file);
    }
    
    FunctionResultValue<GiftiMetaData*> headerResult(readCaretFileHeader(file));
    if (headerResult.isError()) {
        return error(headerResult.getErrorMessage());
    }
    
    std::unique_ptr<GiftiMetaData> metadata(headerResult.getValue());
    CaretAssert(metadata);
    
    StructureEnum::Enum structure = StructureEnum::INVALID;
    const AString structureName(metadata->get("structure"));
    if ( ! structureName.isEmpty()) {
        if (structureName == "right") {
            structure = StructureEnum::CORTEX_RIGHT;
        }
        else if (structureName == "left") {
            structure = StructureEnum::CORTEX_LEFT;
        }
        else if (structureName == "cerebellum") {
            structure = StructureEnum::CEREBELLUM;
        }
    }
    const AString specFilePath(FileInformation(file.fileName()).getAbsolutePath());

    /*
     * Files that require another file for conversion.
     * For example, cell files require a color file
     */
    std::vector<AString> phaseTwoFileNames;
    
    while ( ! file.atEnd()) {
        const int32_t maxLength(0);
        std::pair<AString, AString> line(readCaretHeaderLine(file, maxLength));
        if ( ( ! line.first.isEmpty())
            && ( ! line.second.isEmpty())) {
            const AString tag(line.first);
            AString filename(line.second);
            FileInformation fileInfo(filename);
            if ( ! fileInfo.isAbsolute()) {
                filename = FileInformation::assembleFileComponents(specFilePath, filename);
            }
            
            bool notSupportedFlag(false);
            
            FunctionResultValue<CaretDataFile*> result(FunctionResultValue<CaretDataFile*>(NULL, "", true));
            if (tag == "area_color_file") {
                FunctionResult result(readColorFile(filename,
                                                    CaretFiveFileTypeEnum::AREACOLOR,
                                                    m_areaColorsLabelTable.get()));
            }
            else if (tag == "cell_color_file") {
                FunctionResult result(readColorFile(filename,
                                                    CaretFiveFileTypeEnum::CELLCOLOR,
                                                    m_cellColorsLabelTable.get()));
            }
            else if (tag == "cell_file") {
                phaseTwoFileNames.push_back(filename);
            }
            else if (tag == "cellproj_file") {
                phaseTwoFileNames.push_back(filename);
            }
            else if (tag == "contour_cell_color_file") {
                FunctionResult result(readColorFile(filename,
                                                    CaretFiveFileTypeEnum::CONTOURCELLCOLOR,
                                                    m_contourCellColorsLabelTable.get()));
            }
            else if (tag == "contour_cell_file") {
                phaseTwoFileNames.push_back(filename);
            }
            else if (tag == "contour_file") {
                notSupportedFlag = true;
            }
            else if (tag.endsWith("coord_file")) {
                importDataFile(CaretFiveFileTypeEnum::COORDINATE,
                               structure,
                               filename);
            }
            else if (tag == "foci_color_file") {
                FunctionResult result(readColorFile(filename,
                                                    CaretFiveFileTypeEnum::FOCICOLOR,
                                                    m_fociColorsLabelTable.get()));
            }
            else if (tag == "foci_file") {
                phaseTwoFileNames.push_back(filename);
            }
            else if (tag.endsWith("label_file")) {
                phaseTwoFileNames.push_back(filename);
            }
            else if (tag == "metric_file") {
                importDataFile(CaretFiveFileTypeEnum::METRIC,
                               structure,
                               filename);
            }
            else if (tag == "section_file") {
                notSupportedFlag = true;
            }
            else if (tag == "surface_shape_file") {
                importDataFile(CaretFiveFileTypeEnum::SHAPE,
                               structure,
                               filename);
            }
            else if (tag.endsWith("topo_file")) {
                /* Ignore, topo is read when coordinate is converted */
            }
            else  {
                notSupportedFlag = false;
                addError(file.fileName(),
                         ("Unrecognized tag in spec file: "
                          + tag));
            }
            
            if (notSupportedFlag) {
                addWarningUnsupportedFileType(filename);
            }
        }
    }
    
    for (const AString& filename : phaseTwoFileNames) {
        bool validFlag(false);
        CaretFiveFileTypeEnum::Enum fileType(CaretFiveFileTypeEnum::fromFilename(filename, &validFlag));
        if (validFlag) {
            importDataFile(fileType,
                           structure,
                           filename);
        }
    }
    
    SpecFile specFile;
    setOutputFileName(&specFile,
                      file);
    m_outputSpecFileName = specFile.getFileName();
    
    return FunctionResultValue<CaretDataFile*>(NULL,
                                               "",
                                               true);
}

/**
 * If a spec file was imported, this returns the name for the Workench spec file.
 */
AString
CaretFiveDataFileImporter::getOutputSpecFileName() const
{
    return m_outputSpecFileName;
}

/**
 * Read the the header in XML format
 * @param xml
 *   The XML reader
 *@param caretDataFile
 *   The caret data file
 */
FunctionResult
CaretFiveDataFileImporter::readXmlFileHeader(QXmlStreamReader& xml,
                                             CaretDataFile* caretDataFile)
{
    AString nameText;
    AString valueText;
    
    bool doneFlag(false);
    xml.readNext();
    while ( ( ! xml.atEnd())
           && ( ! xml.hasError())
           && ( ! doneFlag) ) {
        if (xml.isStartElement()) {
            if (xml.name() == "Element") {
            }
            else if (xml.name() == "Name") {
                nameText  = xml.readElementText();
                valueText = "";
            }
            else if (xml.name() == "Value") {
                valueText = xml.readElementText();
                if ( ! nameText.isEmpty()) {
                    caretDataFile->getFileMetaData()->set(nameText,
                                                          valueText);
                }
                
                nameText  = "";
                valueText = "";
            }
        }
        else if (xml.isEndElement()) {
            if (xml.name() == "FileHeader") {
                doneFlag = true;
            }
        }
        
        if ( ! doneFlag) {
            xml.readNext();
        }
    }
    
    if (xml.hasError()) {
        return FunctionResult::error(xml.errorString());
    }
    
    return FunctionResult::ok();
}

/**
 * Read the the cell or focus in XML format
 * @param xml
 *   The XML reader
 *@param foci file
 *   The foci file
 */
FunctionResult
CaretFiveDataFileImporter::readXmlCellOrFocus(QXmlStreamReader& xml,
                                              FociFile* fociFile)
{
    CaretAssert(fociFile);
    
    AString nameText;
    AString valueText;
    
    std::unique_ptr<Focus> focus(new Focus());
    
    bool doneFlag(false);
    xml.readNext();
    while ( ( ! xml.atEnd())
           && ( ! xml.hasError())
           && ( ! doneFlag) ) {
        if (xml.isStartElement()) {
            if (xml.name() == "CellData") {
                focus.reset(new Focus());
            }
            else if (xml.name() == "className") {
                focus->setClassName(xml.readElementText());
            }
            else if (xml.name() == "xyz") {
                std::vector<float> xyz;
                AString::toNumbers(xml.readElementText(), xyz);
                if (xyz.size() == 3) {
                    CaretAssert(focus->getProjection(0));
                    focus->getProjection(0)->setStereotaxicXYZ(xyz.data());
                }
                else {
                    xml.raiseError("XYZ does not have 3 elements");
                }
            }
            else if (xml.name() == "name") {
                focus->setName(xml.readElementText());
            }
            else if (xml.name() == "comment") {
                focus->setComment(xml.readElementText());
            }
            else if (xml.name() == "structure") {
                StructureEnum::Enum structure = StructureEnum::INVALID;
                const AString structureName(xml.readElementText().toLower());
                if (structureName == "right") {
                    structure = StructureEnum::CORTEX_RIGHT;
                }
                else if (structureName == "left") {
                    structure = StructureEnum::CORTEX_LEFT;
                }
                else if (structureName == "cerebellum") {
                    structure = StructureEnum::CEREBELLUM;
                }
                focus->getProjection(0)->setStructure(structure);
            }
        }
        else if (xml.isEndElement()) {
            if (xml.name() == "CellData") {
                doneFlag = true;
                if (focus) {
                    fociFile->addFocus(focus.release());
                }
            }
        }
        
        if ( ! doneFlag) {
            xml.readNext();
        }
    }
    
    if (xml.hasError()) {
        return FunctionResult::error(xml.errorString());
    }
    
    return FunctionResult::ok();
}

/**
 * Read the the cell or focus projection in XML format
 * @param xml
 *   The XML reader
 *@param foci file
 *   The foci file
 */
FunctionResult
CaretFiveDataFileImporter::readXmlCellOrFocusProjection(QXmlStreamReader& xml,
                                                        FociFile* fociFile)
{
    CaretAssert(fociFile);
    
    AString nameText;
    AString valueText;
    float signedDistanceAboveSurface(0.0);
    StructureEnum::Enum structure = StructureEnum::INVALID;

    std::unique_ptr<Focus> focus(new Focus());
    
    enum class ProjType {
        NONE,
        INSIDE,
        OUTSIDE
    };
    ProjType projType = ProjType::NONE;
    std::vector<int32_t> baryClosestTileVertices;
    std::vector<float> baryClosestTileAreas;
    std::vector<float> baryCdistance;
    
    float veDR(0.0);
    std::vector<float> veTriFiducial;
    float vePhiR(0.0);
    float veThetaR(0.0);
    std::vector<int32_t> veTriVertices;
    std::vector<int32_t> veVertex;
    std::vector<float> veVertexFiducial;
    std::vector<float> vePosFiducial;
    float veFracRI(0.0);
    float veFracRJ(0.0);
    
    bool validFocusFlag(false);
    bool doneFlag(false);
    xml.readNext();
    while ( ( ! xml.atEnd())
           && ( ! xml.hasError())
           && ( ! doneFlag) ) {
        if (xml.isStartElement()) {
            if (xml.name() == "CellProjection") {
                focus.reset(new Focus());
            }
            else if (xml.name() == "className") {
                focus->setClassName(xml.readElementText());
            }
            else if (xml.name() == "xyz") {
                std::vector<float> xyz;
                AString::toNumbers(xml.readElementText(), xyz);
                if (xyz.size() == 3) {
                    CaretAssert(focus->getProjection(0));
                    focus->getProjection(0)->setStereotaxicXYZ(xyz.data());
                }
                else {
                    xml.raiseError("XYZ does not have 3 elements");
                }
            }
            else if (xml.name() == "name") {
                focus->setName(xml.readElementText());
            }
            else if (xml.name() == "comment") {
                focus->setComment(xml.readElementText());
            }
            else if (xml.name() == "structure") {
                const AString structureName(xml.readElementText().toLower());
                if (structureName == "right") {
                    structure = StructureEnum::CORTEX_RIGHT;
                }
                else if (structureName == "left") {
                    structure = StructureEnum::CORTEX_LEFT;
                }
                else if (structureName == "cerebellum") {
                    structure = StructureEnum::CEREBELLUM;
                }
            }
            else if (xml.name() == "signedDistanceAboveSurface") {
                const AString text(xml.readElementText());
                signedDistanceAboveSurface = text.toFloat();
            }
            else if (xml.name() == "projectionType") {
                const AString  projText(xml.readElementText().toLower());
                if (projText == "inside") {
                    projType = ProjType::INSIDE;
                }
                else if (projText == "outside") {
                    projType = ProjType::OUTSIDE;
                }
                else {
                    addWarning(fociFile->getFileName(),
                               ("Unrecogized projection type="
                                + projText));
                }
            }
            else if (xml.name() == "closestTileVertices") {
                const AString text(xml.readElementText());
                AString::toNumbers(text, baryClosestTileVertices);
            }
            else if (xml.name() == "closestTileAreas") {
                const AString text(xml.readElementText());
                AString::toNumbers(text, baryClosestTileAreas);
            }
            else if (xml.name() == "cdistance") {
                const AString text(xml.readElementText());
                AString::toNumbers(text, baryCdistance);
            }
            else if (xml.name() == "dR") {
                const AString text(xml.readElementText());
                veDR = text.toFloat();
            }
            else if (xml.name() == "triFiducial") {
                const AString text(xml.readElementText());
                AString::toNumbers(text, veTriFiducial);
            }
            else if (xml.name() == "thetaR") {
                const AString text(xml.readElementText());
                veThetaR = text.toFloat();
            }
            else if (xml.name() == "phiR") {
                const AString text(xml.readElementText());
                vePhiR = text.toFloat();
            }
            else if (xml.name() == "triVertices") {
                const AString text(xml.readElementText());
                AString::toNumbers(text, veTriVertices);
            }
            else if (xml.name() == "vertex") {
                const AString text(xml.readElementText());
                AString::toNumbers(text, veVertex);
            }
            else if (xml.name() == "vertexFiducial") {
                const AString text(xml.readElementText());
                AString::toNumbers(text, veVertexFiducial);
            }
            else if (xml.name() == "posFiducial") {
                const AString text(xml.readElementText());
                AString::toNumbers(text, vePosFiducial);
            }
            else if (xml.name() == "fracRI") {
                const AString text(xml.readElementText());
                veFracRI = text.toFloat();
            }
            else if (xml.name() == "fracRJ") {
                const AString text(xml.readElementText());
                veFracRJ = text.toFloat();
            }
        }
        else if (xml.isEndElement()) {
            if (xml.name() == "CellProjection") {
                doneFlag = true;
                if (focus) {
                    validFocusFlag = true;
                }
            }
        }
        
        if ( ! doneFlag) {
            xml.readNext();
        }
    }
    
    if (xml.hasError()) {
        return FunctionResult::error(xml.errorString());
    }
    
    if (validFocusFlag) {
        CaretAssert(focus);
        
        switch (projType) {
            case ProjType::NONE:
                break;
            case ProjType::INSIDE:
                if ((baryClosestTileAreas.size() == 3)
                    && (baryClosestTileVertices.size() == 3)
                    && (baryCdistance.size() == 3)) {
                    SurfaceProjectedItem* projItem(NULL);
                    if (focus->getNumberOfProjections() > 0) {
                        projItem = focus->getProjection(0);
                    }
                    else {
                        projItem = new SurfaceProjectedItem();
                        focus->addProjection(projItem);
                    }
                    
                    /* Triangles are oriented clockwise in Caret5 */
                    std::swap(baryClosestTileAreas[0], baryClosestTileAreas[2]);
                    std::swap(baryClosestTileVertices[0], baryClosestTileVertices[2]);
                    CaretAssert(projItem);
                    projItem->setStructure(structure);
                    SurfaceProjectionBarycentric* baryProj(projItem->getBarycentricProjection());
                    baryProj->setTriangleAreas(&baryClosestTileAreas[0]);
                    baryProj->setTriangleNodes(&baryClosestTileVertices[0]);
                    baryProj->setSignedDistanceAboveSurface(signedDistanceAboveSurface);
                    baryProj->setValid(true);
                }
                else {
                    addWarning(fociFile->getFileName(),
                               "Failed to read INSIDE projection");
                }
                break;
            case ProjType::OUTSIDE:
                if ((veTriFiducial.size() == 18)
                    && (veTriVertices.size() == 6)
                    && (veVertex.size() == 2)
                    && (veVertexFiducial.size() == 6)
                    && (vePosFiducial.size() == 3)) {
                    SurfaceProjectedItem* projItem(NULL);
                    if (focus->getNumberOfProjections() > 0) {
                        projItem = focus->getProjection(0);
                    }
                    else {
                        projItem = new SurfaceProjectedItem();
                        focus->addProjection(projItem);
                    }
                    CaretAssert(projItem);
                    projItem->setStructure(structure);
                    SurfaceProjectionVanEssen * veProj(projItem->getVanEssenProjection());
                    veProj->setDR(veDR);
                    float triAnat[2][3][3];
                    triAnat[0][0][0] = veTriFiducial[0];
                    triAnat[0][0][1] = veTriFiducial[1];
                    triAnat[0][0][2] = veTriFiducial[2];
                    triAnat[0][1][0] = veTriFiducial[3];
                    triAnat[0][1][1] = veTriFiducial[4];
                    triAnat[0][1][2] = veTriFiducial[5];
                    triAnat[0][2][0] = veTriFiducial[6];
                    triAnat[0][2][1] = veTriFiducial[7];
                    triAnat[0][2][2] = veTriFiducial[8];
                    triAnat[1][0][0] = veTriFiducial[9];
                    triAnat[1][0][1] = veTriFiducial[10];
                    triAnat[1][0][2] = veTriFiducial[11];
                    triAnat[1][1][0] = veTriFiducial[12];
                    triAnat[1][1][1] = veTriFiducial[13];
                    triAnat[1][1][2] = veTriFiducial[14];
                    triAnat[1][2][0] = veTriFiducial[15];
                    triAnat[1][2][1] = veTriFiducial[16];
                    triAnat[1][2][2] = veTriFiducial[17];
                    veProj->setTriAnatomical(triAnat);
                    veProj->setPhiR(vePhiR);
                    veProj->setThetaR(veThetaR);
                    int32_t triVert[2][3];
                    triVert[0][0] = veTriVertices[0];
                    triVert[0][1] = veTriVertices[1];
                    triVert[0][2] = veTriVertices[2];
                    triVert[1][0] = veTriVertices[3];
                    triVert[1][1] = veTriVertices[4];
                    triVert[1][2] = veTriVertices[5];
                    veProj->setTriVertices(triVert);
                    veProj->setVertex(&veVertex[0]);
                    float vertAnat[2][3];
                    vertAnat[0][0] = veVertexFiducial[0];
                    vertAnat[0][1] = veVertexFiducial[1];
                    vertAnat[0][2] = veVertexFiducial[2];
                    vertAnat[1][0] = veVertexFiducial[3];
                    vertAnat[1][1] = veVertexFiducial[4];
                    vertAnat[1][2] = veVertexFiducial[5];
                    veProj->setVertexAnatomical(vertAnat);
                    veProj->setPosAnatomical(&vePosFiducial[0]);
                    veProj->setFracRI(veFracRI);
                    veProj->setFracRJ(veFracRJ);
                    veProj->setValid(true);
                }
                break;
        }

        fociFile->addFocus(focus.release());
    }
    
    return FunctionResult::ok();
}

/**
 * Read the the color in XML format
 * @param xml
 *   The XML reader
 *@param labelTable
 *   The label table for the color
 */
FunctionResult
CaretFiveDataFileImporter::readXmlColor(QXmlStreamReader& xml,
                                        GiftiLabelTable* labelTable)
{
    CaretAssert(labelTable);
    
    AString nameText;
    AString valueText;
    
    AString labelName;
    int32_t labelRgba[4] { 0, 0, 0, 0 };
    
    bool doneFlag(false);
    xml.readNext();
    while ( ( ! xml.atEnd())
           && ( ! xml.hasError())
           && ( ! doneFlag) ) {
        if (xml.isStartElement()) {
            if (xml.name() == "name") {
                labelName = xml.readElementText();
            }
            else if (xml.name() == "red") {
                labelRgba[0] = xml.readElementText().toInt();
            }
            else if (xml.name() == "green") {
                labelRgba[1] = xml.readElementText().toInt();
            }
            else if (xml.name() == "blue") {
                labelRgba[2] = xml.readElementText().toInt();
            }
            else if (xml.name() == "alpha") {
                labelRgba[3] = xml.readElementText().toInt();
            }
        }
        else if (xml.isEndElement()) {
            if (xml.name() == "Color") {
                doneFlag = true;
                labelTable->addLabel(labelName,
                                     labelRgba[0],
                                     labelRgba[1],
                                     labelRgba[2],
                                     labelRgba[3]);
            }
        }
        
        if ( ! doneFlag) {
            xml.readNext();
        }
    }
    
    if (xml.hasError()) {
        return FunctionResult::error(xml.errorString());
    }
    
    return FunctionResult::ok();
}


/**
 * @return An unsupported file encoding message for the given encoding and file
 * @param fileEncoding
 *    The Caret5 file encoding
 * @param file
 *    The file
 */
FunctionResultValue<CaretDataFile*>
CaretFiveDataFileImporter::unsupportedFileEncodingError(const FileEncoding fileEncoding,
                                                        QFile& file)
{
    const AString message("Encoding="
                          + toFileEncodingName(fileEncoding)
                          + " not supported for file "
                          + file.fileName());
    return error(message);
}

/**
 * @return Name describing the file encoding enum
 * @param fileEncoding
 *    The file encoding
 */
AString
CaretFiveDataFileImporter::toFileEncodingName(const FileEncoding fileEncoding) const
{
    AString name("Invalid");
    switch (fileEncoding) {
        case INVALID:
            name = "Invalid";
            break;
        case GIFTI:
            name = "GIFTI";
            break;
        case ASCII:
            name = "ASCII";
            break;
        case BINARY:
            name = "BINARY";
            break;
        case VOLUME:
            name = "VOLUME";
            break;
        case XML:
            name = "XML";
            break;
    }
    return name;
}

/**
 * @return A FunctionResult with an error.
 * @param errorMessage
 *   Message describing the error.
 */
FunctionResultValue<CaretDataFile*>
CaretFiveDataFileImporter::error(const AString& errorMessage) const
{
    return FunctionResultValue<CaretDataFile*>(NULL,
                                               errorMessage,
                                               false);
}

/**
 * Set the name of an output file
 * @param caretDataFile
 *   The output file
 * @param file
 *   File that is being read
 */
FunctionResult
CaretFiveDataFileImporter::setOutputFileName(CaretDataFile* caretDataFile,
                                             const QFile& file) const
{
    FileInformation fileInfo(file.fileName());
    AString path, nameNoExt, ext;
    fileInfo.getFileComponents(path, 
                               nameNoExt,
                               ext);
    
    const AString extension(DataFileTypeEnum::toFileExtension(caretDataFile->getDataFileType()));
    
    AString dirOut(path);
    if ( ! m_outputDirectory.isEmpty()) {
        dirOut = m_outputDirectory;
    }
    AString newName(FileInformation::assembleFileComponents(dirOut,
                                                            (m_convertedFileNamePrefix + nameNoExt),
                                                            extension));
    
    caretDataFile->setFileName(newName);
    
    return FunctionResult::ok();
}

/**
 * Find the auxiliary file for the given CaretFive file type and filename
 * @param primaryFileType
 *    Type of Caret5 File
 * @param primaryFilename
 *    Name of CaretFive file
 * @param auxiliaryFilenameHint
 *    Hint for name of file that may be from the CaretFive file's header
 * @return
 *    FunctionResult containing name of file or error info
 */
FunctionResultString
CaretFiveDataFileImporter::findAuxiliaryFile(const CaretFiveFileTypeEnum::Enum primaryFileType,
                                             const AString primaryFilename,
                                             const AString auxiliaryFileNameHint)
{
    FunctionResultString result("", "Not finished", false);
    
    CaretFiveFileTypeEnum::Enum auxiliaryFileType(CaretFiveFileTypeEnum::INVALID);
    switch (primaryFileType) {
        case CaretFiveFileTypeEnum::INVALID:
            CaretAssert(0);
            break;
        case CaretFiveFileTypeEnum::AREACOLOR:
            break;
        case CaretFiveFileTypeEnum::AREALESTIMATION:
            break;
        case CaretFiveFileTypeEnum::BORDERCOLOR:
            break;
        case CaretFiveFileTypeEnum::BORDER:
            auxiliaryFileType = CaretFiveFileTypeEnum::BORDERCOLOR;
            break;
        case CaretFiveFileTypeEnum::BORDERPROJECTION:
            auxiliaryFileType = CaretFiveFileTypeEnum::BORDERCOLOR;
            break;
        case CaretFiveFileTypeEnum::CELLCOLOR:
            break;
        case CaretFiveFileTypeEnum::CELL:
            auxiliaryFileType = CaretFiveFileTypeEnum::CELLCOLOR;
            break;
        case CaretFiveFileTypeEnum::CELLPROJECTION:
            auxiliaryFileType = CaretFiveFileTypeEnum::CELLCOLOR;
            break;
        case CaretFiveFileTypeEnum::CONTOURCELLCOLOR:
            break;
        case CaretFiveFileTypeEnum::CONTOURCELL:
            auxiliaryFileType = CaretFiveFileTypeEnum::CONTOURCELLCOLOR;
            break;
        case CaretFiveFileTypeEnum::CONTOUR:
            break;
        case CaretFiveFileTypeEnum::COORDINATE:
            auxiliaryFileType = CaretFiveFileTypeEnum::TOPOLOGY;
            break;
        case CaretFiveFileTypeEnum::FOCICOLOR:
            break;
        case CaretFiveFileTypeEnum::FOCI:
            auxiliaryFileType = CaretFiveFileTypeEnum::FOCICOLOR;
            break;
        case CaretFiveFileTypeEnum::FOCIPROJECTION:
            break;
        case CaretFiveFileTypeEnum::LATLON:
            break;
        case CaretFiveFileTypeEnum::MATRIX:
            break;
        case CaretFiveFileTypeEnum::METRIC:
            break;
        case CaretFiveFileTypeEnum::PAINT:
            auxiliaryFileType = CaretFiveFileTypeEnum::AREACOLOR;
            break;
        case CaretFiveFileTypeEnum::SECTION:
            break;
        case CaretFiveFileTypeEnum::SHAPE:
            break;
        case CaretFiveFileTypeEnum::SPEC:
            break;
        case CaretFiveFileTypeEnum::TOPOLOGY:
            break;
        case CaretFiveFileTypeEnum::VOLUME:
            break;
    }

    if (auxiliaryFileType != CaretFiveFileTypeEnum::INVALID) {
        QStringList nameFilters;
        nameFilters << ("*" + CaretFiveFileTypeEnum::toExtension(auxiliaryFileType));
        
        const QFileInfo primaryFileInfo(primaryFilename);
        const AString primaryFilePath(primaryFileInfo.absolutePath());
        
        AString auxFileName;
        if ( ! auxiliaryFileNameHint.isEmpty()) {
            /*
             * Is there a file that is exact match of the name hint?
             */
            if (QFile::exists(auxiliaryFileNameHint)) {
                auxFileName = auxiliaryFileNameHint;
            }
            else {
                /*
                 * Look in directory that contains the primary file
                 */
                QDir dir(primaryFilePath);
                if (dir.exists()) {
                    /*
                     * Look for a file that contains the name hint
                     */
                    QStringList fileList(dir.entryList(nameFilters, QDir::Files));
                    QStringListIterator iter(fileList);
                    bool doneFlag(false);
                    while (iter.hasNext()
                           && ( ! doneFlag)) {
                        const AString name(iter.next());
                        if (name.contains(auxiliaryFileNameHint)) {
                            auxFileName = dir.absoluteFilePath(name);
                            doneFlag = true;
                        }
                    }
                }
            }
        }
        
        if (auxFileName.isEmpty()) {
            /*
             * Look for any file of correct type (matches name filter -
             * file extension)
             */
            QDir dir(primaryFilePath);
            QStringList fileList(dir.entryList(nameFilters, QDir::Files));
            if (fileList.count() > 0) {
                auxFileName = dir.absoluteFilePath(fileList.at(0));
            }
        }
        
        if ( ! auxFileName.isEmpty()) {
            return FunctionResultString(auxFileName,
                                        "",
                                        true);
        }
    }
    
    return result;
}

/**
 * Add an error message regarding the given file
 * @param filename
 *    Name of file
 * @param message
 *    The error message
 */
void
CaretFiveDataFileImporter::addError(const AString& filename,
                                    const AString& message)
{
    m_errorMessage.appendWithNewLine(filename);
    m_errorMessage.appendWithNewLine("   " + message);
}

/**
 * Add a warning message regarding the given file
 * @param filename
 *    Name of file
 * @param message
 *    The error message
 */
void
CaretFiveDataFileImporter::addWarning(const AString& filename,
                                      const AString& message)
{
    m_warningMessage.appendWithNewLine(filename);
    m_warningMessage.appendWithNewLine("   " + message);
}

/**
 * Add a warning message for a file type that is not supported for conversion
 * @param filename
 *    Name of file
 */
void
CaretFiveDataFileImporter::addWarningUnsupportedFileType(const AString& filename)
{
    addWarning(filename,
               "This type of file is not supported for conversion to Workbench format.");
}


