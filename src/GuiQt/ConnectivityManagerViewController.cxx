
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

#include <QAction>
#include <QDir>
#include <QMessageBox>
#include <QTemporaryFile>
#include <QToolButton>
#include <QVBoxLayout>

#define __CONNECTIVITY_MANAGER_VIEW_CONTROLLER_DECLARE__
#include "ConnectivityManagerViewController.h"
#undef __CONNECTIVITY_MANAGER_VIEW_CONTROLLER_DECLARE__

#include "Brain.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "ConnectivityLoaderFile.h"
#include "ConnectivityTimeSeriesViewController.h"
#include "ConnectivityDenseViewController.h"
#include "EventManager.h"
#include "EventListenerInterface.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "ImageFile.h"
#include "QCheckBox"
#include "QFileDialog"
#include "QHBoxLayout"
#include "QLabel"
#include "QSpinBox"
#include "QDoubleSpinBox"
#include "TimeCourseDialog.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;

/**
 * \class caret::ConnectivityManagerViewController 
 * \brief View-Controller for the ConnectivityLoaderManager
 *
 */
/**
 * Constructor.
 */
ConnectivityManagerViewController::ConnectivityManagerViewController(const Qt::Orientation orientation,
                                                                     const int32_t browserWindowIndex,
                                                                     const DataFileTypeEnum::Enum connectivityFileType,                                                                     
                                                                     QWidget* parent)
