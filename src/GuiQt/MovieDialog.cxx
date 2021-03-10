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

#include <QDir>
#include <QFileDialog>
#ifdef CARET_OS_WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "MovieDialog.h"
#include "ui_MovieDialog.h"
#include "BrowserTabContent.h"
#include "BrainBrowserWindow.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretPreferences.h"
#include "DataFileException.h"
#include "EventManager.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventImageCapture.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "ImageFile.h"
#include <Matrix4x4.h>
#include "Model.h"
#include "ModelSurface.h"
#include "ModelSurfaceSelector.h"
#include "SessionManager.h"
#include "Surface.h"
#include "WuQMessageBox.h"
#include <ModelVolume.h>
#include <VolumeSliceViewPlaneEnum.h>
#include <VolumeFile.h>

using namespace caret;

/**
 * \class caret::MovieDialog
 * \brief Dialog used for setting up and rendering Movies.
 * \ingroup GuiQt
 */
MovieDialog::MovieDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MovieDialog)
{
    ui->setupUi(this);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_GRAPHICS_UPDATE_ONE_WINDOW);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_GRAPHICS_UPDATE_ALL_WINDOWS);

    m_browserWindowIndex = 0;
    frame_number = 0;
    rotate_frame_number = 0;

    imageX = 0;
    imageY = 0;

	m_animationStarted = false;
	m_volumeSliceIncrement = 0;
	m_reverseVolumeSliceDirection = false;
	m_PStart = 0;
	m_CStart = 0;
	m_AStart = 0;
	m_PEnd = 0;
	m_CEnd = 0;
	m_AEnd = 0;
    m_interpolationIndex = 0;
    
    m_surface  = NULL;
    m_surface1 = NULL;
    m_surface2 = NULL;
}

MovieDialog::~MovieDialog()
{
    ui->animateButton->setChecked(false);    
    delete ui;
    EventManager::get()->removeAllEventsFromListener(this);

}

void MovieDialog::on_closeButton_clicked()
{
    ui->animateButton->setChecked(false);    
    this->close();
}


void MovieDialog::on_interpolateSurfaceCheckbox_toggled(bool checked)
{
    if ( ! checked) {
        return;
    }
    
    BrainBrowserWindow *bw = GuiManager::get()->getBrowserWindowByWindowIndex(m_browserWindowIndex);
    if(!bw)
    {
        WuQMessageBox::errorOk(this,
                               "Invalid browser window index, " + AString::number(m_browserWindowIndex));
        return;
    }
    
    BrowserTabContent *btc1 = bw->getBrowserTabContent(0);
    BrowserTabContent *btc2 = bw->getBrowserTabContent(1);
    if(!btc1 || !btc2) {
        this->ui->interpolateSurfaceCheckbox->blockSignals(true);
        this->ui->interpolateSurfaceCheckbox->setChecked(false);
        this->ui->interpolateSurfaceCheckbox->blockSignals(false);
        WuQMessageBox::errorOk(this,
                               "There must be two browser tabs.");
        return;
    }
    
    //    int32_t tabIndex1 = btc1->getTabNumber();
    if ((btc1->getSelectedModelType() != ModelTypeEnum::MODEL_TYPE_SURFACE)
        || (btc2->getSelectedModelType() != ModelTypeEnum::MODEL_TYPE_SURFACE)) {
        this->ui->interpolateSurfaceCheckbox->blockSignals(true);
        this->ui->interpolateSurfaceCheckbox->setChecked(false);
        this->ui->interpolateSurfaceCheckbox->blockSignals(false);
        WuQMessageBox::errorOk(this,
                               "Both Tab 1 and Tab 2 must contain surface models.");
        return;
    }
    
    ModelSurface *ms1 = btc1->getDisplayedSurfaceModel();
    
    //    int32_t tabIndex2 = btc2->getTabNumber();
    ModelSurface *ms2 = btc2->getDisplayedSurfaceModel();
    
    if(!(ms1&&ms2)) return;
    
    if (ms1->getSurface()->getStructure() != ms2->getSurface()->getStructure()) {
        this->ui->interpolateSurfaceCheckbox->blockSignals(true);
        this->ui->interpolateSurfaceCheckbox->setChecked(false);
        this->ui->interpolateSurfaceCheckbox->blockSignals(false);
        WuQMessageBox::errorOk(this,
                               "Surfaces in Tab 1 and Tab 2 must be the same structure.");
        return;
    }
}



