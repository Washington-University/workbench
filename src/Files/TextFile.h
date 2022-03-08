#ifndef __TEXT_FILE_H__
#define __TEXT_FILE_H__

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
        
        virtual void readFile(const AString& filename);
        
        virtual void writeFile(const AString& filename);
        
        virtual AString toString() const;
        
        AString getText() const;
        
        std::vector<AString> getTextLines() const;
        
        void replaceText(const AString& text);
        
        void addText(const AString& text);
        
        void addLine(const AString& text);
        
    private:
        AString text;
    };
    
} // namespace

#endif // __TEXT_FILE_H__
