#ifndef __SCENE_FILE_XML_STREAM_FORMAT_TESTER_H__
#define __SCENE_FILE_XML_STREAM_FORMAT_TESTER_H__

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



#include <memory>

#include "CaretObject.h"



namespace caret {

    class SceneFileXmlStreamFormatTester : public CaretObject {
        
    public:
        SceneFileXmlStreamFormatTester();
        
        virtual ~SceneFileXmlStreamFormatTester();
        
        SceneFileXmlStreamFormatTester(const SceneFileXmlStreamFormatTester&) = delete;

        SceneFileXmlStreamFormatTester& operator=(const SceneFileXmlStreamFormatTester&) = delete;
        
        static bool testReadingAndWriting(const QString& filename,
                                          const bool printResultsFlag);
        
        static bool testReading(const QString& filename,
                                const bool printResultsFlag);
        
        static bool testWriting(const QString& filename,
                                const bool printResultsFlag);

        static bool timeReadingAndWriting(const QString& filename);
        
        static bool testReadingAndWritingInDirectory(const QString& directoryName,
                                                     const bool printResultsFlag);
        
        // ADD_NEW_METHODS_HERE

    private:
        static void getTempFileNames(const QString& filename,
                                     const QString& prefixName,
                                     QString& originalFileReWrittenOut,
                                     QString& intermediateFileNameOut,
                                     QString& finalFileNameOut);
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __SCENE_FILE_XML_STREAM_FORMAT_TESTER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SCENE_FILE_XML_STREAM_FORMAT_TESTER_DECLARE__

} // namespace
#endif  //__SCENE_FILE_XML_STREAM_FORMAT_TESTER_H__
