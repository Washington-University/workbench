#ifndef __WUQ_MACRO_SHORT_CUT_COMBOBOX__H_
#define __WUQ_MACRO_SHORT_CUT_COMBOBOX__H_

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

#include <QComboBox>

#include "WuQMacroShortCutKeyEnum.h"
#include "WuQWidget.h"

namespace caret {

    class Surface;
    
    class WuQMacroShortCutKeyComboBox : public WuQWidget {
        
        Q_OBJECT

    public:
        WuQMacroShortCutKeyComboBox(QObject* parent);
        
        virtual ~WuQMacroShortCutKeyComboBox();
        
        WuQMacroShortCutKeyEnum::Enum getSelectedShortCutKey() const;
        
        QWidget* getWidget();

    public slots:
        void setSelectedShortCutKey(const WuQMacroShortCutKeyEnum::Enum shortCutKey);
        
    signals:
        void shortCutKeySelected(const WuQMacroShortCutKeyEnum::Enum);
        
    private slots:
        void shortCutKeyComboBoxSelection(int);
        
    private:
        WuQMacroShortCutKeyComboBox(const WuQMacroShortCutKeyComboBox&);

        WuQMacroShortCutKeyComboBox& operator=(const WuQMacroShortCutKeyComboBox&);
        
        QComboBox* m_shortCutKeyComboBox;
        
    public:
    private:
    };
    
#ifdef __WUQ_MACRO_SHORT_CUT_COMBOBOX_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WUQ_MACRO_SHORT_CUT_COMBOBOX_DECLARE__

} // namespace
#endif  //__WUQ_MACRO_SHORT_CUT_COMBOBOX__H_
