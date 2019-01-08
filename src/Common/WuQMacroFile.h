#ifndef __WU_Q_MACRO_FILE_H__
#define __WU_Q_MACRO_FILE_H__

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

#include "DataFile.h"



namespace caret {

    class WuQMacro;
    class WuQMacroGroup;
    
    class WuQMacroFile : public DataFile {
        
    public:
        WuQMacroFile();
        
        virtual ~WuQMacroFile();
        
        virtual bool isEmpty() const override;
        
        WuQMacroGroup* getMacroGroup();
        
        const WuQMacroGroup* getMacroGroup() const;
        
        virtual void readFile(const AString& filename) override;
        
        virtual void writeFile(const AString& filename) override;

        static QString getFileDialogFilter();
        
        static QString getFileExtension();
            
        virtual void clearModified();
        
        virtual bool isModified() const;
        
        virtual void clear();
        
        void addMacro(WuQMacro* macro);
        
        void appendMacroGroup(const WuQMacroGroup* macroGroup);
        
        // ADD_NEW_METHODS_HERE

    private:
        void setMacroGroupName(const QString& filename);
        
        std::unique_ptr<WuQMacroGroup> m_macroGroup;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WU_Q_MACRO_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_MACRO_FILE_DECLARE__

} // namespace
#endif  //__WU_Q_MACRO_FILE_H__
