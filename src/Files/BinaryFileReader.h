
#ifndef __BINARY_FILE_H__
#define __BINARY_FILE_H__

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

#include <fstream>
#include "DataFileException.h"

namespace caret {
   
   typedef char byte;
   //a simple wrapper around whatever functions we choose to do binary reading with
   //NOTE:DO NOT inherit from this class, we do not want these functions available within a high level file type
   //instead, use it as a private member, for less confusion (please don't use private inheritance, it is more confusing to read through)
   
   //TODO: preferences option for whether to enable and what seek size
   class BinaryFileReader
   {
      enum OpenMode
      {
         READ,
         READ_WRITE,
         WRITE
      };
      //separate wrapping the IO functions from the efficiency functions, but we don't want any other classes to use this directly, even though it makes parent functions anemic
      class BinaryFile
      {
         std::fstream m_theFile;
         AString m_fileName;
         bool m_canRead, m_canWrite;
         uint64_t m_fileSize;
         void statFile(const AString& filename) throw (DataFileException);
      public:
         BinaryFile() { m_canRead = false; m_canWrite = false; m_fileSize = 0; };
         BinaryFile(const AString& filename, OpenMode fileMode) throw (DataFileException);
         void openRead(const AString& filename) throw (DataFileException);
         void openWrite(const AString& filename) throw (DataFileException);
         void openReadWrite(const AString& filename) throw (DataFileException);//ignore this case unless a need arises for it
         void closeFile();
         bool getOpenForRead() { return m_canRead; };
         bool getOpenForWrite() { return m_canWrite; };
         void seekAbsolute(const uint64_t position) throw (DataFileException);
         uint64_t readBytes(byte* dataOut, const uint64_t count) throw (DataFileException);//for now, allow the read to return fewer bytes than requested
         void writeBytes(const byte* dataIn, const uint64_t count) throw (DataFileException);//fstream doesn't let you know number of bytes written
         uint64_t readBytes(byte* dataOut, const uint64_t count, const uint64_t position) throw (DataFileException);//convenience methods
         void writeBytes(const byte* dataIn, const uint64_t count, const uint64_t position) throw (DataFileException);
         void flushFile() throw (DataFileException);
         uint64_t getInitialFileSize() { return m_fileSize; };
         uint64_t getFilePosition() throw (DataFileException);
      };
      
      BinaryFile m_myFile;
      const static uint64_t BUFFER_SIZE = 32768;
      byte m_skipBuffer[BUFFER_SIZE];
      static uint64_t m_minSeek;
      static bool m_enableReadSkip;
   public:
      ///sets the minimum size seek to do via seek rather than read and discard, -1 uses default values
      static void setMinSeek(int64_t minSeek);
      ///opens a file
      BinaryFileReader(const AString& filename, OpenMode fileMode) throw (DataFileException);
      ///open file for reading
      void openRead(const AString& filename) throw (DataFileException);
      ///open file for writing
      void openWrite(const AString& filename) throw (DataFileException);
      ///open file for random read and write
      void openReadWrite(const AString& filename) throw (DataFileException);//ignore this case unless a need arises for it
      ///close the file
      void closeFile();
      ///check if the file can be read from
      bool getOpenForRead() { return m_myFile.getOpenForRead(); };
      ///check if the file can be written to
      bool getOpenForWrite() { return m_myFile.getOpenForWrite(); };
      ///seek within the file, taking into account that small seeks aren't faster than sequential read, and don't use pagecache well
      void seekAbsolute(const uint64_t position) throw (DataFileException);
      ///read from the file at current position
      uint64_t readBytes(byte* dataOut, const uint64_t count) throw (DataFileException);//for now, allow the read to return fewer bytes than requested
      ///write to the file at current position
      void writeBytes(const byte* dataIn, const uint64_t count) throw (DataFileException);//fstream doesn't let you know number of bytes written
      ///seek (with readthrough on small forward seek if enabled) and then read from file
      uint64_t readBytes(byte* dataOut, const uint64_t count, const uint64_t position) throw (DataFileException);//convenience methods
      ///seek (with readthrough on small forward seek if enabled) and then write to file
      void writeBytes(const byte* dataIn, const uint64_t count, const uint64_t position) throw (DataFileException);
      ///flush changes to file
      void flushFile() throw (DataFileException);
      ///get initial file size on files opened with reading enabled
      uint64_t getInitialFileSize() { return m_myFile.getInitialFileSize(); };
   };
} //namespace caret

#endif //__BINARY_FILE_H__