: QWidget(parent)
{
    this->orientation = orientation;
    this->browserWindowIndex = browserWindowIndex;
    this->connectivityFileType = connectivityFileType;
    
    this->viewControllerGridLayout = NULL;

    this->tcDialog = NULL;

    this->timeSeriesButtonLayout = NULL;
    this->graphToolButton = NULL;
    this->graphAction = NULL;
    this->movieToolButton = NULL;
    this->movieAction = NULL;

    this->frameRepeatLabel = new QLabel("Repeat Frames: ");
    this->frameRepeatSpinBox = new QSpinBox();
    this->frameRepeatSpinBox->setMaximum(59);
    this->frameRepeatSpinBox->setMinimum(0);

    this->frameRotateLabel = new QLabel("Rotate: ");
    this->frameRotateXLabel = new QLabel("X:");
    this->frameRotateXSpinBox = new QDoubleSpinBox();
    this->frameRotateXSpinBox->setMaximum(180.0);
    this->frameRotateXSpinBox->setMinimum(-180.0);

    this->frameRotateYLabel = new QLabel("Y:");
    this->frameRotateYSpinBox = new QDoubleSpinBox();
    this->frameRotateYSpinBox->setMaximum(180.0);
    this->frameRotateYSpinBox->setMinimum(-180.0);

    this->frameRotateZLabel = new QLabel("Z:");
    this->frameRotateZSpinBox = new QDoubleSpinBox();
    this->frameRotateZSpinBox->setMaximum(180.0);
    this->frameRotateZSpinBox->setMinimum(-180.0);

    this->frameRotateCountCheckBox = new QCheckBox();
    this->frameRotateCountCheckBox->setText("Rotate Frame Count:");
    this->frameRotateCountLabel = new QLabel();
    this->frameRotateCountSpinBox = new QSpinBox();


	this->movieAction = WuQtUtilities::createAction("Movie...",
		"Record Time Course Movie...",
		this,
		this,
		SLOT(movieActionTriggered(bool)));

	this->movieAction->setCheckable(true);
	this->movieToolButton = new QToolButton();
	this->movieToolButton->setDefaultAction(this->movieAction);
	frame_number = 0;
    
    switch (this->connectivityFileType) {
        case DataFileTypeEnum::CONNECTIVITY_DENSE:
            this->viewControllerGridLayout = ConnectivityDenseViewController::createGridLayout(orientation);
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
        {
            this->timeSeriesButtonLayout = new QHBoxLayout();

            QIcon graphIcon;
            const bool graphIconValid = WuQtUtilities::loadIcon(":/time_series_graph.png",
                graphIcon);
            
            this->graphAction = WuQtUtilities::createAction("Graph...",
                "Launch Time Course Dialog...",
                this,
                this,
                SLOT(graphActionTriggered()));           
            
            if (graphIconValid) {
                this->graphAction->setIcon(graphIcon);
            }            
            this->graphToolButton = new QToolButton();
            this->graphToolButton->setDefaultAction(this->graphAction);

            

            this->viewControllerGridLayout = ConnectivityTimeSeriesViewController::createGridLayout(orientation);
        }
            break;
        default:
            CaretAssertMessage(0, ("Unrecognized connectivity file type "
                                   + DataFileTypeEnum::toName(this->connectivityFileType)));
            return;
            break;
    }
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    if(this->timeSeriesButtonLayout)
    {
        if(this->graphToolButton) this->timeSeriesButtonLayout->addWidget(this->graphToolButton,0,Qt::AlignLeft);
        if(this->movieToolButton) this->timeSeriesButtonLayout->addWidget(this->movieToolButton,0,Qt::AlignLeft);
        if(this->frameRepeatLabel) this->timeSeriesButtonLayout->addWidget(this->frameRepeatLabel,0, Qt::AlignLeft);
        if(this->frameRepeatSpinBox) this->timeSeriesButtonLayout->addWidget(this->frameRepeatSpinBox,0, Qt::AlignLeft);

        if(this->frameRotateLabel) this->timeSeriesButtonLayout->addWidget(this->frameRotateLabel,0, Qt::AlignLeft);
        if(this->frameRotateXLabel) this->timeSeriesButtonLayout->addWidget(this->frameRotateXLabel,0, Qt::AlignLeft);
        if(this->frameRotateXSpinBox) this->timeSeriesButtonLayout->addWidget(this->frameRotateXSpinBox, 0, Qt::AlignLeft);
        if(this->frameRotateYLabel) this->timeSeriesButtonLayout->addWidget(this->frameRotateYLabel,0, Qt::AlignLeft);
        if(this->frameRotateYSpinBox) this->timeSeriesButtonLayout->addWidget(this->frameRotateYSpinBox, 0, Qt::AlignLeft);
        if(this->frameRotateZLabel) this->timeSeriesButtonLayout->addWidget(this->frameRotateZLabel,0, Qt::AlignLeft);
        if(this->frameRotateZSpinBox) this->timeSeriesButtonLayout->addWidget(this->frameRotateZSpinBox, 100, Qt::AlignLeft);

        /*if(this->frameRotateCountCheckBox) this->timeSeriesButtonLayout->addWidget(this->frameRotateCountCheckBox, 0, Qt::AlignLeft);
        if(this->frameRotateCountLabel) this->timeSeriesButtonLayout->addWidget(this->frameRotateCountLabel, 0, Qt::AlignLeft);
        if(this->frameRotateCountSpinBox) this->timeSeriesButtonLayout->addWidget(this->frameRotateCountSpinBox, 100, Qt::AlignLeft);*/

        layout->addLayout(this->timeSeriesButtonLayout);
    }
    
    layout->addLayout(this->viewControllerGridLayout);
    layout->addStretch();    

    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_GRAPHICS_UPDATE_ALL_WINDOWS);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_GRAPHICS_UPDATE_ONE_WINDOW);
}

/**
 * Destructor.
 */
ConnectivityManagerViewController::~ConnectivityManagerViewController()
{    
    EventManager::get()->removeAllEventsFromListener(this);
}

void
ConnectivityManagerViewController::graphActionTriggered()
{
    if(!this->tcDialog)
    {
        this->tcDialog = new TimeCourseDialog(this);
    }
    tcDialog->setTimeSeriesGraphEnabled(true);
    //tcDialog->show();
    tcDialog->updateDialog(true);
   
}

