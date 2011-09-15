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

#include "BinaryFile.h"
#include "FileInformation.h"

using namespace caret;
using namespace std;

void BinaryFile::statFile(const AString& filename) throw (DataFileException)
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


BinaryFile::BinaryFile(const AString& filename, OpenMode fileMode) throw (DataFileException)
{
   switch (fileMode)
   {
      case READ:
         openRead(filename);
         break;
      case WRITE:
         openWrite(filename);
         break;
      /*case READ_WRITE:
         openReadWrite(filename);
         break;//*/
      default:
         throw DataFileException("File open mode not recognized");
   };
}

void BinaryFile::openRead(const AString& filename) throw (DataFileException)
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

void BinaryFile::openWrite(const AString& filename) throw (DataFileException)
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

/*void BinaryFile::openReadWrite(const AString& filename) throw (DataFileException)
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
}//*/ //ignore this case unless a need arises for it, because it can unsync the two file positions (tellg and tellp won't match after read or write unless explicitly set)

void BinaryFile::closeFile()
{
   if (m_theFile.is_open())
   {
      m_theFile.close();
   }
   m_fileSize = 0;
   m_canRead = false;
   m_canWrite = false;
}

void BinaryFile::seekAbsolute(const uint64_t position) throw (DataFileException)
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

uint64_t BinaryFile::readBytes(byte* dataOut, const uint64_t count) throw (DataFileException)
{
   if (!m_canRead)
   {
      throw DataFileException("Read attempted on a file not open for reading");
   }
   m_theFile.read(dataOut, (streamsize)count);
   uint64_t ret = m_theFile.gcount();
   if (m_theFile.bad())//NOTE:DO NOT check fail(), EOF sets failbit!
   {
      throw DataFileException("Error reading from file");
   }
   return ret;
}

void BinaryFile::writeBytes(const caret::byte* dataIn, const uint64_t count) throw (DataFileException)
{
   if (!m_canWrite)
   {
      throw DataFileException("Write attempted on a file not open for writing");
   }
   m_theFile.write(dataIn, (streamsize)count);
   if (m_theFile.fail())
   {
      throw DataFileException("Error writing to file");
   }
}

uint64_t BinaryFile::readBytes(byte* dataOut, const uint64_t count, const uint64_t position)
{
   seekAbsolute(position);
   return readBytes(dataOut, count);
}

void BinaryFile::writeBytes(const caret::byte* dataIn, const uint64_t count, const uint64_t position)
{
   seekAbsolute(position);
   writeBytes(dataIn, count);
}

void BinaryFile::flushFile() throw (DataFileException)
{
   m_theFile.flush();
   if (m_theFile.fail())
   {
      throw DataFileException("Error flushing the file");
   }
}
