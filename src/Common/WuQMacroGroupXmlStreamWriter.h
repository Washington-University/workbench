#ifndef __WU_Q_MACRO_GROUP_XML_STREAM_WRITER_H__
#define __WU_Q_MACRO_GROUP_XML_STREAM_WRITER_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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



#include <memory>

#include "WuQMacroGroupXmlStreamBase.h"

class QXmlStreamWriter;

namespace caret {

    class WuQMacro;
    class WuQMacroCommand;
    class WuQMacroGroup;
    class WuQMacroMouseEventInfo;
    
    class WuQMacroGroupXmlStreamWriter : public WuQMacroGroupXmlStreamBase {

    public:
        WuQMacroGroupXmlStreamWriter();
        
        virtual ~WuQMacroGroupXmlStreamWriter();
        
        void writeToString(const WuQMacroGroup* macroGroup,
                           QString& contentTextString);
        
        WuQMacroGroupXmlStreamWriter(const WuQMacroGroupXmlStreamWriter&) = delete;

        WuQMacroGroupXmlStreamWriter& operator=(const WuQMacroGroupXmlStreamWriter&) = delete;
        

        // ADD_NEW_METHODS_HERE

    private:
        void writeMacroToXML(const WuQMacro* macro);
        
        void writeMacroCommandToXML(const WuQMacroCommand* macroCommand);
        
        void writeMacroMouseEventInfo(const WuQMacroMouseEventInfo* mouseEventInfo);

        std::unique_ptr<QXmlStreamWriter> m_xmlStreamWriter;

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WU_Q_MACRO_GROUP_XML_STREAM_WRITER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_MACRO_GROUP_XML_STREAM_WRITER_DECLARE__

} // namespace
#endif  //__WU_Q_MACRO_GROUP_XML_STREAM_WRITER_H__