void
ConnectivityManagerViewController::movieActionTriggered(bool status)
{
    if(!status&&(frame_number > 0))
    {
        //render frames....

        QString formatString("Movie Files (*.mpg)");

        AString fileName = QFileDialog::getSaveFileName( this, tr("Save File"),QString::null, formatString );
        AString tempDir = QDir::tempPath();
        if ( !fileName.isEmpty() )
        {
            unlink(fileName);
            CaretLogInfo("Rendering movie to:" + fileName);
            AString ffmpeg = QCoreApplication::applicationDirPath() + AString("/ffmpeg ");

            double frame_rate = 30.0/double(1 + this->frameRepeatSpinBox->value());
            
            AString command = ffmpeg + AString("-threads 4 -r " + AString::number(frame_rate) + " -i "+ tempDir + "/movie%d.png -r 30 -q:v 1 -f mpeg1video " + fileName);
            CaretLogFine("running " + command);

            system(command.toAscii().data());
            CaretLogFine("Finished rendering " + fileName);
            
        }
        for(int i = 0;i<frame_number;i++)
        {
            AString tempFile = tempDir + "/movie" + AString::number(i) + AString(".png");
            unlink(tempFile);
        }
        frame_number = 0;
    }
}

void ConnectivityManagerViewController::captureFrame(AString filename)
{
    const int browserWindowIndex = 0;

    int32_t imageX = 0;
    int32_t imageY = 0;
    
    ImageFile imageFile;
    QApplication::setOverrideCursor(QCursor(Qt::BlankCursor));
    bool valid = GuiManager::get()->captureImageOfBrowserWindowGraphicsArea(browserWindowIndex,
        imageX,
        imageY,
        imageFile,
        false);
    QApplication::restoreOverrideCursor();


    if (valid == false) {
        WuQMessageBox::errorOk(this, 
            "Invalid window selected");
        return;
    }
    
    std::vector<AString> imageFileExtensions;
    AString defaultFileExtension;
    ImageFile::getImageFileExtensions(imageFileExtensions, 
        defaultFileExtension);

    

    bool validExtension = false;
    for (std::vector<AString>::iterator extensionIterator = imageFileExtensions.begin();
        extensionIterator != imageFileExtensions.end();
        extensionIterator++) {
            if (filename.endsWith(*extensionIterator)) {
                validExtension = true;
            }
    }

    if (validExtension == false) {
        if (defaultFileExtension.isEmpty() == false) {
            filename += ("." + defaultFileExtension);
        }
    }

    try {
        imageFile.writeFile(filename);
    }
    catch (const DataFileException& /*e*/) {
        QString msg("Unable to save: " + filename);
        WuQMessageBox::errorOk(this, msg);
    }
}


void 
ConnectivityManagerViewController::updateManagerViewController()
{
    Brain* brain = GuiManager::get()->getBrain();
    
    switch (this->connectivityFileType) {
        case DataFileTypeEnum::CONNECTIVITY_DENSE:
        {
            std::vector<ConnectivityLoaderFile*> files;
            brain->getConnectivityDenseFiles(files);
            this->updateForDenseFiles(files);
        }
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
        {
            std::vector<ConnectivityLoaderFile*> files;
            brain->getConnectivityTimeSeriesFiles(files);
            this->updateForTimeSeriesFiles(files);
        }
            break;
        default:
            break;
    }
}

/**
 * Update for dense connectivity files.
 * @param connectivityFiles
 *    The connectivity files.
 */
void 
ConnectivityManagerViewController::updateForDenseFiles(const std::vector<ConnectivityLoaderFile*>& denseFiles)
{
    /*
     * Update, show (and possibly add) connectivity view controllers
     */
    const int32_t numDenseFiles = static_cast<int32_t>(denseFiles.size());
    for (int32_t i = 0; i < numDenseFiles; i++) {
        if (i >= static_cast<int32_t>(this->denseViewControllers.size())) {
            this->denseViewControllers.push_back(new ConnectivityDenseViewController(this->orientation,
                                                                           this->viewControllerGridLayout,
                                                                           this));            
        }
        ConnectivityDenseViewController* cvc = this->denseViewControllers[i];
        cvc->updateViewController(denseFiles[i]);
        cvc->setVisible(true);
    }
    
    /*
     * Hide view controllers not needed
     */
    const int32_t numViewControllers = static_cast<int32_t>(this->denseViewControllers.size());
    for (int32_t i = numDenseFiles; i < numViewControllers; i++) {
        this->denseViewControllers[i]->setVisible(false);
    }    
}

