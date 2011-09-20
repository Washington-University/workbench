/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#include "BinaryFileReader.h"
#include "FileInformation.h"

using namespace caret;
using namespace std;

uint64_t BinaryFileReader::m_minSeek = 2000000;//defaults if BinaryFileReader::setMinSeek(-1) is never called (should probably be called from SessionManager)
bool BinaryFileReader::m_enableReadSkip = true;

void BinaryFileReader::setMinSeek(uint64_t minSeek)
{
   if (minSeek == -1)
   {
      m_enableReadSkip = true;//TODO: load defaults from preferences here
      m_minSeek = 2000000;//for now, default to 2MB
   } else {
      m_enableReadSkip = (minSeek == 0);
      m_minSeek = minSeek;
   }
}

BinaryFileReader::BinaryFileReader(const AString& filename, BinaryFileReader::OpenMode fileMode) throw (DataFileException)
{
   switch (fileMode)
   {
      case READ:
         openRead(filename);
         break;
      case WRITE:
         openWrite(filename);
         break;
      case READ_WRITE:
         openReadWrite(filename);
         break;
      default:
         throw DataFileException("File open mode not recognized");
   };
}

void BinaryFileReader::closeFile()
{
   m_myFile.closeFile();
}

void BinaryFileReader::flushFile() throw (DataFileException)
{
   m_myFile.flushFile();
}

void BinaryFileReader::openRead(const AString& filename) throw (DataFileException)
{
   m_myFile.openRead(filename);
}

void BinaryFileReader::openReadWrite(const AString& filename) throw (DataFileException)
{
   m_myFile.openReadWrite(filename);
}

void BinaryFileReader::openWrite(const AString& filename) throw (DataFileException)
{
   m_myFile.openWrite(filename);
}

uint64_t BinaryFileReader::readBytes(byte* dataOut, const uint64_t count) throw (DataFileException)
{
   return m_myFile.readBytes(dataOut, count);
}

uint64_t BinaryFileReader::readBytes(byte* dataOut, const uint64_t count, const uint64_t position) throw (DataFileException)
{
   seekAbsolute(position);//NOT the file's function, this is where the magic happens
   return m_myFile.readBytes(dataOut, count);
}

void BinaryFileReader::seekAbsolute(const uint64_t position) throw (DataFileException)
{//here is the magic
   uint64_t curPos = m_myFile.getFilePosition();
   if (m_enableReadSkip && m_myFile.getOpenForRead() && position < curPos + m_minSeek && position >= curPos)
   {
      uint64_t toSkip = position - curPos, numRead = 0;
      while (toSkip > BUFFER_SIZE)
      {
         numRead = m_myFile.readBytes(m_skipBuffer, BUFFER_SIZE);
         if (numRead == 0)
         {
            throw DataFileException("Read seeking failed to read enough data");
         }
         toSkip -= numRead;
      }
      if (toSkip > 0)
      {
         toSkip -= m_myFile.readBytes(m_skipBuffer, toSkip);
      }
      if (toSkip > 0)
      {
         throw DataFileException("Read seeking failed to read enough data");
      }
   } else {
      m_myFile.seekAbsolute(position);
   }
}

void BinaryFileReader::writeBytes(const caret::byte* dataIn, const uint64_t count) throw (DataFileException)
{
   m_myFile.writeBytes(dataIn, count);
}

void BinaryFileReader::writeBytes(const caret::byte* dataIn, const uint64_t count, const uint64_t position) throw (DataFileException)
{
   seekAbsolute(position);//again, NOT the file's function, in case it is open READ_WRITE
   m_myFile.writeBytes(dataIn, count);
}

void BinaryFileReader::BinaryFile::statFile(const AString& filename) throw (DataFileException)
{
   FileInformation myInfo(filename);
   if (!myInfo.exists())
   {
      throw DataFileException("File does not exist");
   }
   if (!myInfo.isFile())
   {
      throw DataFileException("File is not a regular file");
   }
   m_fileSize = myInfo.size();
}


BinaryFileReader::BinaryFile::BinaryFile(const AString& filename, OpenMode fileMode) throw (DataFileException)
{
   switch (fileMode)
   {
      case READ:
         openRead(filename);
         break;
      case WRITE:
         openWrite(filename);
         break;
      case READ_WRITE:
         openReadWrite(filename);
         break;
      default:
         throw DataFileException("File open mode not recognized");
   };
}

