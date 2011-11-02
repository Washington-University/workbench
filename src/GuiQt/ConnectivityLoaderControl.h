#ifndef __CONNECTIVITY_LOADER_CONTROL__H_
#define __CONNECTIVITY_LOADER_CONTROL__H_

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

#include <vector>

#include <QWidget>

#include "AString.h"

class QAbstractButton;
class QButtonGroup;
class QGridLayout;
class QLabel;
class QLineEdit;
class QToolButton;

namespace caret {

    class ConnectivityLoaderFile;
    class WuQWidgetObjectGroup;
    
    class ConnectivityLoaderControl : public QWidget {
        
        Q_OBJECT

    public:
        ConnectivityLoaderControl(QWidget* parent = NULL);
        
        virtual ~ConnectivityLoaderControl();
        
        void updateControl();
        
    private:
        ConnectivityLoaderControl(const ConnectivityLoaderControl&);

        ConnectivityLoaderControl& operator=(const ConnectivityLoaderControl&);
        
    private slots:
        void addConnectivityLoader();
        
        void animateButtonPressed(QAbstractButton* button);
        void fileButtonPressed(QAbstractButton* button);
        void networkButtonPressed(QAbstractButton* button);
        void removeButtonPressed(QAbstractButton* button);
    private:
        QGridLayout* loaderLayout;

        std::vector<QLabel*> loaderNumberLabels;
        std::vector<QLineEdit*> fileNameLineEdits;
        std::vector<QLabel*> fileTypeLabels;
        std::vector<QToolButton*> animateButtons;
        std::vector<QToolButton*> fileButtons;
        std::vector<QToolButton*> networkButtons;
        std::vector<QToolButton*> removeButtons;
        
        QButtonGroup* animateButtonsGroup;
        QButtonGroup* fileButtonsGroup;
        QButtonGroup* networkButtonsGroup;
        QButtonGroup* removeButtonsGroup;

        std::vector<WuQWidgetObjectGroup*> rowWidgetGroups;
        
        AString previousCiftiFileTypeFilter;
    };
    
#ifdef __CONNECTIVITY_LOADER_CONTROL_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CONNECTIVITY_LOADER_CONTROL_DECLARE__

} // namespace
#endif  //__CONNECTIVITY_LOADER_CONTROL__H_
