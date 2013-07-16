
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

#include <limits>

#include <QApplication>
#include <QBoxLayout>
#include <QButtonGroup>
#include <QCheckBox>
#include <QClipboard>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QImageWriter>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>

#define __IMAGE_CAPTURE_DIALOG__H__DECLARE__
#include "ImageCaptureDialog.h"
#undef __IMAGE_CAPTURE_DIALOG__H__DECLARE__

#include "Brain.h"
#include "BrainBrowserWindow.h"
#include "CaretAssert.h"
#include "CaretPreferences.h"
#include "EnumComboBoxTemplate.h"
#include "EventBrowserWindowGraphicsRedrawn.h"
#include "EventManager.h"
#include "FileInformation.h"
#include "GuiManager.h"
#include "ImageFile.h"
#include "ImageResolutionUnitsEnum.h"
#include "ImageSizeUnitsEnum.h"
#include "SessionManager.h"
#include "CaretFileDialog.h"
#include "WuQFactory.h"
#include "WuQMessageBox.h"
#include "WuQTimedMessageDisplay.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::ImageCaptureDialog 
 * \brief Dialog for capturing images.
 * \ingroup GuiQt
 * 
 */

/**
 * Constructor for editing a palette selection.
 *
 * @param parent
 *    Parent widget on which this dialog is displayed.
 */
ImageCaptureDialog::ImageCaptureDialog(BrainBrowserWindow* parent)
: WuQDialogNonModal("Image Capture",
                    parent)
{
    setDeleteWhenClosed(false);

    /*
     * Use Apply button for image capture
     */
    setApplyButtonText("Capture");

    /*
     * Image Source
     */
    QWidget* imageSourceWidget = createImageSourceSection();
    
    /*
     * Image Size
     * Note: Label is updated when window size is updated
     */
    QWidget* imageDimensionsWidget = createImageDimensionsSection();
    
    /*
     * Image Options
     */
    QWidget* imageOptionsWidget = createImageOptionsSection();
    
    /*
     * Image Destination
     */
    QWidget* imageDestinationWidget = createImageDestinationSection();
        
    QWidget* w = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(w);
    layout->addWidget(imageSourceWidget);
    layout->addWidget(imageOptionsWidget);
    layout->addWidget(imageDimensionsWidget);
    layout->addWidget(imageDestinationWidget);
    
    setCentralWidget(w);
    
    /*
     * Make apply button the default button.
     */
    QPushButton* applyButton = getDialogButtonBox()->button(QDialogButtonBox::Apply);
    CaretAssert(applyButton);
    applyButton->setAutoDefault(true);
    applyButton->setDefault(true);
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_WINDOW_GRAPHICS_HAVE_BEEN_REDRAWN);
    updateBrowserWindowWidthAndHeightLabel();
}

/**
 * Destructor.
 */
ImageCaptureDialog::~ImageCaptureDialog()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * @return Create and return the image source section.
 */
QWidget*
ImageCaptureDialog::createImageSourceSection()
{
    QLabel* windowLabel = new QLabel("Workbench Window: ");
    m_windowSelectionSpinBox = WuQFactory::newSpinBox();
    m_windowSelectionSpinBox->setRange(1,
                                           BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS);
    m_windowSelectionSpinBox->setSingleStep(1);
    m_windowSelectionSpinBox->setFixedWidth(60);
    QObject::connect(m_windowSelectionSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(updateBrowserWindowWidthAndHeightLabel()));
    
    QGroupBox* groupBox = new QGroupBox("Image Source");
    QGridLayout* gridLayout = new QGridLayout(groupBox);
    gridLayout->addWidget(windowLabel, 0, 0);
    gridLayout->addWidget(m_windowSelectionSpinBox, 0, 1);
    gridLayout->setColumnStretch(0, 0);
    gridLayout->setColumnStretch(1, 0);
    gridLayout->setColumnStretch(1000, 100);
    
    return groupBox;
}

/**
 * @return Create and return the image options section.
 */
