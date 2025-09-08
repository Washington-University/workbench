
/*LICENSE_START*/
/*
 *  Copyright (C) 2020 Washington University School of Medicine
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

#define __PREFERENCES_IMAGE_WIDGET_DECLARE__
#include "PreferencesImageWidget.h"
#undef __PREFERENCES_IMAGE_WIDGET_DECLARE__

    
/**
 * \class caret::PreferencesImageWidget
 * \brief Widget for recent file properties in preferences
 * \ingroup GuiQt
 */
#include <QComboBox>
#include <QGridLayout>
#include <QLabel>

#include "BrainOpenGLMediaDrawing.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretPreferences.h"
#include "EnumComboBoxTemplate.h"
#include "EventManager.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUserInterfaceUpdate.h"
#include "ImageFile.h"
#include "PreferencesDialog.h"
#include "WuQMessageBox.h"
#include "WuQTrueFalseComboBox.h"
#include "WuQtUtilities.h"

using namespace caret;

/**
 * Constructor.
 */
PreferencesImageWidget::PreferencesImageWidget(QWidget* parent)
: QWidget(parent)
{
    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setColumnStretch(2, 100);

    /*
     * CZI Dimension combo box
     */
    const QString dimTT("Preferred dimension for loading higher-resolution CZI Images.  "
                        "Larger size will take longer to load but will reduce the "
                        "frequency of loading higher-resolution data.");
    const QString dimensionToolTip(WuQtUtilities::createWordWrappedToolTipText(dimTT));
    
    m_cziDimensionComboBox = new QComboBox();
    m_cziDimensionComboBox->setToolTip(dimensionToolTip);
    
    std::vector<std::pair<int32_t, QString>> integerAndTextValues;
    CaretPreferences::getSupportedCziDimensions(integerAndTextValues);
    for (auto it : integerAndTextValues) {
        m_cziDimensionComboBox->addItem(it.second,
                                     QVariant(it.first));
    }
    QObject::connect(m_cziDimensionComboBox, QOverload<int>::of(&QComboBox::activated),
                     this, &PreferencesImageWidget::cziDimensionChanged);
    QLabel* dimensionLabel = PreferencesDialog::addWidgetToLayout(gridLayout,
                                                                  "CZI Dimension (w/h)",
                                                                  m_cziDimensionComboBox);
    
    dimensionLabel->setToolTip(dimensionToolTip);

    const AString cziDimNote("CZI Dimension: A larger dimension will take longer to load but the frequency of loading "
                             "will be reduced.  If the dimension is too small, higher resolution images "
                             "may not fill the display, especially on high-resolution displays.  Any files in memory "
                             "MUST be reloaded if this value is changed.");
    QLabel* cziDimNoteLabel(new QLabel(cziDimNote));
    cziDimNoteLabel->setWordWrap(true);
//    PreferencesDialog::addWidgetToLayout(gridLayout, cziDimNote, NULL);
    gridLayout->addWidget(cziDimNoteLabel, gridLayout->rowCount(), 0, 1, 2);
    
    /*
     * Texture compression
     */
    const AString compressToolTip("If an image size in memory is greater than this size "
                                  "(in megabytes), the image will be compressed when "
                                  "drawn in OpenGL.  This is a lossy compression method.  "
                                  "Changes do not take effect until image is reloaded.");
    const AString compressionLabelText("Compress Images Larger than "
                                       + AString::number(ImageFile::getTextureCompressionSizeMegabytes())
                                       + " Megabytes");
    m_imageFileTextureCompressionComboBox = new WuQTrueFalseComboBox("On",
                                                                     "Off",
                                                                     this);
    QObject::connect(m_imageFileTextureCompressionComboBox, &WuQTrueFalseComboBox::statusChanged,
                     this, &PreferencesImageWidget::textureCompressionStatusChanged);
    QLabel* compressionLabel = PreferencesDialog::addWidgetToLayout(gridLayout,
                                         compressionLabelText,
                                         m_imageFileTextureCompressionComboBox->getWidget());
    WuQtUtilities::setWordWrappedToolTip(compressionLabel,
                                         compressToolTip);
    WuQtUtilities::setWordWrappedToolTip(m_imageFileTextureCompressionComboBox->getWidget(),
                                         compressToolTip);
    
    /*
     * Image texture magnification filter
     */
    m_graphicsTextureMagnificationFilterEnumComboBox = new EnumComboBoxTemplate(this);
    m_graphicsTextureMagnificationFilterEnumComboBox->setup<GraphicsTextureMagnificationFilterEnum,GraphicsTextureMagnificationFilterEnum::Enum>();
    QObject::connect(m_graphicsTextureMagnificationFilterEnumComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &PreferencesImageWidget::graphicsTextureMagnificationFilterEnumComboBoxItemActivated);
    WuQtUtilities::setWordWrappedToolTip(m_graphicsTextureMagnificationFilterEnumComboBox->getWidget(),
                                         GraphicsTextureMagnificationFilterEnum::toToolTip());
    QLabel* magFilterLabel = PreferencesDialog::addWidgetToLayout(gridLayout,
                                                                  "Image Magnification Filter (Zoomed In)",
                                                                  m_graphicsTextureMagnificationFilterEnumComboBox->getWidget());
    WuQtUtilities::setWordWrappedToolTip(magFilterLabel,
                                         GraphicsTextureMagnificationFilterEnum::toToolTip());
    
    /*
     * Image texture minification filter
     */
    m_graphicsTextureMinificationFilterEnumComboBox = new EnumComboBoxTemplate(this);
    m_graphicsTextureMinificationFilterEnumComboBox->setup<GraphicsTextureMinificationFilterEnum,GraphicsTextureMinificationFilterEnum::Enum>();
    QObject::connect(m_graphicsTextureMinificationFilterEnumComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &PreferencesImageWidget::graphicsTextureMinificationFilterEnumComboBoxItemActivated);
    WuQtUtilities::setWordWrappedToolTip(m_graphicsTextureMinificationFilterEnumComboBox->getWidget(),
                                         GraphicsTextureMinificationFilterEnum::toToolTip());
    QLabel* minFilterLabel = PreferencesDialog::addWidgetToLayout(gridLayout,
                                                                  "Image Minification Filter (Zoomed Out)",
                                                                  m_graphicsTextureMinificationFilterEnumComboBox->getWidget());
    WuQtUtilities::setWordWrappedToolTip(minFilterLabel,
                                         GraphicsTextureMinificationFilterEnum::toToolTip());


    QLabel* noteLabel = new QLabel("Note: Image Magnification/Minification Filters are NOT saved in the user's preferences.  "
                                   "Therefore, any desired changes to these selections must be made each time "
                                   "the application is started.");
    noteLabel->setWordWrap(true);
    
    /*
     * Layouts
     */
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addLayout(gridLayout);
    layout->addWidget(noteLabel);
    layout->addStretch();
}

