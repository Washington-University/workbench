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

//try to force large file support from zlib, any other file reading calls
#ifndef CARET_OS_MACOSX
#define _LARGEFILE64_SOURCE
#define _LFS64_LARGEFILE 1
#define _FILE_OFFSET_BITS 64
#endif

#include "CaretAssert.h"
#include "CaretBinaryFile.h"
#include "CaretLogger.h"
#include "DataFileException.h"

#include <QFile>
#include "zlib.h"

#include <algorithm>

using namespace caret;
using namespace std;

//private implementation classes
namespace caret
{
#ifdef ZLIB_VERSION
    class ZFileImpl : public CaretBinaryFile::ImplInterface
    {
        gzFile m_zfile;
        const static int64_t CHUNK_SIZE;
    public:
        ZFileImpl() { m_zfile = NULL; }
        void open(const QString& filename, const CaretBinaryFile::OpenMode& opmode);
        void close();
        void seek(const int64_t& position);
        int64_t pos();
        int64_t size() { return -1; }
        void read(void* dataOut, const int64_t& count, int64_t* numRead);
        void write(const void* dataIn, const int64_t& count);
        ~ZFileImpl();
    };
    
    const int64_t ZFileImpl::CHUNK_SIZE = 1<<26;//64MiB, large enough for good performance, small enough for zlib, must convert to uint32
#endif //ZLIB_VERSION

    class QFileImpl : public CaretBinaryFile::ImplInterface
    {
        QFile m_file;
        const static int64_t CHUNK_SIZE;
    public:
        void open(const QString& filename, const CaretBinaryFile::OpenMode& opmode);
        void close();
        void seek(const int64_t& position);
        int64_t pos();
        int64_t size() { return m_file.size(); }
        void read(void* dataOut, const int64_t& count, int64_t* numRead);
        void write(const void* dataIn, const int64_t& count);
    };
    
    const int64_t QFileImpl::CHUNK_SIZE = 1<<30;//1GiB, QT4 apparently chokes at more than 2GiB via buffer.read using int32
}

CaretBinaryFile::ImplInterface::~ImplInterface()
{
}

CaretBinaryFile::CaretBinaryFile(const QString& filename, const OpenMode& fileMode)
{
    open(filename, fileMode);
}

void CaretBinaryFile::close()
{
    m_curMode = NONE;
    if (m_impl == NULL) return;
    m_impl->close();
    m_impl.grabNew(NULL);
}

QString CaretBinaryFile::getFilename() const
{
    if (m_impl == NULL) return "";//don't throw, its not really a problem
    return m_impl->getFilename();
}

bool CaretBinaryFile::getOpenForRead()
{
    return (m_curMode | READ) != 0;
}

bool CaretBinaryFile::getOpenForWrite()
{
    return (m_curMode | WRITE) != 0;
}

void CaretBinaryFile::open(const QString& filename, const OpenMode& opmode)
{
    close();
    if (opmode == NONE) throw DataFileException("can't open file with NONE mode");
    if (filename.endsWith(".gz"))
    {
#ifdef ZLIB_VERSION
        m_impl.grabNew(new ZFileImpl());
#else //ZLIB_VERSION
        throw DataFileException("can't open .gz file '" + filename + "', compiled without zlib support");
#endif //ZLIB_VERSION
    } else {
        m_impl.grabNew(new QFileImpl());
    }
    m_impl->open(filename, opmode);
    m_curMode = opmode;
}

void CaretBinaryFile::read(void* dataOut, const int64_t& count, int64_t* numRead)
{
    CaretAssert(count >= 0);//not sure about allowing 0
    if (!getOpenForRead()) throw DataFileException("file is not open for reading");
    m_impl->read(dataOut, count, numRead);
}

void CaretBinaryFile::seek(const int64_t& position)
{
    CaretAssert(position >= 0);
    if (m_curMode == NONE) throw DataFileException("file is not open, can't seek");
    m_impl->seek(position);
}

int64_t CaretBinaryFile::pos()
{
    if (m_curMode == NONE) throw DataFileException("file is not open, can't report position");
    return m_impl->pos();
}

int64_t CaretBinaryFile::size()
{
    if (m_curMode == NONE) throw DataFileException("file is not open, can't report size");
    return m_impl->size();
}