void MovieDialog::on_animateButton_toggled(bool checked)
{
//    this->renderMovieButton->setChecked(status);

    dx = this->ui->rotateXSpinBox->value();
    dy = this->ui->rotateYSpinBox->value();
    dz = this->ui->rotateZSpinBox->value();
    frameCount = this->ui->rotateFrameCountSpinBox->value();
    reverseDirection = this->ui->reverseDirectionCheckBox->isChecked();
    frameCountEnabled = this->ui->rotateFrameCountSpinBox->value() ? true : false;

    m_interpolationEnabled = this->ui->interpolateSurfaceCheckbox->isChecked();
    m_interpolationSteps = this->ui->interpolationStepsSpinBox->value();
	
    if(checked)
    {
        //this->renderMovieButton->setText("Stop");
		this->m_animationStarted = true;
		this->m_isInterpolating = true;
        while(this->ui->animateButton->isChecked())
        {            
            EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows(true).getPointer());
            QCoreApplication::instance()->processEvents();
			m_animationStarted = false;
        }
		this->m_animationStarted = false;
    }
    else
    {        
        
        //this->renderMovieButton->setText("Play");
		this->CleanupInterpolation();
		EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows(true).getPointer());
    }
    
    rotate_frame_number = 0;
}

void MovieDialog::on_recordButton_toggled(bool checked)
{
    if(checked)
    {
        m_useCustomSize = ui->customSizeRadioButton->isChecked();
        imageX = 0;
        imageY = 0;
        if(m_useCustomSize)
        {
            imageX = ui->windowWidthSpinBox->value();
            imageY = ui->windowHeightSpinBox->value();
        }
    }
    if(!checked&&(frame_number > 0))
    {
        //render frames....

        QString formatString("Movie Files (*.mpg *.mp4)");

        AString fileName = QFileDialog::getSaveFileName( this, tr("Save File"),QString(), formatString );
        AString tempDir = QDir::tempPath();
        if ( !fileName.isEmpty() )
        {
//            int crop[4];
//            crop[0] = imageX;
//            crop[1] = imageY;
//            crop[2] = 0;
//            crop[3] = 0;            
//            if(!(crop[0]&&crop[1])) GuiManager::get()->getBrowserWindowByWindowIndex(m_browserWindowIndex)->getViewportSize(crop[0],crop[1]);
    

            unlink(fileName.toLatin1().data());
            CaretLogInfo("Rendering movie to:" + fileName);
            AString ffmpeg = SystemUtilities::getWorkbenchHome() + AString("/ffmpeg ");            

           // JWH ffmpeg = "/mnt/myelin/distribution/caret7_distribution/workbench//macosx64_apps/ffmpeg ";
            
            
            double frame_rate = 30.0/double(1 + this->ui->repeatFramesSpinBox->value());

            
//            if(ui->cropImageCheckBox->isChecked())
//            {                
//                this->getImageCrop(tempDir + "/movie0" + AString(".png"),crop);
//            }
//
//            crop[0] = (crop[0]/2)*2;
//            crop[1] = (crop[1]/2)*2;
//            CaretLogInfo("Resizing image from " + AString::number(imageX) + AString(":") + AString::number(imageY) + AString(" to ") +
//                         AString::number(crop[0]) + AString(":") + AString::number(crop[1]));
//
//            AString command = ffmpeg + AString("-threads 4 -r " + AString::number(frame_rate) + " -i "+ tempDir + "/movie%d.png -r 30 -q:v 1 -vf crop=" + 
//                AString::number(crop[0]) + ":" + AString::number(crop[1]) + ":" + 
//                AString::number(crop[2]) + ":" + AString::number(crop[3]) + " " + fileName);
            AString command = ffmpeg + AString("-threads 4 -r " + AString::number(frame_rate) + " -i "+ tempDir + "/movie%d.png -r 30 -q:v 1 "
                                               + fileName);
            CaretLogFine("running " + command);

            system(command.toLatin1().data());
            CaretLogFine("Finished rendering " + fileName);

        }
        for(int i = 0;i<frame_number;i++)
        {
            AString tempFile = tempDir + "/movie" + AString::number(i) + AString(".png");
            unlink(tempFile.toLatin1().data());
        }
        frame_number = 0;
    }
}

