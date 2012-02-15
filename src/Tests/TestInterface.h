#ifndef __TEST_INTERFACE_H__
#define __TEST_INTERFACE_H__

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
#include <AString.h>

namespace caret {

   class TestInterface
   {
      AString m_identifier, m_failMessage;
      bool m_failed;
      TestInterface();//deny construction without arguments
      TestInterface& operator=(const TestInterface& right);//deny assignment
   protected:
      void setFailed(const AString failMessage)
      {
         m_failed = true;
         m_failMessage += failMessage + "\n";//append to previous messages
      }
      TestInterface(const AString& identifier) : m_failMessage("")
      {
         m_identifier = identifier;
         m_failed = false;
         m_default_path = "../../wb_files";
      }
   public:
      const AString& getIdentifier()
      {
         return m_identifier;
      }
      bool failed()
      {
         return m_failed;
      }
      const AString& getFailMessage()
      {
         return m_failMessage;
      }
      virtual void execute() = 0;//override this
      virtual ~TestInterface();
      AString m_default_path;
   };

}
#endif //__TEST_INTERFACE_H__
