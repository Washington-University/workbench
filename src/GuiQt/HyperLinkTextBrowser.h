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

#ifndef __HYPER_LINK_TEXT_BROWSER_H__
#define __HYPER_LINK_TEXT_BROWSER_H__

#include <QTextBrowser>

class QKeyEvent;

namespace caret {
    class AString;
    
    class HyperLinkTextBrowser : public QTextBrowser {
        Q_OBJECT
        
    public:
        HyperLinkTextBrowser(QWidget* parent = 0);
        
        ~HyperLinkTextBrowser();
        
        void setContentToHtml(const AString& html);
        
        void setContentToText(const AString& text);
        
    signals:
        void keyPressed();
        
        public slots:      
        void append(const AString& text);
        
        void appendHtml(const AString& html);
        
        private slots:
        void setSource(const QUrl& url);
        
    private:
        void keyPressEvent(QKeyEvent* e);
    };
}


#endif // __HYPER_LINK_TEXT_BROWSER_H__