QWidget*
ImageCaptureDialog::createImageOptionsSection()
{
    m_imageAutoCropCheckBox = new QCheckBox("Automatically Crop Image");
    QLabel* imageAutoCropMarginLabel = new QLabel("   Margin");
    m_imageAutoCropMarginSpinBox = WuQFactory::newSpinBox();
    m_imageAutoCropMarginSpinBox->setMinimum(0);
    m_imageAutoCropMarginSpinBox->setMaximum(100000);
    m_imageAutoCropMarginSpinBox->setSingleStep(1);
    m_imageAutoCropMarginSpinBox->setValue(10);
    m_imageAutoCropMarginSpinBox->setMaximumWidth(100);
    
    QHBoxLayout* cropMarginLayout = new QHBoxLayout();
    cropMarginLayout->addWidget(imageAutoCropMarginLabel);
    cropMarginLayout->addWidget(m_imageAutoCropMarginSpinBox);
    cropMarginLayout->addStretch();
    
    QGroupBox* groupBox = new QGroupBox("Image Options");
    QVBoxLayout* gridLayout = new QVBoxLayout(groupBox);
    gridLayout->addWidget(m_imageAutoCropCheckBox);
    gridLayout->addLayout(cropMarginLayout);
    
    return groupBox;
}

/**
 * @return Create and return the image dimensions section.
 */
