
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

#include <QDir>
#include <iostream>

#define __SCENE_FILE_XML_STREAM_FORMAT_TESTER_DECLARE__
#include "SceneFileXmlStreamFormatTester.h"
#undef __SCENE_FILE_XML_STREAM_FORMAT_TESTER_DECLARE__

#include "CaretAssert.h"
#include "DataFileException.h"
#include "DataFileTypeEnum.h"
#include "ElapsedTimer.h"
#include "FileInformation.h"
#include "SceneFile.h"
#include "SystemUtilities.h"

using namespace caret;

/**
 * \class caret::SceneFileXmlStreamFormatTester 
 * \brief Test new XML Stream Reader and Writer
 * \ingroup Files
 */

/**
 * Constructor.
 */
SceneFileXmlStreamFormatTester::SceneFileXmlStreamFormatTester()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
SceneFileXmlStreamFormatTester::~SceneFileXmlStreamFormatTester()
{
}

bool
SceneFileXmlStreamFormatTester::testReadingAndWritingInDirectory(const QString& directoryName,
                                                                 const bool printResultsFlag)
{
    bool allSuccessfulFlag(false);
    
    QStringList extensions;
    for (auto ext : DataFileTypeEnum::getAllFileExtensions(DataFileTypeEnum::SCENE)) {
        extensions.append("*." + ext);
    }
    
    QDir directory(directoryName);
    QStringList sceneFileList = directory.entryList(extensions,
                                                    QDir::Files,
                                                    QDir::Name);
    
    QStringListIterator iter(sceneFileList);
    while (iter.hasNext()) {
        const QString filename = iter.next();
        const bool passFlag = testReadingAndWriting(filename,
                                                    printResultsFlag);
        std::cout << (passFlag ? "OK: " : "FAILED: ") << filename << std::endl;
    }
    return allSuccessfulFlag;
}

bool
SceneFileXmlStreamFormatTester::testReadingAndWriting(const QString& filename,
                                                      const bool printResultsFlag)
{
    const bool readFlag  = testReading(filename,
                                       printResultsFlag);
    const bool writeFlag = testWriting(filename,
                                       printResultsFlag);
    const bool successFlag = (readFlag
                              && writeFlag);
    if (printResultsFlag) {
        if (successFlag) {
            std::cout << "Success " << std::endl;
        }
        else {
            if ( ! readFlag) {
                std::cout << "Read FAILED" << std::endl;
            }
            if ( ! writeFlag) {
                std::cout << "Write FAILED" << std::endl;
            }
        }
    }
    
    return successFlag;
}

/**
 * Test reading of XML Stream Reader
 *
 * @param filename
 *     Name of file
 */
bool
SceneFileXmlStreamFormatTester::testReading(const QString& filename,
                                            const bool printResultsFlag)
{
    try {
        QString originalReWrittenFileName;
        QString intermediateFileName;
        QString finalFileName;
        getTempFileNames(filename,
                         "Read",
                         originalReWrittenFileName,
                         intermediateFileName,
                         finalFileName);
        
        /*
         * Save original format file since some changes to writing
         * may have been made since original file was last written
         * (perhaps CDATA added)
         */
        SceneFile sceneFileUpdated;
        sceneFileUpdated.readFileSaxReader(filename);
        sceneFileUpdated.writeFileSaxWriter(originalReWrittenFileName);

        SceneFile sceneFile;
        sceneFile.readFileStreamReader(originalReWrittenFileName);
        
        sceneFile.writeFileSaxWriter(finalFileName);
        
        FileInformation origFileInfo(filename);
        FileInformation origReWrittenFileInfo(originalReWrittenFileName);
        FileInformation finalFileInfo(finalFileName);
        
        const bool successFlag(origReWrittenFileInfo.size() == finalFileInfo.size());
        
        if (printResultsFlag) {
            const QString resultString(successFlag
                                       ? "READING MATCHED"
                                       : "FAILED");
            std::cout << resultString << std::endl;
            std::cout << "   Original File: " << origFileInfo.getFileName() << std::endl;
            std::cout << "   Original File (rewritten): " << origReWrittenFileInfo.getFileName() << std::endl;
            std::cout << "   Test output file name: " << finalFileInfo.getFileName() << std::endl;
            std::cout << "   Sizes (orig) " << origReWrittenFileInfo.size()
            << "  (output) " << finalFileInfo.size() << std::endl;
            std::cout << "   sdiff -s " << origReWrittenFileInfo.getFileName()
            << " " << finalFileInfo.getFileName() << std::endl;
            std::cout << std::endl;
        }

        return successFlag;
    }
    catch (const DataFileException& dfe) {
        if (printResultsFlag) {
            std::cout << "Failed: " << dfe.whatString() << std::endl;
        }
        return false;
    }
}

