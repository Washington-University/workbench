#ifndef __CARET_BINARY_FILE_H__
#define __CARET_BINARY_FILE_H__

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

#include "CaretPointer.h"

#include <QString>

#include <stdint.h>

namespace caret {
    
    //class to hide difference between compressed and standard binary file reading, and to automate error checking (throws if problem)
    class CaretBinaryFile
    {
    public:
        enum OpenMode
        {
            NONE = 0,
            READ = 1,
            WRITE = 2,
            READ_WRITE = 3,//for convenience
            TRUNCATE = 4,
            WRITE_TRUNCATE = 6,//ditto
            READ_WRITE_TRUNCATE = 7//ditto
        };
        CaretBinaryFile() { }
        ///constructor that opens file
        CaretBinaryFile(const QString& filename, const OpenMode& fileMode = READ);
        void open(const QString& filename, const OpenMode& opmode = READ);
        void close();
        QString getFilename() const;//not a reference because when no file is open, m_impl is NULL
        bool getOpenForRead();
        bool getOpenForWrite();
        void seek(const int64_t& position);
        int64_t pos();
        void read(void* dataOut, const int64_t& count, int64_t* numRead = NULL);//throw if numRead is NULL and (error or end of file reached early)
        void write(const void* dataIn, const int64_t& count);//failure to complete write is always an exception
        int64_t size();//may return -1 if size cannot be determined efficiently
        class ImplInterface
        {
        protected:
            QString m_fileName;//filename is tracked here so error messages can be implementation-specific
        public:
            virtual void open(const QString& filename, const OpenMode& opmode) = 0;
            virtual void close() = 0;
            const QString& getFilename() const { return m_fileName; }
            virtual void seek(const int64_t& position) = 0;
            virtual int64_t pos() = 0;
            virtual int64_t size() = 0;
            virtual void read(void* dataOut, const int64_t& count, int64_t* numRead) = 0;
            virtual void write(const void* dataIn, const int64_t& count) = 0;
            virtual ~ImplInterface();
        };
    private:
        CaretPointer<ImplInterface> m_impl;
        OpenMode m_curMode;//so implementation classes don't have to track it
    };
} //namespace caret

#endif //__CARET_BINARY_FILE_H__
