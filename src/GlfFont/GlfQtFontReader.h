#ifndef __GLF_QT_FONT_READER_H__
#define __GLF_QT_FONT_READER_H__

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

struct glf_font;

class GlfQtFontReader {
    
public:
    static int readFontFile(const char *fontFileName,
                            struct glf_font *glff);
    
private:
    GlfQtFontReader();
    
    virtual ~GlfQtFontReader();
    
    GlfQtFontReader(const GlfQtFontReader&);
    
    GlfQtFontReader& operator=(const GlfQtFontReader&);
    
public:
    
    // ADD_NEW_METHODS_HERE
    
private:
    // ADD_NEW_MEMBERS_HERE
    
};

#ifdef __GLF_QT_FONT_READER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __GLF_QT_FONT_READER_DECLARE__

#endif  //__GLF_QT_FONT_READER_H__