void CaretBinaryFile::write(const void* dataIn, const int64_t& count)
{
    CaretAssert(count >= 0);//not sure about allowing 0
    if (!getOpenForWrite()) throw DataFileException("file is not open for writing");
    m_impl->write(dataIn, count);
}

#ifdef ZLIB_VERSION
void ZFileImpl::open(const QString& filename, const CaretBinaryFile::OpenMode& opmode)
{
    close();//don't need to, but just because
    m_fileName = filename;
    const char* mode = NULL;
    switch (opmode)//we only support a limited number of combinations, and the string modes are quirky
    {
        case CaretBinaryFile::READ:
            mode = "rb";
            break;
        case CaretBinaryFile::WRITE_TRUNCATE:
            mode = "wb";//you have to do "r+b" in order to ask it to not truncate, which zlib doesn't support anyway
            break;
        default:
            throw DataFileException("compressed file only supports READ and WRITE_TRUNCATE modes");
    }
#if !defined(CARET_OS_MACOSX) && ZLIB_VERNUM > 0x1232
    m_zfile = gzopen64(filename.toLocal8Bit().constData(), mode);
#else
    m_zfile = gzopen(filename.toLocal8Bit().constData(), mode);
#endif
    if (m_zfile == NULL)
    {
        if (!QFile::exists(filename))
        {
            if (!(opmode & CaretBinaryFile::TRUNCATE))
            {
                throw DataFileException("failed to open compressed file '" + filename + "', file does not exist, or folder permissions prevent seeing it");
            } else {//use same logic as QFile impl for now
                throw DataFileException("failed to open compressed file '" + filename + "', unable to create file");
            }
        }//TODO: check gzerror and errno for more informative error messages
        throw DataFileException("failed to open compressed file '" + filename + "'");
    }
}

void ZFileImpl::close()
{
    if (m_zfile == NULL) return;//happens when closed and then destroyed, error opening
    if (gzclose(m_zfile) != 0) throw DataFileException("error closing compressed file '" + m_fileName + "'");
    m_zfile = NULL;
}

void ZFileImpl::read(void* dataOut, const int64_t& count, int64_t* numRead)
{
    if (m_zfile == NULL) throw DataFileException("read called on unopened ZFileImpl");//shouldn't happen
    int64_t totalRead = 0;
    int readret = 0;//to preserve the info of the read that broke early
    while (totalRead < count)
    {
        int64_t iterSize = min(count - totalRead, CHUNK_SIZE);
        readret = gzread(m_zfile, ((char*)dataOut) + totalRead, iterSize);
        if (readret < 1) break;//0 or -1 indicate eof or error
        totalRead += readret;
    }
    if (numRead == NULL)
    {
        if (totalRead != count)
        {
            if (readret < 0) throw DataFileException("error while reading compressed file '" + m_fileName + "'");
            throw DataFileException("premature end of file in compressed file '" + m_fileName + "'");
        }
    } else {
        *numRead = totalRead;
    }
}

void ZFileImpl::seek(const int64_t& position)
{
    if (m_zfile == NULL) throw DataFileException("seek called on unopened ZFileImpl");//shouldn't happen
    if (pos() == position) return;//slight hack, since gzseek is slow or nonfunctional for some cases, so don't try it unless necessary
#if !defined(CARET_OS_MACOSX) && ZLIB_VERNUM > 0x1232
    int64_t ret = gzseek64(m_zfile, position, SEEK_SET);
#else
    int64_t ret = gzseek(m_zfile, position, SEEK_SET);
#endif
    if (ret != position) throw DataFileException("seek failed in compressed file '" + m_fileName + "'");
}

int64_t ZFileImpl::pos()
{
    if (m_zfile == NULL) throw DataFileException("pos called on unopened ZFileImpl");//shouldn't happen
#if !defined(CARET_OS_MACOSX) && ZLIB_VERNUM > 0x1232
    return gztell64(m_zfile);
#else
    return gztell(m_zfile);
#endif
}

