
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
#include "ChartMatrixDisplayProperties.h"
#include "DataFileException.h"
#include "EnumComboBoxTemplate.h"
#include "EventBrowserWindowGraphicsRedrawn.h"
#include "EventGraphicsPaintSoonOneWindow.h"
#include "EventImageCapture.h"
#include "EventManager.h"
#include "FileInformation.h"
#include "GuiManager.h"
#include "ImageFile.h"
#include "ImageCaptureDialogSettings.h"
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
    m_imageDimensionsWidget = NULL;
    
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
    m_imageDimensionsWidget = createImageDimensionsSection();
    
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
    layout->addWidget(m_imageDimensionsWidget);
    layout->addWidget(imageOptionsWidget);
    layout->addWidget(imageDestinationWidget);
    
    setCentralWidget(w,
                     WuQDialog::SCROLL_AREA_NEVER);
    
    /*
     * Make apply button the default button.
     */
    QPushButton* applyButton = getDialogButtonBox()->button(QDialogButtonBox::Apply);
    CaretAssert(applyButton);
    applyButton->setAutoDefault(true);
    applyButton->setDefault(true);
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_WINDOW_GRAPHICS_HAVE_BEEN_REDRAWN);
    updateBrowserWindowWidthAndHeightLabel();
    
    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);

    updateDimensionsSection();
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
    m_windowSelectionSpinBox = new QSpinBox();
    m_windowSelectionSpinBox->setRange(1,
                                           BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS);
    m_windowSelectionSpinBox->setSingleStep(1);
    m_windowSelectionSpinBox->setFixedWidth(60);
    QObject::connect(m_windowSelectionSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(updateBrowserWindowWidthAndHeightLabel()));
    
    const QString cbTT = WuQtUtilities::createWordWrappedToolTipText("When Tab/Window Lock Aspect is selected, empty "
                                                                     "regions may appear in the graphics region.  Selection "
                                                                     "of this option will exclude these empty regions in "
                                                                     "the captured image.");
    m_windowCropToLockAspectRegionCheckBox = new QCheckBox("Crop to Tab/Window Lock Aspect Region");
    m_windowCropToLockAspectRegionCheckBox->setChecked(true);
    QObject::connect(m_windowCropToLockAspectRegionCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(cropToTabWindowLockAspectRegionClicked(bool)));
    m_windowCropToLockAspectRegionCheckBox->setToolTip(cbTT);

    
    QGroupBox* groupBox = new QGroupBox("Source");
    QGridLayout* gridLayout = new QGridLayout(groupBox);
    gridLayout->addWidget(windowLabel, 0, 0);
    gridLayout->addWidget(m_windowSelectionSpinBox, 0, 1, Qt::AlignLeft);
    gridLayout->addWidget(m_windowCropToLockAspectRegionCheckBox, 1, 0, 1, 2, Qt::AlignLeft);
    
    return groupBox;
}

/**
 * @return Create and return the image options section.
 */
