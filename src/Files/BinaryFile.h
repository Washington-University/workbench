
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
   class BinaryFile
   {
      std::fstream m_theFile;
      AString m_fileName;
      bool m_canRead, m_canWrite;
      uint64_t m_fileSize;
      enum OpenMode
      {
         READ,
         WRITE,
         READ_WRITE
      };
      void statFile(const AString& filename) throw (DataFileException);
   public:
      BinaryFile() { m_canRead = false; m_canWrite = false; m_fileSize = 0; };
      BinaryFile(const AString& filename, OpenMode fileMode) throw (DataFileException);
      void openRead(const AString& filename) throw (DataFileException);
      void openWrite(const AString& filename) throw (DataFileException);
      void openReadWrite(const AString& filename) throw (DataFileException);//doesn't truncate on open, not sure of a way to change file size after open, maybe write to end?
      void closeFile();
      bool getOpenForRead() { return m_canRead; };
      bool getOpenForWrite() { return m_canWrite; };
      void seekAbsolute(const uint64_t position) throw (DataFileException);
      uint64_t readBytes(byte* dataOut, const uint64_t count) throw (DataFileException);//for now, allow the read to return fewer bytes than requested
      void writeBytes(const byte* dataIn, const uint64_t count) throw (DataFileException);//fstream doesn't let you know number of bytes written
      uint64_t readBytes(byte* dataOut, const uint64_t count, const uint64_t position);//convenience methods
      void writeBytes(const byte* dataIn, const uint64_t count, const uint64_t position);
      void flushFile() throw (DataFileException);
      uint64_t getInitialFileSize() { return m_fileSize; };
   };
} //namespace caret

#endif //__BINARY_FILE_H__