/**
 * Update for time series files.
 * @param timeSeriesFiles
 *    The time series files.
 */
void 
ConnectivityManagerViewController::updateForTimeSeriesFiles(const std::vector<ConnectivityLoaderFile*>& timeSeriesFiles)
{
    /*
     * Update, show (and possibly add) time series view controllers
     */
    const int32_t numTimeSeriesFiles = static_cast<int32_t>(timeSeriesFiles.size());
    for (int32_t i = 0; i < numTimeSeriesFiles; i++) {
        if (i >= static_cast<int32_t>(this->timeSeriesViewControllers.size())) {
            this->timeSeriesViewControllers.push_back(new ConnectivityTimeSeriesViewController(this->orientation,
                                                                                       this->viewControllerGridLayout,
                                                                                       this));            
        }
        ConnectivityTimeSeriesViewController* tsvc = this->timeSeriesViewControllers[i];
        tsvc->updateViewController(timeSeriesFiles[i]);
        tsvc->setVisible(true);
    }
    
    /*
     * Hide view controllers not needed
     */
    const int32_t numTimeSeriesViewControllers = static_cast<int32_t>(this->timeSeriesViewControllers.size());
    for (int32_t i = numTimeSeriesFiles; i < numTimeSeriesViewControllers; i++) {
        this->timeSeriesViewControllers[i]->setVisible(false);
        this->timeSeriesViewControllers[i]->updateViewController(NULL);
    }    
}

/**
 * Receive events from the event manager.
 * 
 * @param event
 *   Event sent by event manager.
 */
void 
ConnectivityManagerViewController::receiveEvent(Event* event)
{
    
    if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        EventUserInterfaceUpdate* uiEvent =
        dynamic_cast<EventUserInterfaceUpdate*>(event);
        CaretAssert(uiEvent);
        
        if (uiEvent->isUpdateForWindow(this->browserWindowIndex)) {
            if (uiEvent->isConnectivityUpdate()
                || uiEvent->isToolBoxUpdate()) {
                this->updateManagerViewController();
                uiEvent->setEventProcessed();
            }
        }
    }
    else if(event->getEventType() == EventTypeEnum::EVENT_GRAPHICS_UPDATE_ALL_WINDOWS) {
        
        AString tempPath = QDir::tempPath();

        if(this->movieAction->isChecked())
        {
            this->captureFrame(tempPath + AString("/movie") + AString::number(frame_number) + AString(".png"));
			if(this->frameRotateXSpinBox->value() || this->frameRotateYSpinBox->value() || this->frameRotateZSpinBox->value())
			{
				this->processRotateTransformation(this->frameRotateXSpinBox->value(),this->frameRotateYSpinBox->value(),this->frameRotateZSpinBox->value());

			}
			AString temp = tempPath + AString("/movie") + AString::number(frame_number) + AString(".png");
			CaretLogFine(temp);
            CaretLogFine("frame number:" + frame_number);
			frame_number++;
        }
    }
    else if(event->getEventType() == EventTypeEnum::EVENT_GRAPHICS_UPDATE_ONE_WINDOW) {

        AString tempPath = QDir::tempPath();

        if(this->movieAction->isChecked())
        {
            this->captureFrame(tempPath + AString("/movie") + AString::number(frame_number) + AString(".png"));
            AString temp = tempPath + AString("/movie") + AString::number(frame_number) + AString(".png");
            CaretLogFine(temp);
            CaretLogFine("frame number:" + frame_number);
            frame_number++;
        }
    }


}