QWidget*
ImageCaptureDialog::createImageOptionsSection()
{
    const AString cropToolTip("Using the Window's background color, crop the image to "
                              "the content of the graphics region");
    m_imageAutoCropCheckBox = new QCheckBox("Automatically Crop Image");
    QObject::connect(m_imageAutoCropCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(imageCroppingCheckBoxClicked(bool)));
    m_imageAutoCropCheckBox->setToolTip(cropToolTip);

    const AString marginToolTip("Add a margin in the window's background color "
                                "around the captured region.");
    QLabel* imageMarginLabel = new QLabel("Margin");
    m_imageMarginSpinBox = new QSpinBox();
    QObject::connect(m_imageMarginSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(imageMarginValueChanged(int)));
    m_imageMarginSpinBox->setMinimum(0);
    m_imageMarginSpinBox->setMaximum(100);
    m_imageMarginSpinBox->setSingleStep(1);
    m_imageMarginSpinBox->setMaximumWidth(100);
    
    QHBoxLayout* cropMarginLayout = new QHBoxLayout();
    cropMarginLayout->addWidget(imageMarginLabel);
    cropMarginLayout->addWidget(m_imageMarginSpinBox);
    cropMarginLayout->addStretch();
    
    m_backgroundAlphaCheckBox = new QCheckBox("Background Alpha");
    QObject::connect(m_backgroundAlphaCheckBox, &QCheckBox::clicked,
                     this, &ImageCaptureDialog::backgroundAlphaCheckBoxClicked);
    
    m_backgroundAlphaSpinBox  = new QSpinBox();
    m_backgroundAlphaSpinBox->setRange(0, 255);
    m_backgroundAlphaSpinBox->setSingleStep(1);
    QObject::connect(m_backgroundAlphaSpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                     this, &ImageCaptureDialog::backgroundAlphaSpinBoxValueChanged);
    
    /*
     * Disable until implemented
     */
    m_backgroundAlphaCheckBox->setEnabled(false);
    m_backgroundAlphaSpinBox->setEnabled(false);
    
    QGroupBox* groupBox = new QGroupBox("Image Options");
    QGridLayout* groupLayout = new QGridLayout(groupBox);
    groupLayout->setColumnStretch(0, 0);
    groupLayout->setColumnStretch(1, 100);
    int groupRow(0);
    groupLayout->addWidget(m_imageAutoCropCheckBox, groupRow, 0);
    groupLayout->addWidget(m_imageMarginSpinBox, groupRow, 1, Qt::AlignLeft);
    ++groupRow;
    groupLayout->addWidget(m_backgroundAlphaCheckBox, groupRow, 0);
    groupLayout->addWidget(m_backgroundAlphaSpinBox, groupRow, 1, Qt::AlignLeft);
    ++groupRow;

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
    QObject::connect(sizeButtonGroup, SIGNAL(buttonClicked(QAbstractButton*)),
                     this, SLOT(sizeRadioButtonClicked(QAbstractButton*)));
    
    QLabel* customPixelsWidthLabel = new QLabel("Width:");
    QLabel* customPixelsHeightLabel = new QLabel("Height:");
    QLabel* customUnitsWidthLabel = new QLabel("Width:");
    QLabel* customUnitsHeightLabel = new QLabel("Height:");
    QLabel* customResolutionLabel = new QLabel("Resolution:");
    
    const int pixelSpinBoxWidth = 80;
    
    QLabel* pixelDimensionsLabel = new QLabel("Pixel Dimensions");
    m_pixelWidthSpinBox = new QSpinBox();
    m_pixelWidthSpinBox->setFixedWidth(pixelSpinBoxWidth);
    m_pixelWidthSpinBox->setRange(100, 10000000);
    m_pixelWidthSpinBox->setSingleStep(1);
    QObject::connect(m_pixelWidthSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(pixelWidthValueChanged(int)));
    
    m_pixelHeightSpinBox = new QSpinBox();
    m_pixelHeightSpinBox->setFixedWidth(pixelSpinBoxWidth);
    m_pixelHeightSpinBox->setRange(100, 10000000);
    m_pixelHeightSpinBox->setSingleStep(1);
    QObject::connect(m_pixelHeightSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(pixelHeightValueChanged(int)));
    
    const int imageSpinBoxWidth = 100;
    
    QLabel* imageDimensionsLabel = new QLabel("Image Dimensions");
    m_imageWidthSpinBox = new QDoubleSpinBox();
    m_imageWidthSpinBox->setFixedWidth(imageSpinBoxWidth);
    m_imageWidthSpinBox->setRange(0.01, 100000000.0);
    m_imageWidthSpinBox->setSingleStep(0.1);
    QObject::connect(m_imageWidthSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(imageWidthValueChanged(double)));
    
    m_imageHeightSpinBox = new QDoubleSpinBox();
    m_imageHeightSpinBox->setFixedWidth(imageSpinBoxWidth);
    m_imageHeightSpinBox->setRange(0.01, 100000000.0);
    m_imageHeightSpinBox->setSingleStep(0.1);
    QObject::connect(m_imageHeightSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(imageHeightValueChanged(double)));
    
    m_imageResolutionSpinBox = new QDoubleSpinBox();
    m_imageResolutionSpinBox->setFixedWidth(imageSpinBoxWidth);
    m_imageResolutionSpinBox->setRange(0.01, 1000000);
    m_imageResolutionSpinBox->setSingleStep(1);
    QObject::connect(m_imageResolutionSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(imageResolutionValueChanged(double)));

    std::vector<ImageSpatialUnitsEnum::Enum> spatialEnums;
    ImageSpatialUnitsEnum::getAllEnumsExcludingPixels(spatialEnums);
    m_imageSpatialUnitsEnumComboBox = new EnumComboBoxTemplate(this);
    m_imageSpatialUnitsEnumComboBox->setupWithItems<ImageSpatialUnitsEnum,ImageSpatialUnitsEnum::Enum>(spatialEnums);
    QObject::connect(m_imageSpatialUnitsEnumComboBox, SIGNAL(itemActivated()),
                     this, SLOT(imageSizeUnitsEnumComboBoxItemActivated()));
    
    m_imagePixelsPerSpatialUnitsEnumComboBox = new EnumComboBoxTemplate(this);
    m_imagePixelsPerSpatialUnitsEnumComboBox->setup<ImageResolutionUnitsEnum,ImageResolutionUnitsEnum::Enum>();
    QObject::connect(m_imagePixelsPerSpatialUnitsEnumComboBox, SIGNAL(itemActivated()),
                    this, SLOT(imageResolutionUnitsEnumComboBoxItemActivated()));
    
    m_scaleProportionallyCheckBox = new QCheckBox("Scale Proportionally");
    WuQtUtilities::setWordWrappedToolTip(m_scaleProportionallyCheckBox,
                                         ("If checked, the heights of the pixel and image dimensions "
                                          "are automatically adjusted so that their proportions "
                                          "match the proportion of the selected window's "
                                          "graphics region."));
    QObject::connect(m_scaleProportionallyCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(scaleProportionallyCheckBoxClicked(bool)));
    
    QWidget* pixelsSizeWidget = new QWidget();
    QGridLayout* pixelsSizeLayout = new QGridLayout(pixelsSizeWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(pixelsSizeLayout, 4, 0);
    int pixelsRow = 0;
    pixelsSizeLayout->addWidget(pixelDimensionsLabel,
                                pixelsRow, 0, 1, 2, Qt::AlignHCenter);
    pixelsRow++;
    pixelsSizeLayout->addWidget(customPixelsWidthLabel,
                                pixelsRow, 0);
    pixelsSizeLayout->addWidget(m_pixelWidthSpinBox,
                                pixelsRow, 1);
    pixelsRow++;
    pixelsSizeLayout->addWidget(customPixelsHeightLabel,
                                pixelsRow, 0);
    pixelsSizeLayout->addWidget(m_pixelHeightSpinBox,
                                pixelsRow, 1);
    pixelsRow++;
    pixelsSizeLayout->addWidget(WuQtUtilities::createHorizontalLineWidget(),
                                pixelsRow, 0, 1, 2);
    pixelsRow++;
    pixelsSizeLayout->addWidget(m_scaleProportionallyCheckBox,
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
    imageUnitsLayout->addWidget(m_imageWidthSpinBox,
                                unitsRow, 1);
    imageUnitsLayout->addWidget(m_imageSpatialUnitsEnumComboBox->getWidget(),
                                unitsRow, 2, 2, 1);
    unitsRow++;
    imageUnitsLayout->addWidget(customUnitsHeightLabel,
                                unitsRow, 0);
    imageUnitsLayout->addWidget(m_imageHeightSpinBox,
                                unitsRow, 1);
    unitsRow++;
    imageUnitsLayout->addWidget(customResolutionLabel,
                                unitsRow, 0);
    imageUnitsLayout->addWidget(m_imageResolutionSpinBox,
                                unitsRow, 1);
    imageUnitsLayout->addWidget(m_imagePixelsPerSpatialUnitsEnumComboBox->getWidget(),
                                unitsRow, 2);
    unitsRow++;
    imageUnitsWidget->setSizePolicy(QSizePolicy::Fixed,
                                    QSizePolicy::Fixed);
    
    m_customDimensionsWidget = new QWidget();
    QHBoxLayout* customDimensionsLayout = new QHBoxLayout(m_customDimensionsWidget);
    customDimensionsLayout->addSpacing(20);
    customDimensionsLayout->addWidget(pixelsSizeWidget,
                                    0,
                                    Qt::AlignTop);
    customDimensionsLayout->addWidget(WuQtUtilities::createVerticalLineWidget());
    customDimensionsLayout->addWidget(imageUnitsWidget,
                                    0,
                                    Qt::AlignTop);
    
    QLabel* imageBytesLabel = new QLabel("Uncompressed Image Memory Size: ");
    m_imageNumberOfBytesLabel = new QLabel("                    ");
    QWidget* imageBytesWidget = new QWidget();
    QHBoxLayout* imageBytesLayout = new QHBoxLayout(imageBytesWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(imageBytesLayout, 4, 0);
    imageBytesLayout->addWidget(imageBytesLabel);
    imageBytesLayout->addWidget(m_imageNumberOfBytesLabel);
    imageBytesLayout->addStretch();
    
    QGroupBox* groupBox = new QGroupBox("Dimensions");
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->addWidget(m_imageSizeWindowRadioButton, 0, Qt::AlignLeft);
    layout->addWidget(m_imageSizeCustomRadioButton, 0, Qt::AlignLeft);
    layout->addWidget(m_customDimensionsWidget, 0, Qt::AlignLeft);
    layout->addWidget(WuQtUtilities::createHorizontalLineWidget());
    layout->addWidget(imageBytesWidget);

    return groupBox;
}

/**
 * Gets called when the Window Size or Custom radio button is clicked.
 *
 * @param button
 *    Button that was clicked.
 */
void
ImageCaptureDialog::sizeRadioButtonClicked(QAbstractButton* /*button*/)
{
    ImageCaptureDialogSettings* imageCaptureSettings = SessionManager::get()->getImageCaptureDialogSettings();
    if (m_imageSizeCustomRadioButton->isChecked()) {
        imageCaptureSettings->setImageCaptureDimensionsMode(ImageCaptureDimensionsModeEnum::IMAGE_CAPTURE_DIMENSIONS_MODE_CUSTOM);
    }
    else if (m_imageSizeWindowRadioButton->isChecked()) {
        imageCaptureSettings->setImageCaptureDimensionsMode(ImageCaptureDimensionsModeEnum::IMAGE_CAPTURE_DIMENSIONS_MODE_WINDOW_SIZE);
    }
    
    updateDimensionsSection();
}

/**
 * Gets called when the image resolution units are changed.
 */
void
ImageCaptureDialog::imageResolutionUnitsEnumComboBoxItemActivated()
{
    ImageCaptureDialogSettings* imageCaptureSettings = SessionManager::get()->getImageCaptureDialogSettings();
    imageCaptureSettings->setImageResolutionUnits(m_imagePixelsPerSpatialUnitsEnumComboBox->getSelectedItem<ImageResolutionUnitsEnum, ImageResolutionUnitsEnum::Enum>());
    updateDimensionsSection();
}

/**
 * Gets called when the image size units are changed.
 */
void
ImageCaptureDialog::imageSizeUnitsEnumComboBoxItemActivated()
{
    ImageCaptureDialogSettings* imageCaptureSettings = SessionManager::get()->getImageCaptureDialogSettings();
    imageCaptureSettings->setSpatialUnits(m_imageSpatialUnitsEnumComboBox->getSelectedItem<ImageSpatialUnitsEnum, ImageSpatialUnitsEnum::Enum>());
    updateDimensionsSection();
}

/**
 * Gets called when Crop to Tab/Window Lock Aspect Region checkbox clicked
 *
 * @param checked
 *    New checked status.
 */
void
ImageCaptureDialog::cropToTabWindowLockAspectRegionClicked(bool checked)
{
    ImageCaptureDialogSettings* imageCaptureSettings = SessionManager::get()->getImageCaptureDialogSettings();
    imageCaptureSettings->setCropToTabWindowLockAspectRegionEnabled(checked);
    updateBrowserWindowWidthAndHeightLabel();
}


/**
 * May be called to update the dialog's content.
 */
void
ImageCaptureDialog::updateDialog()
{
    ImageCaptureDialogSettings* imageCaptureSettings = SessionManager::get()->getImageCaptureDialogSettings();
    m_windowCropToLockAspectRegionCheckBox->setChecked(imageCaptureSettings->isCropToTabWindowLockAspectRegionEnabled());
    
    updateBrowserWindowWidthAndHeightLabel();
    
    updateDimensionsSection();
    updateImageOptionsSection();
    updateDestinationSection();
}

/**
 * Update the section.
 */
void
ImageCaptureDialog::updateDimensionsSection()
{
    ImageCaptureDialogSettings* imageCaptureSettings = SessionManager::get()->getImageCaptureDialogSettings();
    
    CaretAssert(imageCaptureSettings);
    
    switch (imageCaptureSettings->getImageCaptureDimensionsMode()) {
        case ImageCaptureDimensionsModeEnum::IMAGE_CAPTURE_DIMENSIONS_MODE_CUSTOM:
            m_imageSizeCustomRadioButton->setChecked(true);
            m_customDimensionsWidget->setEnabled(true);
            break;
        case ImageCaptureDimensionsModeEnum::IMAGE_CAPTURE_DIMENSIONS_MODE_WINDOW_SIZE:
            m_imageSizeWindowRadioButton->setChecked(true);
            m_customDimensionsWidget->setEnabled(false);
            break;
    }
    
    m_pixelWidthSpinBox->blockSignals(true);
    m_pixelWidthSpinBox->setValue(imageCaptureSettings->getPixelWidth());
    m_pixelWidthSpinBox->blockSignals(false);
    
    m_pixelHeightSpinBox->blockSignals(true);
    m_pixelHeightSpinBox->setValue(imageCaptureSettings->getPixelHeight());
    m_pixelHeightSpinBox->blockSignals(false);
    
    m_scaleProportionallyCheckBox->setChecked(imageCaptureSettings->isScaleProportionately());
    
    m_imageWidthSpinBox->blockSignals(true);
    m_imageWidthSpinBox->setValue(imageCaptureSettings->getSpatialWidth());
    m_imageWidthSpinBox->blockSignals(false);
    
    m_imageHeightSpinBox->blockSignals(true);
    m_imageHeightSpinBox->setValue(imageCaptureSettings->getSpatialHeight());
    m_imageHeightSpinBox->blockSignals(false);
    
    m_imageResolutionSpinBox->blockSignals(true);
    m_imageResolutionSpinBox->setValue(imageCaptureSettings->getImageResolutionInSelectedUnits());
    m_imageResolutionSpinBox->blockSignals(false);
    
    m_imageSpatialUnitsEnumComboBox->setSelectedItem<ImageSpatialUnitsEnum,ImageSpatialUnitsEnum::Enum>(imageCaptureSettings->getSpatialUnits());
   
    m_imagePixelsPerSpatialUnitsEnumComboBox->setSelectedItem<ImageResolutionUnitsEnum, ImageResolutionUnitsEnum::Enum>(imageCaptureSettings->getImageResolutionUnits());

    AString windowSizeText = "Size of Window";
    
    int32_t width;
    int32_t height;
    float aspectRatio;
    if (getSelectedWindowWidthAndHeight(width,
                                        height,
                                        aspectRatio)) {
        windowSizeText += (" ("
                           + AString::number(width)
                           + " x "
                           + AString::number(height)
                           + " pixels)");
        
        if (imageCaptureSettings->isScaleProportionately()) {
            imageCaptureSettings->updateForAspectRatio(width,
                                                         height);
        }
        
        CaretAssert(m_imageDimensionsWidget);
        m_imageDimensionsWidget->setEnabled(true);
    }
    else {
        windowSizeText += (" (Invalid Window Number)");
        
        CaretAssert(m_imageDimensionsWidget);
        m_imageDimensionsWidget->setEnabled(false);
    }
    
    m_imageSizeWindowRadioButton->setText(windowSizeText);

    updateImageNumberOfBytesLabel();
}

/**
 * Update the section.
 */
void
ImageCaptureDialog::updateImageOptionsSection()
{
    ImageCaptureDialogSettings* imageCaptureSettings = SessionManager::get()->getImageCaptureDialogSettings();
    CaretAssert(imageCaptureSettings);
    
    m_imageAutoCropCheckBox->setChecked(imageCaptureSettings->isCroppingEnabled());
    QSignalBlocker marginSpinBoxLocker(m_imageMarginSpinBox);
    m_imageMarginSpinBox->setValue(imageCaptureSettings->getMargin());
    
    m_backgroundAlphaCheckBox->setChecked(imageCaptureSettings->isBackgroundAlphaEnabled());
    QSignalBlocker backgroundAlphaSpinBoxBlocker(m_backgroundAlphaSpinBox);
    m_backgroundAlphaSpinBox->setValue(imageCaptureSettings->getBackgroundAlpha());
}

/**
 * Update the section.
 */
void
ImageCaptureDialog::updateDestinationSection()
{
    ImageCaptureDialogSettings* imageCaptureSettings = SessionManager::get()->getImageCaptureDialogSettings();
    CaretAssert(imageCaptureSettings);
    
    m_copyImageToClipboardCheckBox->setChecked(imageCaptureSettings->isCopyToClipboardEnabled());
    m_saveImageToFileCheckBox->setChecked(imageCaptureSettings->isSaveToFileEnabled());
    m_imageFileNameLineEdit->setText(imageCaptureSettings->getImageFileName());
}

/**
 * Update the image number of bytes label.
 */
void
ImageCaptureDialog::updateImageNumberOfBytesLabel()
{
    int32_t imageWidth  = 0;
    int32_t imageHeight = 0;
    
    if (m_imageSizeCustomRadioButton->isChecked()) {
        imageWidth = m_pixelWidthSpinBox->value();
        imageHeight = m_pixelHeightSpinBox->value();
    }
    else if (m_imageSizeWindowRadioButton->isChecked()) {
        float aspectRatio = 0.0;
        if ( ! getSelectedWindowWidthAndHeight(imageWidth,
                                               imageHeight,
                                               aspectRatio)) {
            imageWidth  = 0;
            imageHeight = 0;
        }
    }
    
    const int64_t bytesPerPixel = 3;   // RGB
    const int64_t numberOfBytes = (static_cast<int64_t>(imageWidth)
                                   * static_cast<int64_t>(imageHeight)
                                   * bytesPerPixel);
    if (numberOfBytes > 0) {
        const AString sizeString = FileInformation::fileSizeToStandardUnits(numberOfBytes);
        m_imageNumberOfBytesLabel->setText(sizeString);
    }
    else {
        m_imageNumberOfBytesLabel->setText("Invalid/Unknown");
    }
}

/**
 * Update the radio containing the size of the window.
 */
void
ImageCaptureDialog::updateBrowserWindowWidthAndHeightLabel()
{
    ImageCaptureDialogSettings* imageCaptureSettings = SessionManager::get()->getImageCaptureDialogSettings();
    AString windowSizeText = "Size of Window";
    
    int32_t width;
    int32_t height;
    float aspectRatio;
    if (getSelectedWindowWidthAndHeight(width,
                                        height,
                                        aspectRatio)) {
        windowSizeText += (" ("
                           + AString::number(width)
                           + " x "
                           + AString::number(height)
                           + " pixels)");
        
        if (m_scaleProportionallyCheckBox->isChecked()) {
            imageCaptureSettings->updateForAspectRatio(width,
                                                         height);
        }
        
        updateDimensionsSection();
        
        CaretAssert(m_imageDimensionsWidget);
        m_imageDimensionsWidget->setEnabled(true);
    }
    else {
        windowSizeText += (" (Invalid Window Number)");
        
        CaretAssert(m_imageDimensionsWidget);
        m_imageDimensionsWidget->setEnabled(false);
    }
    
    m_imageSizeWindowRadioButton->setText(windowSizeText);
    
}

/**
 * Called when pixel width value is changed.
 *
 * @param value
 *    New value.
 */
void
ImageCaptureDialog::pixelWidthValueChanged(int value)
{
    ImageCaptureDialogSettings* imageCaptureSettings = SessionManager::get()->getImageCaptureDialogSettings();
    imageCaptureSettings->setPixelWidth(value);
    updateDimensionsSection();
}

/**
 * Called when pixel height value is changed.
 *
 * @param value
 *    New value.
 */
void
ImageCaptureDialog::pixelHeightValueChanged(int value)
{
    ImageCaptureDialogSettings* imageCaptureSettings = SessionManager::get()->getImageCaptureDialogSettings();
    imageCaptureSettings->setPixelHeight(value);
    updateDimensionsSection();
}

/**
 * Called when image width value is changed.
 *
 * @param value
 *    New value.
 */
void
ImageCaptureDialog::imageWidthValueChanged(double value)
{
    ImageCaptureDialogSettings* imageCaptureSettings = SessionManager::get()->getImageCaptureDialogSettings();
    imageCaptureSettings->setSpatialWidth(value);

    updateDimensionsSection();
}

/**
 * Called when image height value is changed.
 *
 * @param value
 *    New value.
 */
void
ImageCaptureDialog::imageHeightValueChanged(double value)
{
    ImageCaptureDialogSettings* imageCaptureSettings = SessionManager::get()->getImageCaptureDialogSettings();
    imageCaptureSettings->setSpatialHeight(value);
    
    updateDimensionsSection();
}

/**
 * Called when image resolution value changed.
 * 
 * @param value
 *    New value.
 */
void
ImageCaptureDialog::imageResolutionValueChanged(double value)
{
    ImageCaptureDialogSettings* imageCaptureSettings = SessionManager::get()->getImageCaptureDialogSettings();
    imageCaptureSettings->setImageResolutionInSelectedUnits(value);
    
    updateDimensionsSection();
    
}

/**
 * Called when scale proportionately check box clicked.
 *
 * @parm checked
 *     New checked status.
 */
void
ImageCaptureDialog::scaleProportionallyCheckBoxClicked(bool checked)
{
    ImageCaptureDialogSettings* imageCaptureSettings = SessionManager::get()->getImageCaptureDialogSettings();
    imageCaptureSettings->setScaleProportionately(checked);
    
    if (checked) {
        /*
         * Will cause pixel height to change appropriately
         */
        int32_t windowWidth;
        int32_t windowHeight;
        float aspectRatio;
        if (getSelectedWindowWidthAndHeight(windowWidth,
                                            windowHeight,
                                            aspectRatio)) {
            imageCaptureSettings->updateForAspectRatio(windowWidth,
                                                         windowHeight);
            updateDimensionsSection();
        }
    }
}

/**
 * Called when value for cropping margin is changed.
 *
 * @parm value
 *     New value for cropping margin.
 */
void
ImageCaptureDialog::imageMarginValueChanged(int value)
{
    ImageCaptureDialogSettings* imageCaptureSettings = SessionManager::get()->getImageCaptureDialogSettings();
    imageCaptureSettings->setMargin(value);
}

/**
 * Called when scale proportionately check box clicked.
 *
 * @parm checked
 *     New checked status.
 */
void
ImageCaptureDialog::imageCroppingCheckBoxClicked(bool checked)
{
    ImageCaptureDialogSettings* imageCaptureSettings = SessionManager::get()->getImageCaptureDialogSettings();
    imageCaptureSettings->setCroppingEnabled(checked);
}


/**
 * Called when background alpha check box clicked.
 *
 * @parm checked
 *     New checked status.
 */
void
ImageCaptureDialog::backgroundAlphaCheckBoxClicked(bool checked)
{
    ImageCaptureDialogSettings* imageCaptureSettings = SessionManager::get()->getImageCaptureDialogSettings();
    imageCaptureSettings->setBackgroundAlphaEnabled(checked);
}

/**
 * Called when background alpha spin box value changed.
 *
 * @parm value
 *     New alpha.
 */
void
ImageCaptureDialog::backgroundAlphaSpinBoxValueChanged(int value)
{
    ImageCaptureDialogSettings* imageCaptureSettings = SessionManager::get()->getImageCaptureDialogSettings();
    imageCaptureSettings->setBackgroundAlpha(value);
}

/**
 * Called when scale proportionately check box clicked.
 *
 * @parm checked
 *     New checked status.
 */
void
ImageCaptureDialog::copyImageToClipboardCheckBoxClicked(bool checked)
{
    ImageCaptureDialogSettings* imageCaptureSettings = SessionManager::get()->getImageCaptureDialogSettings();
    imageCaptureSettings->setCopyToClipboardEnabled(checked);
}

/**
 * Called when scale proportionately check box clicked.
 *
 * @parm checked
 *     New checked status.
 */
void
ImageCaptureDialog::saveImageToFileCheckBoxClicked(bool checked)
{
    ImageCaptureDialogSettings* imageCaptureSettings = SessionManager::get()->getImageCaptureDialogSettings();
    imageCaptureSettings->setSaveToFileEnabled(checked);
}

/**
 * @return Create and return the image destination section.
 */
QWidget*
ImageCaptureDialog::createImageDestinationSection()
{
    m_copyImageToClipboardCheckBox = new QCheckBox("Copy to Clipboard");
    QObject::connect(m_copyImageToClipboardCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(copyImageToClipboardCheckBoxClicked(bool)));
    m_saveImageToFileCheckBox = new QCheckBox("Save to File: " );
    QObject::connect(m_saveImageToFileCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(saveImageToFileCheckBoxClicked(bool)));
    m_imageFileNameLineEdit = new QLineEdit();
    m_imageFileNameLineEdit->setText("untitled.png");
    QObject::connect(m_imageFileNameLineEdit, SIGNAL(editingFinished()),
                     this, SLOT(imageFileNameValueChanged()));
    QPushButton* fileNameSelectionPushButton = new QPushButton("Choose File...");
    QObject::connect(fileNameSelectionPushButton, SIGNAL(clicked()),
                     this, SLOT(selectImagePushButtonPressed()));
    
    QGroupBox* groupBox = new QGroupBox("Destination");
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
 * Get the width and height of the selected window.
 *
 * @param xOut
 *    X-Offset of window graphics
 * @param yOut
 *    Y-Offset of window graphics
 * @param widthOut
 *    Width of window with aspect applied.
 * @param heightOut
 *    Height of window with aspect applied
 * @param int32_t& graphicsWidthOut
 *    Width of graphics without aspect applied.
 * @param int32_t& graphicsHeightOut
 *    Height of graphics without aspect applied.
 * @param aspectRatioOut
 *    Aspect ratio of window (height / width)
 * @return
 *    True if selected window is valid and both height and width
 *    are greater than zero, else false.
 */
bool
ImageCaptureDialog::getSelectedWindowCoordsWidthAndHeight(int32_t& xOut,
                                                          int32_t& yOut,
                                                          int32_t& widthOut,
                                                          int32_t& heightOut,
                                                          int32_t& graphicsWidthOut,
                                                          int32_t& graphicsHeightOut,
                                                          float& aspectRatioOut) const
{
    xOut = 0;
    yOut = 0;
    widthOut  = 0;
    heightOut = 0;
    graphicsWidthOut  = 0;
    graphicsHeightOut = 0;
    
    const int selectedBrowserWindowIndex = m_windowSelectionSpinBox->value() - 1;
    BrainBrowserWindow* browserWindow = GuiManager::get()->getBrowserWindowByWindowIndex(selectedBrowserWindowIndex);
    
    if (browserWindow != NULL) {
        browserWindow->getGraphicsWidgetSize(xOut,
                                             yOut,
                                             widthOut,
                                             heightOut,
                                             graphicsWidthOut,
                                             graphicsHeightOut,
                                             m_windowCropToLockAspectRegionCheckBox->isChecked());
        if ((widthOut > 0)
            && (heightOut > 0)) {
            aspectRatioOut = (static_cast<float>(heightOut)
                              / static_cast<float>(widthOut));
            return true;
        }
    }
    
    return false;
}

/**
 * Get the width and height of the selected window.
 *
 * @param widthOut
 *    Width of window.
 * @param heightOut
 *    Height of window.
 * @param aspectRatioOut
 *    Aspect ratio of window (height / width)
 * @return 
 *    True if selected window is valid and both height and width
 *    are greater than zero, else false.
 */
bool
ImageCaptureDialog::getSelectedWindowWidthAndHeight(int32_t& widthOut,
                                                    int32_t& heightOut,
                                                    float& aspectRatioOut) const
{
    int32_t x = 0;
    int32_t y = 0;
    int32_t graphicsWidth  = 0;
    int32_t graphicsHeight = 0;
    return getSelectedWindowCoordsWidthAndHeight(x,
                                                 y,
                                                 widthOut,
                                                 heightOut,
                                                 graphicsWidth,
                                                 graphicsHeight,
                                                 aspectRatioOut);
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
        defaultFileName = absFileInfo.getAbsoluteFilePath();
    }
    
    std::vector<AString> imageFileFilters;
    AString defaultFileFilter;
    ImageFile::getSaveQFileDialogImageFilters(imageFileFilters,
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
                                                  defaultFileName,
                                                  filters,
                                                  &defaultFileFilter);
    if (name.isEmpty() == false) {
        name = DataFileTypeEnum::addFileExtensionIfMissing(name,
                                                           DataFileTypeEnum::IMAGE);
        ImageCaptureDialogSettings* imageCaptureSettings = SessionManager::get()->getImageCaptureDialogSettings();
        imageCaptureSettings->setImageFileName(name);
        updateDestinationSection();
    }
}

/**
 * Gets called when user changes name of the image file.
 */
void
ImageCaptureDialog::imageFileNameValueChanged()
{
    ImageCaptureDialogSettings* imageCaptureSettings = SessionManager::get()->getImageCaptureDialogSettings();
    imageCaptureSettings->setImageFileName(m_imageFileNameLineEdit->text().trimmed());
}


/**
 * Called when the apply button is pressed.
 */
void
ImageCaptureDialog::applyButtonClicked()
{
    ImageCaptureDialogSettings* imageCaptureSettings = SessionManager::get()->getImageCaptureDialogSettings();
    const int browserWindowIndex = m_windowSelectionSpinBox->value() - 1;
    
    int32_t windowX;
    int32_t windowY;
    int32_t windowWidth;
    int32_t windowHeight;
    int32_t widgetWidth;
    int32_t widgetHeight;
    float windowAspectRatio;
    if ( ! getSelectedWindowCoordsWidthAndHeight(windowX,
                                                 windowY,
                                                 windowWidth,
                                                 windowHeight,
                                                 widgetWidth,
                                                 widgetHeight,
                                                 windowAspectRatio)) {
        WuQMessageBox::errorOk(this,
                               "Failed to get window size");
        return;
    }
    
    AString imageFileName = m_imageFileNameLineEdit->text().trimmed();
    if (m_saveImageToFileCheckBox->isChecked()) {
        if (imageFileName.isEmpty()) {
            WuQMessageBox::errorOk(this, "Save to File name is empty.  Choose an Image File.");
            return;
        }
    }
    
    /*
     * Default to width of window that may exclude empty regions
     * caused by locking of aspect ratio.
     */
    int32_t imageWidth = windowWidth;
    int32_t imageHeight = windowHeight;
    if (m_imageSizeCustomRadioButton->isChecked()) {
        imageWidth  = m_pixelWidthSpinBox->value();
        imageHeight = m_pixelHeightSpinBox->value();
        
        if ((windowWidth > 0)
            && (windowHeight > 0)) {
            const float windowWidthScaling = (static_cast<float>(imageWidth)
                                              / static_cast<float>(windowWidth));
            const float windowHeightScaling = (static_cast<float>(imageHeight)
                                               / static_cast<float>(windowHeight));
            
            ChartMatrixDisplayProperties::setManualScaleModeWindowWidthHeightScaling(windowWidthScaling,
                                                                                     windowHeightScaling);
        }
    }
    
    const float pixelsPerCentimeter = imageCaptureSettings->getImageResolutionInCentimeters();
    const float pixelsPerMeter = pixelsPerCentimeter * 100;
    
    EventImageCapture imageCaptureEvent(browserWindowIndex,
                                        windowX,
                                        windowY,
                                        windowWidth,
                                        windowHeight,
                                        imageWidth,
                                        imageHeight);
    imageCaptureEvent.setOutputImageWidthAndHeight(imageWidth,
                                                   imageHeight,
                                                   ImageSpatialUnitsEnum::PIXELS,
                                                   ImageResolutionUnitsEnum::PIXELS_PER_CENTIMETER,
                                                   imageCaptureSettings->getImageResolutionInCentimeters());
    imageCaptureEvent.setMargin(imageCaptureSettings->getMargin());

    EventManager::get()->sendEvent(imageCaptureEvent.getPointer());
    bool errorFlag = false;
    if (imageCaptureEvent.getEventProcessCount() <= 0) {
        WuQMessageBox::errorOk(this,
                               "Invalid window selected");
        errorFlag = true;
    }
    else if (imageCaptureEvent.isError()) {
        WuQMessageBox::errorOk(this,
                               imageCaptureEvent.getErrorMessage());
        errorFlag = true;
    }
    
    if ( ! errorFlag) {
        uint8_t backgroundColor[3];
        imageCaptureEvent.getBackgroundColor(backgroundColor);
        ImageFile imageFile;
        imageFile.setFromQImage(imageCaptureEvent.getCapturedImage());
                
        imageFile.setDotsPerMeter(pixelsPerMeter,
                                  pixelsPerMeter);
        
        
        if (imageCaptureSettings->isCroppingEnabled()) {
            const int32_t marginSize = imageCaptureSettings->getMargin();
            imageFile.cropImageRemoveBackground(marginSize,
                                                backgroundColor);
        }
        
        if (m_copyImageToClipboardCheckBox->isChecked()) {
            QApplication::clipboard()->setImage(*imageFile.getAsQImage(),
                                                QClipboard::Clipboard);
        }

        if (m_saveImageToFileCheckBox->isChecked()) {
            std::vector<AString> readImageFileExtensions, writeImageFileExtensions;
            AString defaultImageExtension;
            ImageFile::getWorkbenchSupportedImageFileExtensions(readImageFileExtensions,
                                                                writeImageFileExtensions,
                                                                defaultImageExtension);
            
            CaretAssert( ! imageFileName.isEmpty());
            
            bool validExtension = false;
            for (std::vector<AString>::iterator extensionIterator = writeImageFileExtensions.begin();
                 extensionIterator != writeImageFileExtensions.end();
                 extensionIterator++) {
                if (imageFileName.endsWith(*extensionIterator)) {
                    validExtension = true;
                }
            }
            
            if ( ! validExtension) {
                if (defaultImageExtension.isEmpty() == false) {
                    imageFileName += ("." + defaultImageExtension);
                }
            }
            
            try {
                imageFile.writeFile(imageFileName);
                const AString emptySceneName;
                SessionManager::get()->getCaretPreferences()->addToRecentFilesAndOrDirectories(imageFileName,
                                                                                               emptySceneName);
            }
            catch (const DataFileException& /*e*/) {
                QString msg("Unable to save: " + imageFileName);
                WuQMessageBox::errorOk(this, msg);
                errorFlag = true;
            }
        }
    }
    
    ChartMatrixDisplayProperties::setManualScaleModeWindowWidthHeightScaling(1.0, 1.0);
    
    if ( ! errorFlag) {
        /*
         * Display over "Capture" (the renamed Apply) button.
         */
        QWidget* parent = getDialogButtonBox()->button(QDialogButtonBox::Apply);
        CaretAssert(parent);
        
        WuQTimedMessageDisplay::showNonModal(parent,
                                             3.0,
                                             "Image captured");
    }
    
    EventManager::get()->sendEvent(EventGraphicsPaintSoonOneWindow(browserWindowIndex).getPointer());
}


