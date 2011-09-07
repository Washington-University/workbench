#ifndef __TEXT_FILE_H__
#define __TEXT_FILE_H__

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

#include "DataFile.h"

namespace caret {
    
    /**
     * A simple text file.
     */
    class TextFile : public DataFile {
        
    public:
        TextFile();
        
        virtual ~TextFile();
        
    private:
        TextFile(const TextFile&);
        
        TextFile& operator=(const TextFile&);
        
    public:
        virtual void clear();
        
        virtual bool isEmpty() const;
        
        virtual void readFile(const AString& filename) throw (DataFileException);
        
        virtual void writeFile(const AString& filename) throw (DataFileException);
        
        virtual AString toString() const;
        
        AString getText() const;
        
        void replaceText(const AString& text);
        
        void addText(const AString& text);
        
        void addLine(const AString& text);
        
    private:
        AString text;
    };
    
} // namespace

#endif // __TEXT_FILE_H__