/**
 * Destructor.
 */
PreferencesImageWidget::~PreferencesImageWidget()
{
}

/*
 * Update the content in this widget
 * @param caretPreferences
 *    The caret preferences
 */
void
PreferencesImageWidget::updateContent(CaretPreferences* caretPreferences)
{
    m_preferences = caretPreferences;
    CaretAssert(m_preferences);
    const int32_t dimValue(m_preferences->getCziDimension());
    
    bool foundFlag(false);
    const int32_t numItems = m_cziDimensionComboBox->count();
    for (int32_t i = 0; i < numItems; i++) {
        if (dimValue == m_cziDimensionComboBox->itemData(i)) {
            m_cziDimensionComboBox->setCurrentIndex(i);
            foundFlag = true;
            break;
        }
    }
    
    if ( ! foundFlag) {
        CaretLogSevere("Unable to find CZI Dimension when updating Preferences Dialog.");
    }

    m_imageFileTextureCompressionComboBox->setStatus(m_preferences->isImageFileTextureCompressionEnabled());
    
    const GraphicsTextureMinificationFilterEnum::Enum minFilter  = BrainOpenGLMediaDrawing::getTextureMinificationFilter();
    const GraphicsTextureMagnificationFilterEnum::Enum magFilter = BrainOpenGLMediaDrawing::getTextureMagnificationFilter();
    m_graphicsTextureMagnificationFilterEnumComboBox->setSelectedItem<GraphicsTextureMagnificationFilterEnum,GraphicsTextureMagnificationFilterEnum::Enum>(magFilter);
    m_graphicsTextureMinificationFilterEnumComboBox->setSelectedItem<GraphicsTextureMinificationFilterEnum,GraphicsTextureMinificationFilterEnum::Enum>(minFilter);
    
}

/**
 * Called when dimension combo box is changed
 * @param index
 *    Index of item selected
 */
void
PreferencesImageWidget::cziDimensionChanged(int index)
{
    const int32_t dimension = m_cziDimensionComboBox->itemData(index).toInt();
    CaretAssert(m_preferences);
    m_preferences->setCziDimension(dimension);
    updateGraphicsAndUserInterface();
    
    WuQMessageBox::informationOk(this, "CZI files must be reloaded or wb_view restarted after changing this value.");
}

/**
 * Called when graphics magnification filter changed
 */
void
PreferencesImageWidget::graphicsTextureMagnificationFilterEnumComboBoxItemActivated()
{
    const GraphicsTextureMagnificationFilterEnum::Enum magFilter = m_graphicsTextureMagnificationFilterEnumComboBox->getSelectedItem<GraphicsTextureMagnificationFilterEnum,GraphicsTextureMagnificationFilterEnum::Enum>();
    BrainOpenGLMediaDrawing::setTextureMagnificationFilter(magFilter);
    updateGraphicsAndUserInterface();
    
}

/**
 * Called when graphics minification filter changed
 */
void
PreferencesImageWidget::graphicsTextureMinificationFilterEnumComboBoxItemActivated()
{
    const GraphicsTextureMinificationFilterEnum::Enum minFilter = m_graphicsTextureMinificationFilterEnumComboBox->getSelectedItem<GraphicsTextureMinificationFilterEnum,GraphicsTextureMinificationFilterEnum::Enum>();
    BrainOpenGLMediaDrawing::setTextureMinificationFilter(minFilter);
    updateGraphicsAndUserInterface();
}

/**
 * Update the graphics and the user interface
 */
void
PreferencesImageWidget::updateGraphicsAndUserInterface()
{
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}

/**
 * Called when texture compression status changed
 * @param status
 *    New status
 */
void
PreferencesImageWidget::textureCompressionStatusChanged(bool status)
{
    CaretAssert(m_preferences);
    m_preferences->setImageFileTextureCompressionEnabled(status);
}
                     