bool
SceneFileXmlStreamFormatTester::testWriting(const QString& filename,
                                            const bool printResultsFlag)
{
    try {
        QString originalReWrittenFileName;
        QString intermediateFileName;
        QString finalFileName;
        getTempFileNames(filename,
                         "Write",
                         originalReWrittenFileName,
                         intermediateFileName,
                         finalFileName);
  
        /*
         * Read with old reader and write with old reader since
         * writing may have changed since file was originally written
         */
        SceneFile sceneFileUpdated;
        sceneFileUpdated.readFileSaxReader(filename);
        sceneFileUpdated.writeFileSaxWriter(originalReWrittenFileName);

        /*
         * Write with new reader
         */
        sceneFileUpdated.writeFileSaxWriter(intermediateFileName);
        
        /*
         * Read file with old reader and write with
         * old reader.
         */
        SceneFile sceneFile;
        sceneFile.readFileSaxReader(intermediateFileName);
        sceneFile.writeFileSaxWriter(finalFileName);
        
        FileInformation origFileInfo(filename);
        FileInformation rewrittenFileInfo(originalReWrittenFileName);
        FileInformation intermediateFileInfo(intermediateFileName);
        FileInformation finalFileInfo(finalFileName);
        
        /*
         * If 'original re-written' matches final then
         * the intermediate writing with new writer was
         * successful
         */
        const bool successFlag(rewrittenFileInfo.size() == finalFileInfo.size());
        
        if (printResultsFlag) {
            const QString resultString(successFlag
                                       ? "WRITING MATCHED"
                                       : "FAILED");
            std::cout << resultString << std::endl;
            std::cout << "   Original File: " << origFileInfo.getFileName() << std::endl;
            std::cout << "   Old Reritten File: " << rewrittenFileInfo.getFileName() << std::endl;
            std::cout << "   New Writer File: " << intermediateFileInfo.getFileName() << std::endl;
            std::cout << "   Final file: " << finalFileInfo.getFileName() << std::endl;
            std::cout << "   Sizes (re-written) " << rewrittenFileInfo.size()
            << "  (final) " << finalFileInfo.size() << std::endl;
            std::cout << "   sdiff -s " << rewrittenFileInfo.getFileName()
            << " " << finalFileInfo.getFileName() << std::endl;
            std::cout << std::endl;
        }
        
        return successFlag;
    }
    catch (const DataFileException& dfe) {
        if (printResultsFlag) {
            std::cout << "Failed: " << dfe.whatString() << std::endl;
        }
        return false;
    }
}

void
SceneFileXmlStreamFormatTester::getTempFileNames(const QString& filename,
                                                 const QString& prefixName,
                                                 QString& originalFileReWrittenOut,
                                                 QString& intermediateFileNameOut,
                                                 QString& finalFileNameOut)
{
    
    FileInformation fileInfo(filename);
    AString absPath, nameNoExt, extNoDot;
    fileInfo.getFileComponents(absPath, nameNoExt, extNoDot);
    
    const AString path = fileInfo.getAbsolutePath();
    const AString name = fileInfo.getFileName();
    
    originalFileReWrittenOut = FileInformation::assembleFileComponents(absPath, prefixName + "TestOriginalReWritten", extNoDot);
    intermediateFileNameOut = FileInformation::assembleFileComponents(absPath,
                                                                      prefixName + "TestItermediate",
                                                                      extNoDot);
    
    finalFileNameOut = FileInformation::assembleFileComponents(absPath,
                                                               prefixName + "TestFinal",
                                                               extNoDot);
}

bool
SceneFileXmlStreamFormatTester::timeReadingAndWriting(const QString& filename)
{
    const bool successFlag(false);
    
    
    AString path, name, ext;
    FileInformation fileInfo(filename);
    fileInfo.getFileComponents(path, name, ext);
    
        std::cout << "Timing (seconds) for " << fileInfo.getFileName() << std::endl;
        
    try {
            SceneFile sceneFile;
            ElapsedTimer timer;
            timer.start();
            sceneFile.readFileSaxReader(filename);
            std::cout << "   Read OLD: " << timer.getElapsedTimeSeconds() << std::endl;
            
            timer.reset();

            QString tempName(FileInformation::assembleFileComponents(QDir::tempPath(), "TestRead", ext));
            
            timer.start();
            sceneFile.writeFileSaxWriter(tempName);
            std::cout << "   Write OLD: " << timer.getElapsedTimeSeconds() << std::endl;
    }
    catch (const DataFileException& dfe) {
        std::cout << "Test OLD Failed: " << dfe.whatString() << std::endl;
    }

    try {
            SceneFile sceneFile;
            ElapsedTimer timer;
            timer.start();
            sceneFile.readFileStreamReader(filename);
            std::cout << "   Read NEW: " << timer.getElapsedTimeSeconds() << std::endl;
            
            timer.reset();
            
            QString tempName(FileInformation::assembleFileComponents(QDir::tempPath(), "TestWrite", ext));

            timer.start();
            sceneFile.writeFileStreamWriter(tempName);
            std::cout << "   Write NEW: " << timer.getElapsedTimeSeconds() << std::endl;
        }
    catch (const DataFileException& dfe) {
        std::cout << "Test New Failed: " << dfe.whatString() << std::endl;
    }

    return successFlag;
}
