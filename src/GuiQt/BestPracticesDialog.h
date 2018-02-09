#ifndef __BEST_PRACTICES_DIALOG_H__
#define __BEST_PRACTICES_DIALOG_H__

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

#include "WuQDialogNonModal.h"



namespace caret {

    class BestPracticesDialog : public WuQDialogNonModal {
        
        Q_OBJECT

    public:
        enum class InfoMode {
            LOCK_ASPECT_BEST_PRACTICES
        };
        
        enum TextMode {
            BRIEF,
            FULL
        };
        
        BestPracticesDialog(const InfoMode infoMode,
                            QWidget* parent);
        
        virtual ~BestPracticesDialog();

        static AString getTextForInfoMode(const InfoMode infoMode,
                                          const TextMode textMode);

        // ADD_NEW_METHODS_HERE

    private:
        BestPracticesDialog(const BestPracticesDialog&);

        BestPracticesDialog& operator=(const BestPracticesDialog&);
        
        static AString getLockAspectText(const TextMode textMode);
        
        const InfoMode m_infoMode;
        
        const TextMode m_textMode;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __BEST_PRACTICES_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BEST_PRACTICES_DIALOG_DECLARE__

} // namespace
#endif  //__BEST_PRACTICES_DIALOG_H__