//void MovieDialog::getImageCrop(AString fileName, int *cropOut)
//{
//    const int marginSize = this->ui->marginSpinBox->value();
//    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
//    uint8_t backgroundColor[3];
//    prefs->getColorBackground(backgroundColor);
//
//    ImageFile file;
//    file.readFile(fileName);
//
//    int leftTopRightBottom[4];
//    file.findImageObject(backgroundColor,leftTopRightBottom);
//
//    const int width = leftTopRightBottom[2] - leftTopRightBottom[0] + 1;
//    const int height = leftTopRightBottom[3] - leftTopRightBottom[1] + 1;
//    cropOut[0] = width + 2*marginSize;
//    cropOut[1] = height + 2*marginSize;
//    cropOut[2] = leftTopRightBottom[0]+marginSize;
//    cropOut[3] = leftTopRightBottom[1]+marginSize;
//}

void MovieDialog::processRotateTransformation(const double dx, const double dy, const double dz)
{
    BrowserTabContent* browserTabContent = 
		GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
	if (browserTabContent == NULL) {
		return;
	}
    Model* modelController = browserTabContent->getModelForDisplay();
    if (modelController != NULL) {
//        const int32_t tabIndex = browserTabContent->getTabNumber();
            
        {
            Matrix4x4 rotationMatrix = browserTabContent->getRotationMatrix();
            rotationMatrix.rotateX(dx);
            rotationMatrix.rotateY(dy);
            rotationMatrix.rotateZ(dz);
            browserTabContent->setRotationMatrix(rotationMatrix);
            
//            /*
//             * There are several rotation matrix.  The 'NORMAL' matrix is used
//             * in most cases and others are used in special viewing modes
//             * such as surface montage and right/left lateral medial yoking
//             */ 
//            if (browserTabContent->isDisplayedModelSurfaceRightLateralMedialYoked()) {
//                Matrix4x4* rotationMatrix = modelController->getViewingRotationMatrix(tabIndex, 
//                                                                                      Model::VIEWING_TRANSFORM_NORMAL);
//                rotationMatrix->rotateX(dx);
//                rotationMatrix->rotateY(dy);
//                rotationMatrix->rotateZ(dz);
//                
//                /*
//                 * Matrix for a right medial/lateral yoked surface
//                 */
//                Matrix4x4* rotationMatrixRightLatMedYoked = modelController->getViewingRotationMatrix(tabIndex, 
//                                                                                                      Model::VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED);
//                rotationMatrixRightLatMedYoked->rotateX(-dx);
//                rotationMatrixRightLatMedYoked->rotateY(-dy);
//                rotationMatrixRightLatMedYoked->rotateZ(-dz);
//            }
//            else {               
//                
//                
//                Matrix4x4* rotationMatrix = modelController->getViewingRotationMatrix(tabIndex, 
//                                                                                      Model::VIEWING_TRANSFORM_NORMAL);
//                rotationMatrix->rotateX(-dx);
//                rotationMatrix->rotateY(dy);
//                rotationMatrix->rotateZ(dz);
//                
//                /*
//                 * Matrix for a left surface opposite view in surface montage
//                 */
//                Matrix4x4* rotationMatrixSurfMontLeftOpp = modelController->getViewingRotationMatrix(tabIndex, 
//                                                                                                     Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE);
//                rotationMatrixSurfMontLeftOpp->rotateX(-dx);
//                rotationMatrixSurfMontLeftOpp->rotateY(dy);
//                rotationMatrixSurfMontLeftOpp->rotateZ(dz);
//                
//                /*
//                 * Matrix for a right surface view in surface montage
//                 */
//                Matrix4x4* rotationMatrixSurfMontRight = modelController->getViewingRotationMatrix(tabIndex, 
//                                                                                                     Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT);
//                rotationMatrixSurfMontRight->rotateX(dx); 
//                rotationMatrixSurfMontRight->rotateY(-dy);
//                rotationMatrixSurfMontRight->rotateZ(dz);
//                
//                /*
//                 * Matrix for a right surface opposite view in surface montage
//                 */
//                Matrix4x4* rotationMatrixSurfMontRightOpp = modelController->getViewingRotationMatrix(tabIndex, 
//                                                                                                   Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE);
//                rotationMatrixSurfMontRightOpp->rotateX(dx);
//                rotationMatrixSurfMontRightOpp->rotateY(-dy);
//                rotationMatrixSurfMontRightOpp->rotateZ(dz);
//                
//                /*
//                 * Matrix for a right medial/lateral yoked surface
//                 */
//                Matrix4x4* rotationMatrixRightLatMedYoked = modelController->getViewingRotationMatrix(tabIndex, 
//                                                                                           Model::VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED);
//                rotationMatrixRightLatMedYoked->rotateX(dx);
//                rotationMatrixRightLatMedYoked->rotateY(-dy);
//                rotationMatrixRightLatMedYoked->rotateZ(dz);
//            }            
        }
    }
}