void ZFileImpl::write(const void* dataIn, const int64_t& count)
{
    if (m_zfile == NULL) throw DataFileException("read called on unopened ZFileImpl");//shouldn't happen
    int64_t totalWritten = 0;
    while (totalWritten < count)
    {
        int64_t iterSize = min(count - totalWritten, CHUNK_SIZE);
        int writeret = gzwrite(m_zfile, ((const char*)dataIn) + totalWritten, iterSize);
        if (writeret < 1) break;//0 or -1 indicate eof or error
        totalWritten += writeret;
    }
    if (totalWritten != count) throw DataFileException("failed to write to compressed file '" + m_fileName + "'");
}

ZFileImpl::~ZFileImpl()
{
    try//throwing from a destructor is a bad idea
    {
        close();
    } catch (CaretException& e) {//handles DataFileException, should be the only culprit
        CaretLogSevere(e.whatString());
    } catch (exception& e) {
        CaretLogSevere(e.what());
    } catch (...) {
        CaretLogSevere("caught unknown exception type while closing a compressed file");
    }
}
#endif //ZLIB_VERSION

void QFileImpl::open(const QString& filename, const CaretBinaryFile::OpenMode& opmode)
{
    close();//don't need to, but just because
    m_fileName = filename;
    QIODevice::OpenMode mode = QIODevice::NotOpen;//means 0
    if (opmode & CaretBinaryFile::READ) mode |= QIODevice::ReadOnly;
    if (opmode & CaretBinaryFile::WRITE) mode |= QIODevice::WriteOnly;
    if (opmode & CaretBinaryFile::TRUNCATE) mode |= QIODevice::Truncate;//expect QFile to recognize silliness like TRUNCATE by itself
    m_file.setFileName(filename);
    if (!m_file.open(mode))
    {
        if (!m_file.exists())
        {
            if (!(opmode & CaretBinaryFile::TRUNCATE))
            {
                throw DataFileException("failed to open file '" + filename + "', file does not exist, or folder permissions prevent seeing it");
            } else {//m_file.error() doesn't help identify this case, see below
                throw DataFileException("failed to open file '" + filename + "', unable to create file");
            }
        }
        switch (m_file.error())
        {
            case QFile::ResourceError://on linux at least, it never gives another code besides the unhelpful OpenError
                throw DataFileException("failed to open file '" + filename + "', too many open files, try 'ulimit -n 4096'");
            default:
                if (opmode & CaretBinaryFile::WRITE)
                {
                    throw DataFileException("failed to open file '" + filename + "' for writing");
                } else {
                    throw DataFileException("failed to open file '" + filename + "'");
                }
        }
    }
}

void QFileImpl::close()
{
    m_file.close();
}

void QFileImpl::read(void* dataOut, const int64_t& count, int64_t* numRead)
{
    int64_t total = 0;
    int64_t readret = -1;
    while (total < count)
    {
        int64_t maxToRead = min(count - total, CHUNK_SIZE);
        readret = m_file.read(((char*)dataOut) + total, maxToRead);//QFile chokes on large reads also
        if (readret < 1) break;//0 or -1 means error or eof
        total += readret;
    }
    if (numRead == NULL)
    {
        if (total != count)
        {
            if (readret < 0) throw DataFileException("error while reading file '" + m_fileName + "'");
            throw DataFileException("premature end of file in '" + m_fileName + "'");
        }
    } else {
        *numRead = total;
    }
}

void QFileImpl::seek(const int64_t& position)
{
    if (!m_file.seek(position)) throw DataFileException("seek failed in file '" + m_fileName + "'");
}

int64_t QFileImpl::pos()
{
    return m_file.pos();
}

void QFileImpl::write(const void* dataIn, const int64_t& count)
{
    int64_t total = 0;
    int64_t writeret = -1;
    while (total < count)
    {
        int64_t maxToWrite = min(count - total, CHUNK_SIZE);
        writeret = m_file.write((const char*)dataIn, maxToWrite);//QFile probably also chokes on large writes
        if (writeret < 1) break;//0 or -1 means error or eof
        total += writeret;
    }
    const AString msg = ("failed to write file '"
                         + m_fileName
                         + "'.  Tried to write "
                         + AString::number(count)
                         + " bytes but actually wrote "
                         + AString::number(total)
                         + " bytes.");
    if (total != count) throw DataFileException(msg);
}
