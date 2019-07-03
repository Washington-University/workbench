#ifndef __CIFTI_CONNECTIVITY_MATRIX_VIEW_CONTROLLER__H_
#define __CIFTI_CONNECTIVITY_MATRIX_VIEW_CONTROLLER__H_

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

#include <set>
#include <stdint.h>
#include <QWidget>

#include "EventListenerInterface.h"

class QCheckBox;
class QComboBox;
class QGridLayout;
class QLineEdit;
class QSignalMapper;
class QToolButton;

namespace caret {

    class CiftiMappableConnectivityMatrixDataFile;
    class CiftiFiberTrajectoryFile;
    class VolumeDynamicConnectivityFile;
    
    class CiftiConnectivityMatrixViewController : public QWidget, EventListenerInterface {
        
        Q_OBJECT

    public:
        CiftiConnectivityMatrixViewController(const QString& parentObjectName,
                                              QWidget* parent);
        
        virtual ~CiftiConnectivityMatrixViewController();
        
        void receiveEvent(Event* event);
        
    private slots:
        void enabledCheckBoxClicked(int);

        void layerCheckBoxClicked(int);
        
        void copyToolButtonClicked(int);
        
        void fiberOrientationFileComboBoxActivated(int);
        
    private:
        CiftiConnectivityMatrixViewController(const CiftiConnectivityMatrixViewController&);

        CiftiConnectivityMatrixViewController& operator=(const CiftiConnectivityMatrixViewController&);
        
//        void updateUserInterfaceAndGraphicsWindow();
        
        void updateViewController();
        
        void updateOtherCiftiConnectivityMatrixViewControllers();
        
        void updateFiberOrientationComboBoxes();
        
        void getFileAtIndex(const int32_t indx,
                            CiftiMappableConnectivityMatrixDataFile* &ciftiMatrixFileOut,
                            CiftiFiberTrajectoryFile* &ciftiTrajFileOut,
                            VolumeDynamicConnectivityFile* &volDynConnFileOut);
        
        const QString m_objectNamePrefix;
        
        std::vector<QCheckBox*> m_fileEnableCheckBoxes;
        
        std::vector<QCheckBox*> m_layerCheckBoxes;
        
        std::vector<QLineEdit*> m_fileNameLineEdits;
        
        std::vector<QToolButton*> m_fileCopyToolButtons;
        
        std::vector<QComboBox*> m_fiberOrientationFileComboBoxes;
        
        QGridLayout* m_gridLayout;
        
        QSignalMapper* m_signalMapperFileEnableCheckBox;
        
        QSignalMapper* m_signalMapperLayerCheckBox;
        
        QSignalMapper* m_signalMapperFileCopyToolButton;
        
        QSignalMapper* m_signalMapperFiberOrientationFileComboBox;
        
        static std::set<CiftiConnectivityMatrixViewController*> s_allCiftiConnectivityMatrixViewControllers;
        
        static int COLUMN_ENABLE_CHECKBOX;
        static int COLUMN_LAYER_CHECKBOX;
        static int COLUMN_COPY_BUTTON;
        static int COLUMN_NAME_LINE_EDIT;
        static int COLUMN_ORIENTATION_FILE_COMBO_BOX;
        
    };
    
#ifdef __CIFTI_CONNECTIVITY_MATRIX_VIEW_CONTROLLER_DECLARE__
    std::set<CiftiConnectivityMatrixViewController*> CiftiConnectivityMatrixViewController::s_allCiftiConnectivityMatrixViewControllers;
    int CiftiConnectivityMatrixViewController::COLUMN_ENABLE_CHECKBOX = 0;
    int CiftiConnectivityMatrixViewController::COLUMN_LAYER_CHECKBOX  = 1;
    int CiftiConnectivityMatrixViewController::COLUMN_COPY_BUTTON     = 2;
    int CiftiConnectivityMatrixViewController::COLUMN_NAME_LINE_EDIT  = 3;
    int CiftiConnectivityMatrixViewController::COLUMN_ORIENTATION_FILE_COMBO_BOX  = 4;
#endif // __CIFTI_CONNECTIVITY_MATRIX_VIEW_CONTROLLER_DECLARE__

} // namespace
#endif  //__CIFTI_CONNECTIVITY_MATRIX_VIEW_CONTROLLER__H_
