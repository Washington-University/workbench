#ifndef __MULTI_PAGE_DIALOG_PAGE__H_
#define __MULTI_PAGE_DIALOG_PAGE__H_

/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
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


#include <QObject>

#include "AString.h"

class QWidget;

namespace caret {

    class MultiPageDialogPage : public QObject {
        
        Q_OBJECT

    protected:
        MultiPageDialogPage(QObject* pareant,
                            const AString& pageName);
        
    public:
        virtual ~MultiPageDialogPage();

        QWidget* getPageWidget();
        
        AString getPageName() const;
        
        /**
         * Is the page valid?
         */
        virtual bool isPageValid() = 0;
        
        /**
         * Update the contents of the page.
         */
        void updatePage();
        
    public slots:
        /**
         * Will be called when Apply button is pressed.
         */
        void applyPage();
        

    protected:
        void addWidget(QObject* w,
                       const bool isValueChangedSignalConnectedToApplyPage = true);
        
        /**
         * Apply the pages content.
         */
        virtual void applyPageContent() = 0;
        
        /**
         * Will be called to request that the page be constructed.
         */
        virtual QWidget* createPageContent() = 0;
        
        /**
         * Update the contents of the page.
         */
        virtual void updatePageContent() = 0;
        
        void updateAllGraphics();
        
    private:
        MultiPageDialogPage(const MultiPageDialogPage&);

        MultiPageDialogPage& operator=(const MultiPageDialogPage&);
        
    public:
    private:
        AString pageName;
        
        QWidget* pageWidget;
        
        bool isPageUpdateInProgress;
        
    };
    
#ifdef __MULTI_PAGE_DIALOG_PAGE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __MULTI_PAGE_DIALOG_PAGE_DECLARE__

} // namespace
#endif  //__MULTI_PAGE_DIALOG_PAGE__H_