/**
 * Receive events from the event manager.
 * 
 * @param event
 *   Event sent by event manager.
 */
void 
MovieDialog::receiveEvent(Event* event)
{
    
    if(event->getEventType() == EventTypeEnum::EVENT_GRAPHICS_UPDATE_ALL_WINDOWS ||
       event->getEventType() == EventTypeEnum::EVENT_GRAPHICS_UPDATE_ONE_WINDOW    )
    {

        AString tempPath = QDir::tempPath();
        if(this->ui->recordButton->isChecked())
        {
            this->captureFrame(tempPath + AString("/movie") + AString::number(frame_number) + AString(".png"));

            AString temp = tempPath + AString("/movie") + AString::number(frame_number) + AString(".png");
            CaretLogFine(temp);
            CaretLogFine("frame number:" + QString::number(frame_number));
            frame_number++;
        }

        if(this->ui->animateButton->isChecked())
        {
			this->processUpdateVolumeSlice();
            this->processUpdateSurfaceInterpolation();

            if(frameCountEnabled && frameCount)
            {                
                if(!reverseDirection) 
                {
                    if(frameCount <= rotate_frame_number)
                    {
                        //this->ui->animateButton->setChecked(false);
                        dx = dy = dz = 0.0;
                        return;
                    }
                }
                else
                {
                    if(!(rotate_frame_number %frameCount) &&
                        (rotate_frame_number > 0))
                    {
                        dx *= -1.0;
                        dy *= -1.0;
                        dz *= -1.0;
                    }
                }
            }
            if(dx || dy || dz)
            {
                this->processRotateTransformation(dx, dy, dz);
            }
            rotate_frame_number++;
			
        }
    }
}

