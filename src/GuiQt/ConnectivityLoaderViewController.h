#ifndef __CONNECTIVITY_LOADER_VIEW_CONTROLLER__H_
#define __CONNECTIVITY_LOADER_VIEW_CONTROLLER__H_

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

#include <set>

#include <QObject>

class QAction;
class QCheckBox;
class QGridLayout;
class QLineEdit;
class QSpinBox;

namespace caret {

    class ConnectivityLoaderFile;
    class WuQGridLayoutGroup;
    
    class ConnectivityLoaderViewController : public QObject {
        
        Q_OBJECT

    public:
        ConnectivityLoaderViewController(const Qt::Orientation orientation,
                                         QGridLayout* gridLayout,
                                         const bool showTopHorizontalBar,
                                         QObject* parent);
        
        virtual ~ConnectivityLoaderViewController();
        
        void updateViewController(ConnectivityLoaderFile* connectivityLoaderFile);
        
        void setVisible(bool visible);
        
    private slots:
        void openFileActionTriggered();

        void openWebActionTriggered();
        
        void enabledCheckBoxStateChanged(int);

    private:
        ConnectivityLoaderViewController(const ConnectivityLoaderViewController&);

        ConnectivityLoaderViewController& operator=(const ConnectivityLoaderViewController&);
        
        void updateUserInterfaceAndGraphicsWindow();
        
        void updateOtherConnectivityViewControllers();
        
        void updateViewController();
        
        ConnectivityLoaderFile* connectivityLoaderFile;
        
        QAction* openFileAction;
        
        QAction* openWebAction;
        
        QCheckBox* enabledCheckBox;
        
        QLineEdit* fileNameLineEdit;
        
        WuQGridLayoutGroup* gridLayoutGroup;
        
        static std::set<ConnectivityLoaderViewController*> allConnectivityViewControllers;
    };
    
#ifdef __CONNECTIVITY_LOADER_VIEW_CONTROLLER_DECLARE__
    std::set<ConnectivityLoaderViewController*> ConnectivityLoaderViewController::allConnectivityViewControllers;
#endif // __CONNECTIVITY_LOADER_VIEW_CONTROLLER_DECLARE__

} // namespace
#endif  //__CONNECTIVITY_LOADER_VIEW_CONTROLLER__H_
