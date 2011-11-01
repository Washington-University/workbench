#ifndef __CONNECTIVITY_LOADER_FILE_CONTROL__H_
#define __CONNECTIVITY_LOADER_FILE_CONTROL__H_

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

class QLabel;
class QLineEdit;
class QToolButton;

namespace caret {

    class ConnectivityLoaderFile;
    class WuQWidgetObjectGroup;
    
    class ConnectivityLoaderFileControl : public QObject {
        
        Q_OBJECT

    public:
        ConnectivityLoaderFileControl(QWidget* parent,
                                      const int32_t loaderIndex);
        
        virtual ~ConnectivityLoaderFileControl();
        
    private slots:
        void browseButtonPressed();
        void networkButtonPressed();
        void clearButtonPressed();
        void removeButtonPressed();
        
    private:
        ConnectivityLoaderFileControl(const ConnectivityLoaderFileControl&);

        ConnectivityLoaderFileControl& operator=(const ConnectivityLoaderFileControl&);
        
        void update(ConnectivityLoaderFile* clf);
        
        QLabel* loaderNumberLabel;
        QLineEdit* fileNameLineEdit;
        QLabel* fileTypeLabel;
        QToolButton* browseButton;
        QToolButton* networkButton;
        QToolButton* clearButton;
        QToolButton* removeButton;
        ConnectivityLoaderFile* connectivityLoaderFile;

        WuQWidgetObjectGroup* widgetGroup;
        
        friend class ConnectivityLoaderControl;
    };
    
#ifdef __CONNECTIVITY_LOADER_FILE_CONTROL_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CONNECTIVITY_LOADER_FILE_CONTROL_DECLARE__

} // namespace
#endif  //__CONNECTIVITY_LOADER_FILE_CONTROL__H_