int32_t MovieDialog::getSliceDelta(const std::vector<int64_t> &dim, const caret::VolumeSliceViewPlaneEnum::Enum &vpe,
	                               const int32_t &sliceIndex)
{
	if(m_animationStarted)
	{
		m_volumeSliceIncrement = ui->sliceIncrementCountSpinBox->value();
		m_reverseVolumeSliceDirection = ui->reverseSliceDirectionCheckBox->isChecked();
        m_sliceIncrementIsNegative = (m_volumeSliceIncrement != abs(m_volumeSliceIncrement));
        
        switch(vpe)
		{
			case VolumeSliceViewPlaneEnum::AXIAL:
                if(m_sliceIncrementIsNegative)
                {                   
                    m_AEnd = sliceIndex;
                    m_AStart = 0>(m_AEnd+m_volumeSliceIncrement)?0:m_AEnd+m_volumeSliceIncrement;
                    dA=-1;
                }
                else
                {
				    m_AStart = sliceIndex;
    				m_AEnd = (dim[2]-1)<(m_AStart+m_volumeSliceIncrement)?(dim[2]-1):m_AStart+m_volumeSliceIncrement;
    				dA=1;
                }
				break;
			case VolumeSliceViewPlaneEnum::CORONAL:
                if(m_sliceIncrementIsNegative)
                {
                    m_CEnd = sliceIndex;
                    m_CStart = 0>(m_CEnd+m_volumeSliceIncrement)?0:m_CEnd+m_volumeSliceIncrement;
                    dC=-1;
                }
                else
                {
				    m_CStart = sliceIndex;
				    m_CEnd = (dim[1]-1)<(m_CStart+m_volumeSliceIncrement)?(dim[1]-1):m_CStart+m_volumeSliceIncrement;
				    dC=1;
                }
				break;

			case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                if(m_sliceIncrementIsNegative)
                {
                    m_PEnd = sliceIndex;
                    m_PStart = 0>(m_PEnd+m_volumeSliceIncrement)?0:m_PEnd+m_volumeSliceIncrement;
                    dP=-1;
                }
                else
                {
				    m_PStart = sliceIndex;
				    m_PEnd = (dim[0]-1)<(m_PStart+m_volumeSliceIncrement)?(dim[0]-1):m_PStart+m_volumeSliceIncrement;
				    dP=1;
                }
				break;
			default:
				break;
		}
		
		
	}
	switch(vpe) {
		case VolumeSliceViewPlaneEnum::AXIAL:
			if(dA > 0 ) //the current increment is positive
			{
			    if((m_AEnd-sliceIndex) > 0) //there is still room to increment by one
				{
					return dA;
				}
				else if(m_reverseVolumeSliceDirection)
				{
					return dA = -dA;
				}
				else
				{
					return 0;
				}
			}
		    else if(dA < 0)
			{
				if((sliceIndex-m_AStart) > 0)
				{
					return dA;
				}
				else if(m_reverseVolumeSliceDirection) 
				{
					return dA = -dA;
				}
                else
                {
                    return 0;
                }
			}
			
			return dA;
			
			break;
		case VolumeSliceViewPlaneEnum::CORONAL:
			if(dC > 0 ) //the current increment is positive
			{
				if((m_CEnd-sliceIndex) > 0) //there is still room to increment by one
				{
					return dC;
				}
				else if(m_reverseVolumeSliceDirection)
				{
					return dC = -dC;
				}
				else
				{
					return 0;
				}
			}
			else if(dC < 0)
			{
				if((sliceIndex-m_CStart) > 0)
				{
					return dC;
				}
				else if(m_reverseVolumeSliceDirection) 
				{
					return dC = -dC;
				}
                else
                {
                    return 0;
                }
			}

			return dC;
			break;

		case VolumeSliceViewPlaneEnum::PARASAGITTAL:
			if(dP > 0 ) //the current increment is positive
			{
				if((m_PEnd-sliceIndex) > 0) //there is still room to increment by one
				{
					return dP;
				}
				else if(m_reverseVolumeSliceDirection)
				{
					return dP = -dP;
				}
				else
				{
					return 0;
				}
			}
			else if(dP < 0)
			{
				if((sliceIndex-m_PStart) > 0)
				{
					return dP;
				}
				else if(m_reverseVolumeSliceDirection) 
				{
					return dP = -dP;
                }
                else
                {
                    return 0;
                }
			}
			return dP;

			break;
		default:
			break;
	}

    return 0;	

}

void MovieDialog::processUpdateVolumeSlice()
{
	BrainBrowserWindow *bw = GuiManager::get()->getBrowserWindowByWindowIndex(m_browserWindowIndex);
    if(!bw)
    {
        CaretLogInfo("Invalid browser window index, " + AString::number(m_browserWindowIndex));
        ui->animateButton->setChecked(false);
        return;
    }
	BrowserTabContent *btc = bw->getBrowserTabContent();
	int32_t tabIndex = btc->getTabNumber();
	ModelVolume* mv = btc->getDisplayedVolumeModel();
	if(mv == NULL) 
	{
		return;
	}

    switch (btc->getSliceProjectionType()) {
        case caret::VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
            return;
            break;
        case caret::VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
            break;
    }


//	VolumeSliceCoordinateSelection* vscs = btc->getSelectedVolumeSlices();
	VolumeMappableInterface* vf = mv->getUnderlayVolumeFile(tabIndex);
	std::vector<int64_t> dim;
	vf->getDimensions(dim);
	VolumeSliceViewPlaneEnum::Enum vpe = btc->getSliceViewPlane();
	
	if(vpe == VolumeSliceViewPlaneEnum::ALL ||
	   vpe == VolumeSliceViewPlaneEnum::AXIAL)
	{		
		int64_t sliceIndex = btc->getSliceIndexAxial(vf);
		sliceIndex += this->getSliceDelta(dim,VolumeSliceViewPlaneEnum::AXIAL,sliceIndex);
		btc->setSliceIndexAxial(vf,sliceIndex);
	}
	if(vpe == VolumeSliceViewPlaneEnum::ALL ||
	   vpe == VolumeSliceViewPlaneEnum::CORONAL)
	{
		int64_t sliceIndex = btc->getSliceIndexCoronal(vf);
		sliceIndex += this->getSliceDelta(dim,VolumeSliceViewPlaneEnum::CORONAL,sliceIndex);
		btc->setSliceIndexCoronal(vf,sliceIndex);
	}
	if(vpe == VolumeSliceViewPlaneEnum::ALL ||
		vpe == VolumeSliceViewPlaneEnum::PARASAGITTAL)
	{
		int64_t sliceIndex = btc->getSliceIndexParasagittal(vf);
		sliceIndex += this->getSliceDelta(dim,VolumeSliceViewPlaneEnum::PARASAGITTAL,sliceIndex);
		btc->setSliceIndexParasagittal(vf,sliceIndex);
	}
}

