#ifndef __DISPLAY_GROUP_ENUM_COMBO_BOX__H_
#define __DISPLAY_GROUP_ENUM_COMBO_BOX__H_

/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/


#include "DisplayGroupEnum.h"
#include "WuQWidget.h"

class QComboBox;

namespace caret {

    class DisplayGroupEnumComboBox : public WuQWidget {
        Q_OBJECT
    public:
        DisplayGroupEnumComboBox(QObject* parent);
        
        virtual ~DisplayGroupEnumComboBox();
        
        DisplayGroupEnum::Enum getSelectedDisplayGroup() const;
        
        QWidget* getWidget();
        
    public slots:
        void setSelectedDisplayGroup(const DisplayGroupEnum::Enum displayGroup);
        
    signals:
        void displayGroupSelected(const DisplayGroupEnum::Enum);
        
    private slots:
        void displayGroupComboBoxSelection(int);
        
    private:
        DisplayGroupEnumComboBox(const DisplayGroupEnumComboBox&);

        DisplayGroupEnumComboBox& operator=(const DisplayGroupEnumComboBox&);
        
        QComboBox* displayGroupComboBox;
    };
    
#ifdef __DISPLAY_GROUP_ENUM_COMBO_BOX_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DISPLAY_GROUP_ENUM_COMBO_BOX_DECLARE__

} // namespace
#endif  //__DISPLAY_GROUP_ENUM_COMBO_BOX__H_
