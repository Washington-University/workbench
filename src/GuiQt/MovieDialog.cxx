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

#include <QDir>
#include <QFileDialog>

#include "MovieDialog.h"
#include "ui_MovieDialog.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventManager.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "GuiManager.h"
#include "ImageFile.h"
#include <Matrix4x4.h>
#include "Model.h"
#include "WuQMessageBox.h"

using namespace caret;
MovieDialog::MovieDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MovieDialog)
{
    ui->setupUi(this);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_GRAPHICS_UPDATE_ONE_WINDOW);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_GRAPHICS_UPDATE_ALL_WINDOWS);

    browserWindowIndex = 0;
    frame_number = 0;
    rotate_frame_number = 0;
}

MovieDialog::~MovieDialog()
{
    delete ui;
    EventManager::get()->removeAllEventsFromListener(this);
}

void MovieDialog::on_closeButton_clicked()
{
    this->close();
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
    if(checked)
    {
        //this->renderMovieButton->setText("Stop");


        while(this->ui->animateButton->isChecked())
        {            
            EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows(true).getPointer());
            QCoreApplication::instance()->processEvents();
        }

    }
    /*else
    {        
        this->renderMovieButton->setText("Play");
    }*/
    rotate_frame_number = 0;
}

void MovieDialog::on_recordButton_toggled(bool checked)
{
    if(!checked&&(frame_number > 0))
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

            double frame_rate = 30.0/double(1 + this->ui->repeatFramesSpinBox->value());

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

void MovieDialog::on_cropImageCheckBox_toggled(bool checked)
{

}

void MovieDialog::processRotateTransformation(const double dx, const double dy, const double dz)
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
            CaretLogFine("frame number:" + frame_number);
            frame_number++;
        }

        if(this->ui->animateButton->isChecked())
        {

            if(frameCountEnabled && frameCount)
            {                
                if(!reverseDirection) 
                {
                    if(frameCount <= rotate_frame_number)
                    {
                        this->ui->animateButton->setChecked(false);
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



void MovieDialog::captureFrame(AString filename)
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