void BinaryFileReader::BinaryFile::openRead(const AString& filename) throw (DataFileException)
{
   closeFile();
   statFile(filename);
   m_theFile.open(filename.c_str(), ios_base::in | ios_base::binary);
   if (m_theFile.fail())
   {//don't bother finding the exact error for now, if further investigation is done, put it in a member function
      throw DataFileException("Error opening file for reading");
   }
   m_canRead = true;
   m_canWrite = false;
}

void BinaryFileReader::BinaryFile::openWrite(const AString& filename) throw (DataFileException)
{
   closeFile();
   m_theFile.open(filename.c_str(), ios_base::out | ios_base::binary | ios_base::trunc);
   if (m_theFile.fail())
   {//don't bother finding the exact error for now, if further investigation is done, put it in a member function
      throw DataFileException("Error opening file for writing");
   }
   m_canRead = false;
   m_canWrite = true;
}

void BinaryFileReader::BinaryFile::openReadWrite(const AString& filename) throw (DataFileException)
{
   closeFile();
   statFile(filename);
   m_theFile.open(filename.c_str(), ios_base::in | ios_base::out | ios_base::binary);
   if (m_theFile.fail())
   {//don't bother finding the exact error for now, if further investigation is done, put it in a member function
      throw DataFileException("Error opening file for read and write");
   }
   m_canRead = true;
   m_canWrite = true;
}

void BinaryFileReader::BinaryFile::closeFile()
{
   if (m_theFile.is_open())
   {
      m_theFile.close();
   }
   m_fileSize = 0;
   m_canRead = false;
   m_canWrite = false;
}

void BinaryFileReader::BinaryFile::seekAbsolute(const uint64_t position) throw (DataFileException)
{
   if (m_canRead)
   {
      m_theFile.seekg((streamsize)position);
   }
   if (m_canWrite)
   {
      m_theFile.seekp((streamsize)position);
   }
   if (m_theFile.fail())
   {
      throw DataFileException("Error seeking in file");
   }
}

uint64_t BinaryFileReader::BinaryFile::readBytes(byte* dataOut, const uint64_t count) throw (DataFileException)
{
   if (!m_canRead)
   {
      throw DataFileException("Read attempted on a file not open for reading");
   }
   m_theFile.read(dataOut, (streamsize)count);
   if (m_canWrite)
   {//keep pointers synchronized for READ_WRITE
      m_theFile.seekp(m_theFile.tellg());
   }
   uint64_t ret = m_theFile.gcount();
   if (m_theFile.bad())//NOTE:DO NOT check fail(), EOF sets failbit!
   {
      throw DataFileException("Error reading from file");
   }
   return ret;
}

void BinaryFileReader::BinaryFile::writeBytes(const caret::byte* dataIn, const uint64_t count) throw (DataFileException)
{
   if (!m_canWrite)
   {
      throw DataFileException("Write attempted on a file not open for writing");
   }
   m_theFile.write(dataIn, (streamsize)count);
   if (m_canRead)
   {//keep pointers synchronized for READ_WRITE
      m_theFile.seekg(m_theFile.tellp());
   }
   if (m_theFile.fail())
   {
      throw DataFileException("Error writing to file");
   }
}

uint64_t BinaryFileReader::BinaryFile::readBytes(byte* dataOut, const uint64_t count, const uint64_t position) throw (DataFileException)
{
   seekAbsolute(position);
   return readBytes(dataOut, count);
}

void BinaryFileReader::BinaryFile::writeBytes(const caret::byte* dataIn, const uint64_t count, const uint64_t position) throw (DataFileException)
{
   seekAbsolute(position);
   writeBytes(dataIn, count);
}

void BinaryFileReader::BinaryFile::flushFile() throw (DataFileException)
{
   m_theFile.flush();
   if (m_theFile.fail())
   {
      throw DataFileException("Error flushing the file");
   }
}

uint64_t BinaryFileReader::BinaryFile::getFilePosition() throw (DataFileException)
{
   if (m_canRead)
   {
      return m_theFile.tellg();
   }
   if (m_canWrite)
   {
      return m_theFile.tellp();
   }
   throw DataFileException("File is not open");
}