#if 1
#include <Matrix4x4.h>
#include "BrowserTabContent.h"
#include "Model.h"

void 
ConnectivityManagerViewController::processRotateTransformation(const double dx,
                                                  const double dy,
                                                  const double dz)
{
	BrowserTabContent* browserTabContent = 
		GuiManager::get()->getBrowserTabContentForBrowserWindow(browserWindowIndex, true);
	if (browserTabContent == NULL) {
		return;
	}
    Model* modelController = browserTabContent->getModelControllerForTransformation();
    if (modelController != NULL) {
        const int32_t tabIndex = browserTabContent->getTabNumber();
            
        {
            /*
             * There are several rotation matrix.  The 'NORMAL' matrix is used
             * in most cases and others are used in special viewing modes
             * such as surface montage and right/left lateral medial yoking
             */ 
            if (browserTabContent->isDisplayedModelSurfaceRightLateralMedialYoked()) {
                Matrix4x4* rotationMatrix = modelController->getViewingRotationMatrix(tabIndex, 
                                                                                      Model::VIEWING_TRANSFORM_NORMAL);
                rotationMatrix->rotateX(dx);
                rotationMatrix->rotateY(dy);
                rotationMatrix->rotateZ(dz);
                
                /*
                 * Matrix for a right medial/lateral yoked surface
                 */
                Matrix4x4* rotationMatrixRightLatMedYoked = modelController->getViewingRotationMatrix(tabIndex, 
                                                                                                      Model::VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED);
                rotationMatrixRightLatMedYoked->rotateX(-dy);
                rotationMatrixRightLatMedYoked->rotateY(-dx);
                rotationMatrixRightLatMedYoked->rotateZ(-dz);
            }
            else {
                ModelSurfaceMontage* montageModel = browserTabContent->getDisplayedSurfaceMontageModel();
                
                
                Matrix4x4* rotationMatrix = modelController->getViewingRotationMatrix(tabIndex, 
                                                                                      Model::VIEWING_TRANSFORM_NORMAL);
                rotationMatrix->rotateX(dy);
                rotationMatrix->rotateY(dx);
                rotationMatrix->rotateZ(dz);
                
                /*
                 * Matrix for a left surface opposite view in surface montage
                 */
                Matrix4x4* rotationMatrixSurfMontLeftOpp = modelController->getViewingRotationMatrix(tabIndex, 
                                                                                                     Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE);
                rotationMatrixSurfMontLeftOpp->rotateX(-dy);
                rotationMatrixSurfMontLeftOpp->rotateY(dx);
                rotationMatrixSurfMontLeftOpp->rotateZ(dz);
                
                /*
                 * Matrix for a right surface view in surface montage
                 */
                Matrix4x4* rotationMatrixSurfMontRight = modelController->getViewingRotationMatrix(tabIndex, 
                                                                                                     Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT);
                rotationMatrixSurfMontRight->rotateX(dy); 
                rotationMatrixSurfMontRight->rotateY(-dx);
                rotationMatrixSurfMontRight->rotateZ(dz);
                
                /*
                 * Matrix for a right surface opposite view in surface montage
                 */
                Matrix4x4* rotationMatrixSurfMontRightOpp = modelController->getViewingRotationMatrix(tabIndex, 
                                                                                                   Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE);
                rotationMatrixSurfMontRightOpp->rotateX(dy);
                rotationMatrixSurfMontRightOpp->rotateY(-dx);
                rotationMatrixSurfMontRightOpp->rotateZ(dz);
                
                /*
                 * Matrix for a right medial/lateral yoked surface
                 */
                Matrix4x4* rotationMatrixRightLatMedYoked = modelController->getViewingRotationMatrix(tabIndex, 
                                                                                           Model::VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED);
                rotationMatrixRightLatMedYoked->rotateX(dy);
                rotationMatrixRightLatMedYoked->rotateY(-dx);
                rotationMatrixRightLatMedYoked->rotateZ(dz);
            }            
        }
    }
}

#endif
