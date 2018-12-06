#ifndef __WU_Q_MACRO_GROUP_XML_READER_H__
#define __WU_Q_MACRO_GROUP_XML_READER_H__

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

#include "WuQMacroGroupXmlFormatBase.h"

class QXmlStreamReader;


namespace caret {

    class WuQMacro;
    class WuQMacroCommand;
    class WuQMacroGroup;
    class WuQMacroMouseEventInfo;
    
    class WuQMacroGroupXmlReader : public WuQMacroGroupXmlFormatBase {

    public:
        WuQMacroGroupXmlReader();
        
        virtual ~WuQMacroGroupXmlReader();
        
        WuQMacroGroupXmlReader(const WuQMacroGroupXmlReader&) = delete;

        WuQMacroGroupXmlReader& operator=(const WuQMacroGroupXmlReader&) = delete;

        bool hasError() const;
        
        QString getErrorMessage() const;
        
        bool hasWarnings() const;
        
        QString getWarningMessage() const;
        
        void readFromString(const QString& xmlString,
                            WuQMacroGroup* macroGroup);

        // ADD_NEW_METHODS_HERE

    private:
        WuQMacroMouseEventInfo* readMacroMouseEventInfo();
        
        void readVersionOne(WuQMacroGroup* macroGroup);
        
        WuQMacro* readMacroVersionOne();
        
        WuQMacroCommand* readMacroCommandVersionOne();
        
        void addToWarnings(const QString& warning);
        
        std::unique_ptr<QXmlStreamReader> m_xmlStreamReader;

        QString m_warningMessage;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WU_Q_MACRO_GROUP_XML_READER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_MACRO_GROUP_XML_READER_DECLARE__

} // namespace
#endif  //__WU_Q_MACRO_GROUP_XML_READER_H__