void MovieDialog::captureFrame(AString filename)
{
    

    ImageFile imageFile;
    QApplication::setOverrideCursor(QCursor(Qt::BlankCursor));
//    bool valid = GuiManager::get()->captureImageOfBrowserWindowGraphicsArea(m_browserWindowIndex,
//        imageX,
//        imageY,
//        imageFile,
//        false);
    EventImageCapture imageCaptureEvent(m_browserWindowIndex,
                                        0,
                                        0,
                                        0,
                                        0,
                                        imageX,
                                        imageY);
    EventManager::get()->sendEvent(imageCaptureEvent.getPointer());
    
    bool valid = true;
    AString errorMessage;
    if (imageCaptureEvent.getEventProcessCount() <= 0) {
        errorMessage = "Invalid window selected";
        valid = false;
    }
    else if (imageCaptureEvent.isError()) {
        errorMessage = imageCaptureEvent.getErrorMessage();
        valid = false;
    }
    
    imageFile.setFromQImage(imageCaptureEvent.getImage());
    QApplication::restoreOverrideCursor();


    if (valid == false) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
//            "Invalid window selected");
        ui->recordButton->setChecked(false);
        return;
    }

    /*if (ui->cropImageCheckBox->isChecked()) {
        const int marginSize = ui->marginSpinBox->value();
        CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
        uint8_t backgroundColor[3];
        prefs->getColorBackground(backgroundColor);
        imageFile.cropImageRemoveBackground(marginSize, backgroundColor);
        croppedImageX = imageFile.getAsQImage()->size().height();
        croppedImageY = imageFile.getAsQImage()->size().width();
    }*/

    std::vector<AString> readImageFileExtensions, writeImageFileExtensions;
    AString defaultImageExtension;
    ImageFile::getWorkbenchSupportedImageFileExtensions(readImageFileExtensions,
                                                        writeImageFileExtensions,
                                                        defaultImageExtension);

    bool validExtension = false;
    for (std::vector<AString>::iterator extensionIterator = writeImageFileExtensions.begin();
        extensionIterator != writeImageFileExtensions.end();
        extensionIterator++) {
            if (filename.endsWith(*extensionIterator)) {
                validExtension = true;
            }
    }

    if (validExtension == false) {
        if (defaultImageExtension.isEmpty() == false) {
            filename += ("." + defaultImageExtension);
        }
    }

    uint8_t backgroundColor[3];
    imageCaptureEvent.getBackgroundColor(backgroundColor);
    
    try {
        const int marginSize = this->ui->marginSpinBox->value();
        if (marginSize > 0) {
            imageFile.addMargin(marginSize,
                                backgroundColor);
        }
        imageFile.writeFile(filename);
    }
    catch (const DataFileException& /*e*/) {
        QString msg("Unable to save: " + filename);
        WuQMessageBox::errorOk(this, msg);
    }
}


void MovieDialog::on_workbenchWindowSpinBox_valueChanged(int arg1)
{
    m_browserWindowIndex = arg1-1;
}

