#ifndef __BRAIN_OPEN_G_L_WIDGET_CONTEXT_MENU__H_
#define __BRAIN_OPEN_G_L_WIDGET_CONTEXT_MENU__H_

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

#include <stdint.h>
#include <vector>

#include <QMenu>

class QAction;

namespace caret {

    class Brain;
    class BrowserTabContent;
    class CiftiConnectivityMatrixDataFileManager;
    class ConnectivityLoaderManager;
    class SelectionManager;
    class LabelFile;
    class Surface;
    
    class BrainOpenGLWidgetContextMenu : public QMenu {
        
        Q_OBJECT

    public:
        BrainOpenGLWidgetContextMenu(SelectionManager* identificationManager,
                                     BrowserTabContent* browserTabContent,
                                     QWidget* parent);
        
        virtual ~BrainOpenGLWidgetContextMenu();
        
    private slots:
        void createSurfaceFocusSelected();
        
        void createSurfaceIDSymbolFocusSelected();
        
        void createVolumeFocusSelected();
        
        void editSurfaceFocusSelected();
        
        void editVolumeFocusSelected();
        
        void removeAllNodeIdentificationSymbolsSelected();
        
        void removeNodeIdentificationSymbolSelected();
        
        void identifySurfaceBorderSelected();
        
        void identifySurfaceFocusSelected();
        
        void identifyVolumeFocusSelected();
        
        void identifySurfaceNodeSelected();

        void identifyVoxelSelected();
        
        void parcelCiftiConnectivityActionSelected(QAction* action);

        void parcelConnectivityActionSelected(QAction* action);
        
        void parcelTimeSeriesActionSelected(QAction* action);
        
        void borderCiftiConnectivitySelected();

        void borderConnectivitySelected();
        
        void borderTimeSeriesSelected();
        
    private:
        class ParcelConnectivity {
        public:
            ParcelConnectivity(LabelFile* labelFile,
                               const int32_t labelFileMapIndex,
                               const int32_t labelKey,
                               const QString& labelName,
                               Surface* surface,
                               const int32_t nodeNumber,
                               ConnectivityLoaderManager* connectivityLoaderManager,
                               CiftiConnectivityMatrixDataFileManager* ciftiConnectivityManager) {
                this->labelFile = labelFile;
                this->labelFileMapIndex = labelFileMapIndex;
                this->labelKey = labelKey;
                this->labelName = labelName;
                this->surface = surface;
                this->nodeNumber = nodeNumber;
                this->connectivityLoaderManager = connectivityLoaderManager;
                this->ciftiConnectivityManager = ciftiConnectivityManager;
            }
            
            LabelFile* labelFile;
            int32_t labelFileMapIndex;
            int32_t labelKey;
            QString labelName;
            Surface* surface;
            int32_t nodeNumber;
            CiftiConnectivityMatrixDataFileManager* ciftiConnectivityManager;
            ConnectivityLoaderManager* connectivityLoaderManager;
        };
        
        BrainOpenGLWidgetContextMenu(const BrainOpenGLWidgetContextMenu&);

        BrainOpenGLWidgetContextMenu& operator=(const BrainOpenGLWidgetContextMenu&);
                
        bool warnIfNetworkNodeCountIsLarge(const ConnectivityLoaderManager* clm,
                                           const std::vector<int32_t>& nodeIndices);
        bool warnIfNetworkNodeCountIsLarge(const CiftiConnectivityMatrixDataFileManager* cmdf,
                                           const std::vector<int32_t>& nodeIndices);
        
        std::vector<ParcelConnectivity*> parcelConnectivities;
        
        SelectionManager* identificationManager;
        
        BrowserTabContent* browserTabContent;
        
        QWidget* parentWidget;
    };
    
#ifdef __BRAIN_OPEN_G_L_WIDGET_CONTEXT_MENU_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_OPEN_G_L_WIDGET_CONTEXT_MENU_DECLARE__

} // namespace
#endif  //__BRAIN_OPEN_G_L_WIDGET_CONTEXT_MENU__H_
