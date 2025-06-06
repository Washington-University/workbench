#ifndef __CARET_FIVE_DATA_FILE_IMPORTER_H__
#define __CARET_FIVE_DATA_FILE_IMPORTER_H__

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



#include <memory>

#include "CaretFiveFileTypeEnum.h"
#include "CaretObject.h"
#include "FunctionResult.h"
#include "StructureEnum.h"

class QDir;
class QFile;
class QXmlStreamReader;

namespace caret {
    class CaretDataFile;
    class FociFile;
    class GiftiLabelTable;
    class GiftiMetaData;

    class CaretFiveDataFileImporter : public CaretObject {
        
    public:
        CaretFiveDataFileImporter();
        
        virtual ~CaretFiveDataFileImporter();
        
        CaretFiveDataFileImporter(const CaretFiveDataFileImporter&) = delete;

        CaretFiveDataFileImporter& operator=(const CaretFiveDataFileImporter&) = delete;

        void importFile(const AString& outputDirectory,
                        const AString& convertedFileNamePrefix,
                        const AString& filename,
                        const AString& auxiliaryFilename);

        std::vector<CaretDataFile*> takeImportedFiles();
        
        AString getOutputSpecFileName() const;
        
        bool hasErrors() const;
        
        AString getErrorMessage() const;
        
        bool hasWarnings() const;
        
        AString getWarningMessage() const;

        // ADD_NEW_METHODS_HERE
        
    private:
        enum FileEncoding {
            GIFTI,
            INVALID,
            ASCII,
            BINARY,
            VOLUME,
            XML
        };
        
        bool importDataFile(const CaretFiveFileTypeEnum::Enum caretFiveFileType,
                            const StructureEnum::Enum structure,
                            const AString& filename,
                            const AString& auxiliaryFilename = "");
        
        bool openFileAndGetEncoding(const AString& filename,
                                    QFile& fileOut,
                                    FileEncoding& encodingOut);
        
        FunctionResultValue<FileEncoding> determineFileEncoding(QFile& file) const;
        
        FunctionResultValue<GiftiMetaData*> readCaretFileHeader(QFile& file);
        
        std::pair<AString, AString> readCaretHeaderLine(QFile& file,
                                                        const int32_t maxLength) const;
        
        FunctionResultValue<CaretDataFile*> convertCellOrFociFile(const CaretFiveFileTypeEnum::Enum fileType,
                                                                  const FileEncoding fileEncoding,
                                                                  QFile& file,
                                                                  const AString& colorFileNameHint);

        FunctionResultValue<CaretDataFile*> convertCellOrFociProjectionFile(const CaretFiveFileTypeEnum::Enum fileType,
                                                                            const FileEncoding fileEncoding,
                                                                            QFile& file,
                                                                            const AString& colorFileNameHint);

        FunctionResultValue<CaretDataFile*> readColorFileIntoLabelFile(const CaretFiveFileTypeEnum::Enum fileType,
                                                                        const FileEncoding fileEncoding,
                                                                        QFile& file);

        FunctionResultValue<CaretDataFile*> convertCoordinateFile(const CaretFiveFileTypeEnum::Enum fileType,
                                                                  const FileEncoding fileEncoding,
                                                                  QFile& file,
                                                                  const AString& topologyFileNameHint);
        
        FunctionResultValue<CaretDataFile*> convertMetricFile(const FileEncoding fileEncoding,
                                                              const StructureEnum::Enum structure,
                                                              const bool metricFlag,
                                                              QFile& file);

        FunctionResultValue<CaretDataFile*> convertSpecFile(const FileEncoding fileEncoding,
                                                            QFile& file);
        
        AString toFileEncodingName(const FileEncoding fileEncoding) const;
        
        FunctionResultValue<CaretDataFile*> unsupportedFileEncodingError(const FileEncoding fileEncoding,
                                                                         QFile& file);
        FunctionResultValue<CaretDataFile*> error(const AString& errorMessage) const;
        
        FunctionResult readXmlFileHeader(QXmlStreamReader& xml,
                                         CaretDataFile* caretDataFile);
        
        FunctionResult readXmlCellOrFocus(QXmlStreamReader& xml,
                                          FociFile* fociFile);
        
        FunctionResult readXmlCellOrFocusProjection(QXmlStreamReader& xml,
                                                    FociFile* fociFile);
        
        FunctionResult readXmlColor(QXmlStreamReader& xml,
                                    GiftiLabelTable* labelTable);
        
        FunctionResult readTopologyFile(const AString& filename,
                                        std::vector<int32_t>& trianglesOut);
        
        FunctionResult readColorFile(const AString& filename,
                                     const CaretFiveFileTypeEnum::Enum fileType,
                                     GiftiLabelTable* labelTable);
        
        FunctionResult setOutputFileName(CaretDataFile* caretDataFile,
                                         const QFile& file) const;
        
        FunctionResultString findAuxiliaryFile(const CaretFiveFileTypeEnum::Enum primaryFileType,
                                               const AString primaryFilename,
                                               const AString auxiliaryFileNameHint);
        
        void addError(const AString& filename,
                      const AString& message);
        
        void addWarning(const AString& filename,
                        const AString& message);
        
        void addWarningUnsupportedFileType(const AString& filename);
        
        AString m_outputDirectory;
        
        AString m_convertedFileNamePrefix;
        
        std::unique_ptr<GiftiLabelTable> m_areaColorsLabelTable;
        std::unique_ptr<GiftiLabelTable> m_cellColorsLabelTable;
        std::unique_ptr<GiftiLabelTable> m_contourCellColorsLabelTable;
        std::unique_ptr<GiftiLabelTable> m_fociColorsLabelTable;
        
        std::vector<CaretDataFile*> m_importedFiles;
        
        AString m_outputSpecFileName;
        
        AString m_errorMessage;
        
        AString m_warningMessage;

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CARET_FIVE_DATA_FILE_IMPORTER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CARET_FIVE_DATA_FILE_IMPORTER_DECLARE__

} // namespace
#endif  //__CARET_FIVE_DATA_FILE_IMPORTER_H__