void MovieDialog::processUpdateSurfaceInterpolation()
{

    if(!m_interpolationEnabled||!m_isInterpolating)
        return;

    BrainBrowserWindow *bw = GuiManager::get()->getBrowserWindowByWindowIndex(m_browserWindowIndex);
    if(!bw)
    {
        CaretLogInfo("Invalid browser window index, " + AString::number(m_browserWindowIndex));
        ui->animateButton->setChecked(false);
        return;
    }

    BrowserTabContent *btc1 = bw->getBrowserTabContent(0);
    BrowserTabContent *btc2 = bw->getBrowserTabContent(1);
    if(!btc1) return;
    if(!btc2) return;

    //    int32_t tabIndex1 = btc1->getTabNumber();
    if ((btc1->getSelectedModelType() != ModelTypeEnum::MODEL_TYPE_SURFACE)
        || (btc2->getSelectedModelType() != ModelTypeEnum::MODEL_TYPE_SURFACE)) {
        CaretLogInfo("Both Tab 1 and Tab 2 must contain surface models.");
        return;
    }
    
    ModelSurface *ms1 = btc1->getDisplayedSurfaceModel();

//    int32_t tabIndex2 = btc2->getTabNumber();
    ModelSurface *ms2 = btc2->getDisplayedSurfaceModel();
    
    if(!(ms1&&ms2)) return;

    if (ms1->getSurface()->getStructure() != ms2->getSurface()->getStructure()) {
        CaretLogInfo("Surfaces in Tab 1 and Tab 2 must be the same structure.");
        return;
    }

    if(m_interpolationIndex == 0)
    {
    
        m_surface1 = ms1->getSurface();
        m_surface2 = ms2->getSurface();

        int32_t coordCount1 = m_surface1->getNumberOfNodes();
        int32_t coordCount2 = m_surface2->getNumberOfNodes();
        if(coordCount1 != coordCount2) return;

        const float* coords1 = m_surface1->getCoordinateData();
        const float* coords2 = m_surface2->getCoordinateData();
        float center1[3];
		float center2[3];
		float centerdelta[3];
		m_surface1->getBoundingBox()->getCenter(center1);
		m_surface2->getBoundingBox()->getCenter(center2);
		for(int i = 0;i<3;i++)
		{
			centerdelta[i] = center2[i]-center1[i];
		}

        m_surfaceCoords2Back.clear();
        m_delta.clear();
        for(int32_t i = 0;i<coordCount1;i++)
        {
            m_surfaceCoords2Back.push_back(coords2[i*3]);
            m_surfaceCoords2Back.push_back(coords2[i*3+1]);
            m_surfaceCoords2Back.push_back(coords2[i*3+2]);            
            
            m_delta.push_back((coords2[i*3]-coords1[i*3]-centerdelta[0])/(double)m_interpolationSteps);
            m_delta.push_back((coords2[i*3+1]-coords1[i*3+1]-centerdelta[1])/(double)m_interpolationSteps);
            m_delta.push_back((coords2[i*3+2]-coords1[i*3+2]-centerdelta[2])/(double)m_interpolationSteps);
        }

		coords = new float[3*coordCount1];
		memcpy(coords,coords1,12*coordCount1);
		coordsCount = coordCount1;
		for(int i = 0;i<coordsCount;i++)
		{
			coords[i*3] += centerdelta[0];
			coords[i*3+1] += centerdelta[1];
			coords[i*3+2] += centerdelta[2];
		}
        m_surface2->setCoordinates(coords);
		m_surface2->invalidateNormals();
		m_surface2->computeNormals();
        btc1->getSurfaceModelSelector()->setSelectedSurfaceModel(btc2->getSurfaceModelSelector()->getSelectedSurfaceModel());
        btc1->getSurfaceModelSelector()->setSelectedStructure(btc2->getSurfaceModelSelector()->getSelectedStructure());
        EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());        
    }

    if(m_interpolationIndex <m_interpolationSteps)
    {
        for(int64_t i = 0;i<coordsCount*3;i++)
        {
            coords[i] += m_delta[i];
        }
        m_surface2->setCoordinates(coords);
		m_surface2->invalidateNormals();
		m_surface2->computeNormals();
        m_interpolationIndex++;        
    }
    else
    {
        CleanupInterpolation();
    }
}

void MovieDialog::CleanupInterpolation()
{
	if(!m_interpolationEnabled||!m_isInterpolating) 
		return;
    if (m_surface2 == NULL) {
        return;
    }
    
	for(int64_t i = 0;i<coordsCount*3;i++)
	{
		coords[i] = m_surfaceCoords2Back[i];
	}
	m_surface2->setCoordinates(coords);   
	m_surface2->invalidateNormals();
	m_surface2->computeNormals();
	m_isInterpolating = false;
	m_interpolationIndex = 0;
	if(!coords)
	{
		delete coords;
		coords = NULL;
		coordsCount = 0;
	}
    
    m_surface1 = NULL;
    m_surface2 = NULL;
}