QWidget*
ImageCaptureDialog::createImageDimensionsSection()
{
    m_imageSizeWindowRadioButton = new QRadioButton("Size of Window");
    
    m_imageSizeCustomRadioButton = new QRadioButton("Custom");
    
    QButtonGroup* sizeButtonGroup = new QButtonGroup(this);
    sizeButtonGroup->addButton(m_imageSizeWindowRadioButton);
    sizeButtonGroup->addButton(m_imageSizeCustomRadioButton);
    m_imageSizeWindowRadioButton->setChecked(true);
    
    QLabel* customPixelsWidthLabel = new QLabel("Width:");
    QLabel* customPixelsHeightLabel = new QLabel("Height:");
    QLabel* customUnitsWidthLabel = new QLabel("Width:");
    QLabel* customUnitsHeightLabel = new QLabel("Height:");
    QLabel* customResolutionLabel = new QLabel("Resolution:");
    
    QLabel* pixelDimensionsLabel = new QLabel("Pixel Dimensions");
    m_customPixelsWidthSpinBox = WuQFactory::newSpinBox();
    m_customPixelsWidthSpinBox->setFixedWidth(80);
    m_customPixelsWidthSpinBox->setRange(1, 10000000);
    m_customPixelsWidthSpinBox->setSingleStep(1);
    m_customPixelsWidthSpinBox->setValue(2560);
    
    m_customPixelsHeightSpinBox = WuQFactory::newSpinBox();
    m_customPixelsHeightSpinBox->setFixedWidth(80);
    m_customPixelsHeightSpinBox->setRange(1, 10000000);
    m_customPixelsHeightSpinBox->setSingleStep(1);
    m_customPixelsHeightSpinBox->setValue(2048);
    
    QLabel* imageDimensionsLabel = new QLabel("Image Dimensions");
    m_customImageUnitsWidthSpinBox = WuQFactory::newDoubleSpinBox();
    m_customImageUnitsWidthSpinBox->setFixedWidth(80);
    m_customImageUnitsWidthSpinBox->setRange(0.0, 100000000.0);
    m_customImageUnitsWidthSpinBox->setSingleStep(0.1);
    m_customImageUnitsWidthSpinBox->setValue(2048);
    
    m_customImageUnitsHeightSpinBox = WuQFactory::newDoubleSpinBox();
    m_customImageUnitsHeightSpinBox->setFixedWidth(80);
    m_customImageUnitsHeightSpinBox->setRange(0.0, 100000000.0);
    m_customImageUnitsHeightSpinBox->setSingleStep(0.1);
    m_customImageUnitsHeightSpinBox->setValue(2048);
    
    m_customResolutionSpinBox = WuQFactory::newDoubleSpinBox();
    m_customResolutionSpinBox->setFixedWidth(80);
    m_customResolutionSpinBox->setRange(1, 1000000);
    m_customResolutionSpinBox->setSingleStep(1);
    m_customResolutionSpinBox->setValue(72);
    
    m_customSizeUnitsEnumComboBox = new EnumComboBoxTemplate(this);
    m_customSizeUnitsEnumComboBox->setup<ImageSizeUnitsEnum,ImageSizeUnitsEnum::Enum>();
    QObject::connect(m_customSizeUnitsEnumComboBox, SIGNAL(itemActivated()),
                     this, SLOT(imageSizeUnitsEnumComboBoxItemActivated()));
    
    m_customResolutionUnitsEnumComboBox = new EnumComboBoxTemplate(this);
    m_customResolutionUnitsEnumComboBox->setup<ImageResolutionUnitsEnum,ImageResolutionUnitsEnum::Enum>();
    QObject::connect(m_customResolutionUnitsEnumComboBox, SIGNAL(itemActivated()),
                    this, SLOT(imageResolutionUnitsEnumComboBoxItemActivated()));
    
    m_customScaleProportionallyCheckBox = new QCheckBox("Scale Proportionally");

    
    QWidget* pixelsSizeWidget = new QWidget();
    QGridLayout* pixelsSizeLayout = new QGridLayout(pixelsSizeWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(pixelsSizeLayout, 4, 0);
    int pixelsRow = 0;
    pixelsSizeLayout->addWidget(pixelDimensionsLabel,
                                pixelsRow, 0, 1, 2, Qt::AlignHCenter);
    pixelsRow++;
    pixelsSizeLayout->addWidget(customPixelsWidthLabel,
                                pixelsRow, 0);
    pixelsSizeLayout->addWidget(m_customPixelsWidthSpinBox,
                                pixelsRow, 1);
    pixelsRow++;
    pixelsSizeLayout->addWidget(customPixelsHeightLabel,
                                pixelsRow, 0);
    pixelsSizeLayout->addWidget(m_customPixelsHeightSpinBox,
                                pixelsRow, 1);
    pixelsRow++;
    pixelsSizeLayout->addWidget(WuQtUtilities::createHorizontalLineWidget(),
                                pixelsRow, 0, 1, 2);
    pixelsRow++;
    pixelsSizeLayout->addWidget(m_customScaleProportionallyCheckBox,
                                pixelsRow, 0, 1, 2, Qt::AlignLeft);
    pixelsRow++;
    pixelsSizeWidget->setSizePolicy(QSizePolicy::Fixed,
                                    QSizePolicy::Fixed);
    
    
    QWidget* imageUnitsWidget = new QWidget();
    QGridLayout* imageUnitsLayout = new QGridLayout(imageUnitsWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(imageUnitsLayout, 4, 0);
    int unitsRow = 0;
    imageUnitsLayout->addWidget(imageDimensionsLabel,
                                unitsRow, 0, 1, 3, Qt::AlignHCenter);
    unitsRow++;
    imageUnitsLayout->addWidget(customUnitsWidthLabel,
                                unitsRow, 0);
    imageUnitsLayout->addWidget(m_customImageUnitsWidthSpinBox,
                                unitsRow, 1);
    imageUnitsLayout->addWidget(m_customSizeUnitsEnumComboBox->getWidget(),
                                unitsRow, 2, 2, 1);
    unitsRow++;
    imageUnitsLayout->addWidget(customUnitsHeightLabel,
                                unitsRow, 0);
    imageUnitsLayout->addWidget(m_customImageUnitsHeightSpinBox,
                                unitsRow, 1);
    unitsRow++;
    imageUnitsLayout->addWidget(customResolutionLabel,
                                unitsRow, 0);
    imageUnitsLayout->addWidget(m_customResolutionSpinBox,
                                unitsRow, 1);
    imageUnitsLayout->addWidget(m_customResolutionUnitsEnumComboBox->getWidget(),
                                unitsRow, 2);
    unitsRow++;
    imageUnitsWidget->setSizePolicy(QSizePolicy::Fixed,
                                    QSizePolicy::Fixed);
    
    QHBoxLayout* pixelsAndImageLayout = new QHBoxLayout();
    pixelsAndImageLayout->addSpacing(20);
    pixelsAndImageLayout->addWidget(pixelsSizeWidget,
                                    0,
                                    Qt::AlignTop);
    pixelsAndImageLayout->addWidget(WuQtUtilities::createVerticalLineWidget());
    pixelsAndImageLayout->addWidget(imageUnitsWidget,
                                    0,
                                    Qt::AlignTop);
    
    QGroupBox* groupBox = new QGroupBox("Image Dimensions");
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->addWidget(m_imageSizeWindowRadioButton, 0, Qt::AlignLeft);
    layout->addWidget(m_imageSizeCustomRadioButton, 0, Qt::AlignLeft);
    layout->addLayout(pixelsAndImageLayout, 0);
    
    return groupBox;
}

/**
 * Gets called when the image resolution units are changed.
 */
void
ImageCaptureDialog::imageResolutionUnitsEnumComboBoxItemActivated()
{
    
}

/**
 * Gets called when the image size units are changed.
 */
void
ImageCaptureDialog::imageSizeUnitsEnumComboBoxItemActivated()
{
    
}

/**
 * @return Create and return the image destination section.
 */
QWidget*
ImageCaptureDialog::createImageDestinationSection()
{
    m_copyImageToClipboardCheckBox = new QCheckBox("Copy to Clipboard");
    m_copyImageToClipboardCheckBox->setChecked(true);
    m_saveImageToFileCheckBox = new QCheckBox("Save to File: " );
    m_imageFileNameLineEdit = new QLineEdit();
    m_imageFileNameLineEdit->setText("untitled.png");
    QPushButton* fileNameSelectionPushButton = new QPushButton("Choose File...");
    QObject::connect(fileNameSelectionPushButton, SIGNAL(clicked()),
                     this, SLOT(selectImagePushButtonPressed()));
    
    QGroupBox* groupBox = new QGroupBox("Image Destination");
    QGridLayout* gridLayout = new QGridLayout(groupBox);
    gridLayout->addWidget(m_copyImageToClipboardCheckBox, 0, 0, 1, 3);
    gridLayout->addWidget(m_saveImageToFileCheckBox, 1, 0);
    gridLayout->addWidget(m_imageFileNameLineEdit, 1, 1);
    gridLayout->addWidget(fileNameSelectionPushButton, 1, 2);
    
    return groupBox;
}

/**
 * Receive events from the event manager.
 *
 * @param event
 *   Event sent by event manager.
 */
void
ImageCaptureDialog::receiveEvent(Event* event)
{
    AString windowSizeText = "";
    
    if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_WINDOW_GRAPHICS_HAVE_BEEN_REDRAWN) {
        EventBrowserWindowGraphicsRedrawn* graphicsRedrawnEvent =
        dynamic_cast<EventBrowserWindowGraphicsRedrawn*>(event);
        CaretAssert(graphicsRedrawnEvent);
        graphicsRedrawnEvent->setEventProcessed();
        
        updateBrowserWindowWidthAndHeightLabel();
    }
}

/**
 * Update the radio containing the size of the window.
 */
void
ImageCaptureDialog::updateBrowserWindowWidthAndHeightLabel()
{
    AString windowSizeText = "Size of Window";
    
    const int selectedBrowserWindowIndex = m_windowSelectionSpinBox->value() - 1;
    BrainBrowserWindow* browserWindow = GuiManager::get()->getBrowserWindowByWindowIndex(selectedBrowserWindowIndex);
    
    if (browserWindow != NULL) {
        int32_t width, height;
        browserWindow->getGraphicsWidgetSize(width,
                                             height);
        windowSizeText += (" ("
                           + AString::number(width)
                           + ", "
                           + AString::number(height)
                           + ")");
    }
    else {
        windowSizeText += (" (Invalid Window Number)");
    }
    
    m_imageSizeWindowRadioButton->setText(windowSizeText);
}

/**
 * May be called to update the dialog's content.
 */
void 
ImageCaptureDialog::updateDialog()
{
    updateBrowserWindowWidthAndHeightLabel();
}

/**
 * Set the selected browser window to the browser window with the
 * given index.
 * @param browserWindowIndex
 *    Index of browser window.
 */
void
ImageCaptureDialog::setBrowserWindowIndex(const int32_t browserWindowIndex)
{
    m_windowSelectionSpinBox->setValue(browserWindowIndex + 1);
}


/**
 * Called when choose file pushbutton is pressed.
 */
void 
ImageCaptureDialog::selectImagePushButtonPressed()
{
    QString defaultFileName = m_imageFileNameLineEdit->text().trimmed();
    if (defaultFileName.isEmpty()) {
        defaultFileName = "untitled.png";
    }
    FileInformation fileInfo(m_imageFileNameLineEdit->text().trimmed());
    if (fileInfo.isRelative()) {
        FileInformation absFileInfo(GuiManager::get()->getBrain()->getCurrentDirectory(),
                                    m_imageFileNameLineEdit->text().trimmed());
        defaultFileName = absFileInfo.getFilePath();
    }
    
    std::vector<AString> imageFileFilters;
    AString defaultFileFilter;
    ImageFile::getImageFileFilters(imageFileFilters, 
                                   defaultFileFilter);
    QString filters;
    for (std::vector<AString>::iterator filterIterator = imageFileFilters.begin();
         filterIterator != imageFileFilters.end();
         filterIterator++) {
        if (filters.isEmpty() == false) {
            filters += ";;";
        }
        
        filters += *filterIterator;
    }
    
    AString name = CaretFileDialog::getSaveFileNameDialog(this,
                                                  "Choose File Name",
                                                  defaultFileName, //GuiManager::get()->getBrain()->getCurrentDirectory(),
                                                  filters,
                                                  &defaultFileFilter);
    if (name.isEmpty() == false) {
        m_imageFileNameLineEdit->setText(name.trimmed());
    }
}

/**
 * Called when the apply button is pressed.
 */
void
ImageCaptureDialog::applyButtonPressed()
{
    const int browserWindowIndex = m_windowSelectionSpinBox->value() - 1;
    
    int32_t imageX = 0;
    int32_t imageY = 0;
    if (m_imageSizeCustomRadioButton->isChecked()) {
        imageX = m_customPixelsWidthSpinBox->value();
        imageY = m_customPixelsHeightSpinBox->value();
    }
    ImageFile imageFile;
    bool valid = GuiManager::get()->captureImageOfBrowserWindowGraphicsArea(browserWindowIndex,
                                                                            imageX,
                                                                            imageY,
                                                                            imageFile);
    
    if (valid == false) {
        WuQMessageBox::errorOk(this, 
                               "Invalid window selected");
        return;
    }
    
    const ImageSizeUnitsEnum::Enum sizeUnits = m_customSizeUnitsEnumComboBox->getSelectedItem<ImageSizeUnitsEnum,ImageSizeUnitsEnum::Enum>();
    const ImageResolutionUnitsEnum::Enum resolutionUnits = m_customResolutionUnitsEnumComboBox->getSelectedItem<ImageResolutionUnitsEnum,ImageResolutionUnitsEnum::Enum>();
    
    if (m_imageAutoCropCheckBox->isChecked()) {
        const int marginSize = m_imageAutoCropMarginSpinBox->value();
        CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
        uint8_t backgroundColor[3];
        prefs->getColorBackground(backgroundColor);
        imageFile.cropImageRemoveBackground(marginSize, backgroundColor);
    }
    
    bool errorFlag = false;
    
    if (m_copyImageToClipboardCheckBox->isChecked()) {
        QApplication::clipboard()->setImage(*imageFile.getAsQImage(), QClipboard::Clipboard);
    }

    if (m_saveImageToFileCheckBox->isChecked()) {
        std::vector<AString> imageFileExtensions;
        AString defaultFileExtension;
        ImageFile::getImageFileExtensions(imageFileExtensions, 
                                          defaultFileExtension);
        
        AString filename = m_imageFileNameLineEdit->text().trimmed();
        
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
            errorFlag = true;
        }
    }
    
    if (errorFlag == false) {
        /*
         * Display over "Capture" (the renamed Apply) button.
         */
        QWidget* parent = getDialogButtonBox()->button(QDialogButtonBox::Apply);
        CaretAssert(parent);
        
        WuQtUtilities::playSound("sound_camera_shutter.wav");
        WuQTimedMessageDisplay::show(parent,
                                     2.0,
                                     "Image captured");
    }
}


