
/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#define __MAP_SETTINGS_COLOR_BAR_WIDGET_DECLARE__
#include "MapSettingsColorBarWidget.h"
#undef __MAP_SETTINGS_COLOR_BAR_WIDGET_DECLARE__

#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QSpinBox>
#include <QVBoxLayout>

#include "AnnotationColorBar.h"
#include "CaretAssert.h"
#include "CaretMappableDataFile.h"
#include "EnumComboBoxTemplate.h"
#include "EventGetViewportSize.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventUserInterfaceUpdate.h"
#include "EventManager.h"
#include "MapSettingsColorBarPaletteOptionsWidget.h"
#include "NumericFormatModeEnum.h"
#include "Overlay.h"
#include "PaletteColorBarValuesModeEnum.h"
#include "PaletteColorMapping.h"
#include "WuQtUtilities.h"

using namespace caret;



/**
 * \class caret::MapSettingsColorBarWidget
 * \brief Contains controls for adjusting a color bar's attributes.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
MapSettingsColorBarWidget::MapSettingsColorBarWidget(QWidget* parent)
: QWidget(parent)
{
    m_colorBar = NULL;
    m_paletteColorMapping = NULL;
    
    QWidget* locationPositionWidget = this->createLocationPositionSection();
    
    QWidget* numericsWidget = this->createDataNumericsSection();
    
    m_paletteOptionsWidget = new MapSettingsColorBarPaletteOptionsWidget();
    
    
    QGridLayout* layout = new QGridLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 6, 6);
    layout->addWidget(locationPositionWidget, 0, 0, Qt::AlignLeft);
    layout->addWidget(numericsWidget, 1, 0, Qt::AlignLeft);
    layout->addWidget(m_paletteOptionsWidget);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    
    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);
}

/**
 * Destructor.
 */
MapSettingsColorBarWidget::~MapSettingsColorBarWidget()
{
}

///**
// * Update the content of widget.
// *
// * @param overlay
// *     Overlay for display in this widget.
// */
//void
//MapSettingsColorBarWidget::updateContent(Overlay* overlay)
//{
//    m_colorBar = NULL;
//    m_paletteColorMapping = NULL;
//    
//    if (overlay != NULL) {
//        CaretMappableDataFile* mapFile = NULL;
//        int32_t mapIndex = 0;
//        overlay->getSelectionData(mapFile,
//                                  mapIndex);
//        if (mapFile != NULL) {
//            if ((mapIndex >= 0)
//                && (mapIndex < mapFile->getNumberOfMaps())) {
//                if (mapFile->isMappedWithPalette()) {
//                    m_paletteColorMapping = mapFile->getMapPaletteColorMapping(mapIndex);
//                    m_colorBar = overlay->getColorBar();
//                }
//            }
//        }
//    }
//}

/**
 * Update the content of the widget.
 *
 * @param caretMappableDataFile
 *    Data file containing palette that is edited.
 * @param mapIndex
 *    Index of map for palette that is edited.
 */
void
MapSettingsColorBarWidget::updateContent(CaretMappableDataFile* caretMappableDataFile,
                                         const int32_t mapIndex,
                                         AnnotationColorBar* annotationColorBar,
                                         PaletteColorMapping* paletteColorMapping)
{
    m_caretMappableDataFile = caretMappableDataFile;
    m_mapIndex            = mapIndex;
    m_colorBar            = annotationColorBar;
    m_paletteColorMapping = paletteColorMapping;

    updateContentPrivate();
}

/**
 * Update the content of the widget.
 */
void
MapSettingsColorBarWidget::updateContentPrivate()
{
    bool enableWidget = false;
    
    if ((m_colorBar != NULL)
        && (m_paletteColorMapping != NULL)) {
        const AnnotationColorBarPositionModeEnum::Enum positionMode = m_colorBar->getPositionMode();
        m_annotationColorBarPositionModeEnumComboBox->setSelectedItem<AnnotationColorBarPositionModeEnum,AnnotationColorBarPositionModeEnum::Enum>(positionMode);
        
        const AnnotationCoordinateSpaceEnum::Enum coordinateSpace = m_colorBar->getCoordinateSpace();
        m_annotationCoordinateSpaceEnumComboBox->setSelectedItem<AnnotationCoordinateSpaceEnum,AnnotationCoordinateSpaceEnum::Enum>(coordinateSpace);
        
        std::vector<Annotation*> annotationVector;
        annotationVector.push_back(m_colorBar);
        
        std::vector<AnnotationOneCoordinateShape*> annotationTwoDimVector;
        annotationTwoDimVector.push_back(m_colorBar);

        m_paletteOptionsWidget->updateEditor(m_caretMappableDataFile,
                                             m_mapIndex);
        
        enableWidget = true;
    }
    
    
    this->updateColorBarAttributes();
    
    setEnabled(enableWidget);
}

/**
 * Called when a control is changed.
 */
void
MapSettingsColorBarWidget::applySelections()
{
    if (m_paletteColorMapping != NULL) {
        m_paletteColorMapping->setColorBarValuesMode(m_colorBarDataModeComboBox->getSelectedItem<PaletteColorBarValuesModeEnum, PaletteColorBarValuesModeEnum::Enum>());
        m_paletteColorMapping->setColorBarNumericFormatMode(m_colorBarNumericFormatModeComboBox->getSelectedItem<NumericFormatModeEnum, NumericFormatModeEnum::Enum>());
        m_paletteColorMapping->setColorBarPrecisionDigits(m_colorBarDecimalsSpinBox->value());
        m_paletteColorMapping->setColorBarNumericSubdivisionCount(m_colorBarNumericSubdivisionsSpinBox->value());
        m_paletteColorMapping->setColorBarShowTickMarksSelected(m_showTickMarksCheckBox->isChecked());
    }
    
    m_paletteOptionsWidget->applyOptions();
    
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}

/**
 * Gets called when a position mode combo box item is selected.
 */
void
MapSettingsColorBarWidget::annotationColorBarPositionModeEnumComboBoxItemActivated()
{
    const AnnotationColorBarPositionModeEnum::Enum positionMode = m_annotationColorBarPositionModeEnumComboBox->getSelectedItem<AnnotationColorBarPositionModeEnum,AnnotationColorBarPositionModeEnum::Enum>();
    if (m_colorBar != NULL) {
        m_colorBar->setPositionMode(positionMode);
        switch (positionMode) {
            case AnnotationColorBarPositionModeEnum::AUTOMATIC:
                m_colorBar->resetSizeAttributes();
                break;
            case AnnotationColorBarPositionModeEnum::MANUAL:
                break;
        }
        EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
    }
    updateContentPrivate();
}

/**
 * Gets called when a coordinate space combo box item is selected.
 */
void
MapSettingsColorBarWidget::annotationCoordinateSpaceEnumComboBoxItemActivated()
{
    const AnnotationCoordinateSpaceEnum::Enum newCoordinateSpace = m_annotationCoordinateSpaceEnumComboBox->getSelectedItem<AnnotationCoordinateSpaceEnum,AnnotationCoordinateSpaceEnum::Enum>();
    if (m_colorBar != NULL) {
        if (m_colorBar->getPositionMode() == AnnotationColorBarPositionModeEnum::MANUAL) {
            bool tabToWindowFlag(false);
            bool windowToTabFlag(false);
            if ((m_colorBar->getCoordinateSpace() == AnnotationCoordinateSpaceEnum::TAB)
                && (newCoordinateSpace == AnnotationCoordinateSpaceEnum::WINDOW)) {
                tabToWindowFlag = true;
            }
            if ((m_colorBar->getCoordinateSpace() == AnnotationCoordinateSpaceEnum::WINDOW)
                && (newCoordinateSpace == AnnotationCoordinateSpaceEnum::TAB)) {
                windowToTabFlag = true;
            }
            
            if (tabToWindowFlag
                || windowToTabFlag) {
                /*
                 * Transitioning  between TAB to WINDOW space
                 * Change percentage width/height so pixel width/height
                 * does not change
                 */
                const int32_t tabIndex = m_colorBar->getTabIndex();
                EventGetViewportSize tabSizeEvent(EventGetViewportSize::MODE_TAB_AFTER_MARGINS_INDEX,
                                                  tabIndex);
                EventManager::get()->sendEvent(tabSizeEvent.getPointer());
                if (tabSizeEvent.getEventProcessCount() > 0) {
                    EventGetViewportSize windowSizeEvent(EventGetViewportSize::MODE_WINDOW_FROM_TAB_INDEX,
                                                         tabIndex);
                    EventManager::get()->sendEvent(windowSizeEvent.getPointer());
                    if (windowSizeEvent.getEventProcessCount() > 0) {
                        int32_t tabViewport[4];
                        tabSizeEvent.getViewportSize(tabViewport);
                        int32_t windowViewport[4];
                        windowSizeEvent.getViewportSize(windowViewport);
                        
                        const bool matchPositionFlag(true);
                        const bool matchSizeFlag(true);
                        if (tabToWindowFlag) {
                            m_colorBar->matchPixelPositionAndSizeInNewViewport(tabViewport,
                                                                               windowViewport,
                                                                               matchPositionFlag,
                                                                               matchSizeFlag);
                        }
                        else if (windowToTabFlag) {
                            m_colorBar->matchPixelPositionAndSizeInNewViewport(windowViewport,
                                                                               tabViewport,
                                                                               matchPositionFlag,
                                                                               matchSizeFlag);
                        }
                        else {
                            CaretAssert(0);
                        }
                    }
                }
            }
        }

        m_colorBar->setCoordinateSpace(newCoordinateSpace);
        EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
        EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    }
}

QWidget*
MapSettingsColorBarWidget::createLocationPositionSection()
{
    QLabel* positionModeLabel = new QLabel("Positioning");
    m_annotationColorBarPositionModeEnumComboBox = new EnumComboBoxTemplate(this);
    m_annotationColorBarPositionModeEnumComboBox->setup<AnnotationColorBarPositionModeEnum,AnnotationColorBarPositionModeEnum::Enum>();
    QObject::connect(m_annotationColorBarPositionModeEnumComboBox, SIGNAL(itemActivated()),
                     this, SLOT(annotationColorBarPositionModeEnumComboBoxItemActivated()));
    m_annotationColorBarPositionModeEnumComboBox->getWidget()->setToolTip("AUTOMATIC - color bars are stacked\n"
                                                                          "   in lower left corner of Tab/Window\n"
                                                                          "MANUAL - user must set the X and Y\n"
                                                                          "   coordinates in the Tab/Window\n");
    
    std::vector<AnnotationCoordinateSpaceEnum::Enum> supportedSpaces;
    supportedSpaces.push_back(AnnotationCoordinateSpaceEnum::TAB);
    supportedSpaces.push_back(AnnotationCoordinateSpaceEnum::WINDOW);
    
    QLabel* coordinateSpaceLabel = new QLabel("Show Color Bar in ");
    m_annotationCoordinateSpaceEnumComboBox = new EnumComboBoxTemplate(this);
    m_annotationCoordinateSpaceEnumComboBox->setupWithItems<AnnotationCoordinateSpaceEnum,AnnotationCoordinateSpaceEnum::Enum>(supportedSpaces);
    QObject::connect(m_annotationCoordinateSpaceEnumComboBox, SIGNAL(itemActivated()),
                     this, SLOT(annotationCoordinateSpaceEnumComboBoxItemActivated()));
    m_annotationCoordinateSpaceEnumComboBox->getWidget()->setToolTip("TAB - Restricts location of colorbar\n"
                                                                     "   to within its respective tab's\n"
                                                                     "   region in the graphics area\n"
                                                                     "WINDOW - Allows colorbar in any\n"
                                                                     "   location in the graphics area");
        
    QWidget* modeSpaceWidget = new QWidget();
    QGridLayout* modeSpaceLayout = new QGridLayout(modeSpaceWidget);
    modeSpaceLayout->setContentsMargins(0, 0, 0, 0);
    modeSpaceLayout->addWidget(coordinateSpaceLabel, 0, 0);
    modeSpaceLayout->addWidget(m_annotationCoordinateSpaceEnumComboBox->getWidget(), 0, 1);
    modeSpaceLayout->addWidget(positionModeLabel, 1, 0);
    modeSpaceLayout->addWidget(m_annotationColorBarPositionModeEnumComboBox->getWidget(), 1, 1);
    
    QGroupBox* locationAndDimGroupBox = new QGroupBox("Location and Positioning");
    QVBoxLayout* locationAndDimLayout = new QVBoxLayout(locationAndDimGroupBox);
    locationAndDimLayout->setContentsMargins(0, 0, 0, 0);
    locationAndDimLayout->setSpacing(2);
    locationAndDimLayout->addWidget(modeSpaceWidget, 0, Qt::AlignLeft);
    
    locationAndDimGroupBox->setSizePolicy(QSizePolicy::Fixed,
                                          QSizePolicy::Fixed);
    
    return locationAndDimGroupBox;
}

/**
 * @return Create the color bar section.
 */
QWidget*
MapSettingsColorBarWidget::createDataNumericsSection()
{
    QLabel* valuesModeLabel = new QLabel("Data Mode");
    m_colorBarDataModeComboBox = new EnumComboBoxTemplate(this);
    QObject::connect(m_colorBarDataModeComboBox, SIGNAL(itemActivated()),
                     this, SLOT(colorBarItemActivated()));
    m_colorBarDataModeComboBox->setup<PaletteColorBarValuesModeEnum, PaletteColorBarValuesModeEnum::Enum>();
    m_colorBarDataModeComboBox->getWidget()->setToolTip("Format of numbers above color bar\n"
                                                        "   DATA: Numbers show mapping of data to the colobar\n"
                                                        "   PERCENTILE: Numbers show data percentiles\n"
                                                        "   SIGN ONLY: Text above colorbar indicates sign of data");
    
    m_colorBarNumericFormatModeLabel = new QLabel("Numeric Format");
    m_colorBarNumericFormatModeComboBox = new EnumComboBoxTemplate(this);
    QObject::connect(m_colorBarNumericFormatModeComboBox, SIGNAL(itemActivated()),
                     this, SLOT(colorBarItemActivated()));
    m_colorBarNumericFormatModeComboBox->setup<NumericFormatModeEnum, NumericFormatModeEnum::Enum>();
    m_colorBarNumericFormatModeComboBox->getWidget()->setToolTip("Format of numbers above color bar\n"
                                                                 "   AUTO: Format (decimal or scientific notation)\n"
                                                                 "         and precision digits are automatically\n"
                                                                 "         generated by examination of the data\n"
                                                                 "   DECIMAL: Format as decimal number with the\n"
                                                                 "         selected precision\n"
                                                                 "   SCIENTIFIC: Format as scientific notation\n"
                                                                 "         with the selected precision in the\n"
                                                                 "         significand");
    
    m_colorBarDecimalsLabel = new QLabel("Dec/Sci Decimals"); //Precision Digits");
    m_colorBarDecimalsSpinBox = new QSpinBox();
    m_colorBarDecimalsSpinBox->setMinimum(0);
    m_colorBarDecimalsSpinBox->setMaximum(100);
    m_colorBarDecimalsSpinBox->setSingleStep(1);
    QObject::connect(m_colorBarDecimalsSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(applySelections()));
    m_colorBarDecimalsSpinBox->setToolTip("Number of digits right of the decimal point\n"
                                          "in the numbers above the color bar when format\n"
                                          "is " + NumericFormatModeEnum::toGuiName(NumericFormatModeEnum::DECIMAL)
                                          + " or " + NumericFormatModeEnum::toGuiName(NumericFormatModeEnum::SCIENTIFIC));
    
    m_colorBarNumericSubdivisionsLabel = new QLabel("Subdivisions");
    m_colorBarNumericSubdivisionsSpinBox = new QSpinBox();
    m_colorBarNumericSubdivisionsSpinBox->setMinimum(0);
    m_colorBarNumericSubdivisionsSpinBox->setMaximum(100);
    m_colorBarNumericSubdivisionsSpinBox->setSingleStep(1);
    QObject::connect(m_colorBarNumericSubdivisionsSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(applySelections()));
    m_colorBarNumericSubdivisionsSpinBox->setToolTip("Adds additional numbers to the negative\n"
                                                     "and positive regions in the color bar");
    
    m_showTickMarksCheckBox = new QCheckBox("Show Tick Marks");
    QObject::connect(m_showTickMarksCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(applySelections()));
    
    QGroupBox* colorBarGroupBox = new QGroupBox("Data Numerics");
    QGridLayout* gridLayout = new QGridLayout(colorBarGroupBox);
    int row = gridLayout->rowCount();
    //    this->setLayoutSpacingAndMargins(gridLayout);
    gridLayout->addWidget(valuesModeLabel, row, 0);
    gridLayout->addWidget(m_colorBarDataModeComboBox->getWidget(), row, 1);
    row++;
    gridLayout->addWidget(m_colorBarNumericFormatModeLabel, row, 0);
    gridLayout->addWidget(m_colorBarNumericFormatModeComboBox->getWidget(), row, 1);
    row++;
    gridLayout->addWidget(m_colorBarDecimalsLabel, row, 0);
    gridLayout->addWidget(m_colorBarDecimalsSpinBox, row, 1);
    row++;
    gridLayout->addWidget(m_colorBarNumericSubdivisionsLabel, row, 0);
    gridLayout->addWidget(m_colorBarNumericSubdivisionsSpinBox, row, 1);
    row++;
    gridLayout->addWidget(m_showTickMarksCheckBox, row, 0, 1, 2, Qt::AlignLeft);
    
    colorBarGroupBox->setSizePolicy(QSizePolicy::Fixed,
                                    QSizePolicy::Fixed);
    
    return colorBarGroupBox;
}

/**
 * Gets called when a color bar item is selected.
 */
void
MapSettingsColorBarWidget::colorBarItemActivated()
{
    applySelections();
    
    updateColorBarAttributes();
}

/**
 * Update the colorbar attributes section.
 */
void
MapSettingsColorBarWidget::updateColorBarAttributes()
{
    if (m_paletteColorMapping != NULL) {
        bool numericForatComboBoxEnabled   = true;
        bool precisionDigitsSpinBoxEnabled = true;
        bool subdivisionsSpinBoxEnabled    = true;
        
        const PaletteColorBarValuesModeEnum::Enum valuesMode = m_paletteColorMapping->getColorBarValuesMode();
        switch (valuesMode) {
            case PaletteColorBarValuesModeEnum::DATA:
                break;
            case PaletteColorBarValuesModeEnum::PERCENTILE:
                break;
            case PaletteColorBarValuesModeEnum::SIGN_ONLY:
                numericForatComboBoxEnabled   = false;
                precisionDigitsSpinBoxEnabled = false;
                subdivisionsSpinBoxEnabled    = false;
                break;
        }
        
        const NumericFormatModeEnum::Enum numericFormat = m_paletteColorMapping->getColorBarNumericFormatMode();
        switch (numericFormat) {
            case NumericFormatModeEnum::AUTO:
                precisionDigitsSpinBoxEnabled = false;
                break;
            case NumericFormatModeEnum::DECIMAL:
                break;
            case NumericFormatModeEnum::SCIENTIFIC:
                break;
        }
        
        m_colorBarDataModeComboBox->setSelectedItem<PaletteColorBarValuesModeEnum, PaletteColorBarValuesModeEnum::Enum>(valuesMode);
        
        m_colorBarNumericFormatModeLabel->setEnabled(numericForatComboBoxEnabled);
        m_colorBarNumericFormatModeComboBox->setSelectedItem<NumericFormatModeEnum, NumericFormatModeEnum::Enum>(numericFormat);
        m_colorBarNumericFormatModeComboBox->getWidget()->setEnabled(numericForatComboBoxEnabled);
        
        m_colorBarDecimalsLabel->setEnabled(precisionDigitsSpinBoxEnabled);
        m_colorBarDecimalsSpinBox->blockSignals(true);
        m_colorBarDecimalsSpinBox->setValue(m_paletteColorMapping->getColorBarPrecisionDigits());
        m_colorBarDecimalsSpinBox->blockSignals(false);
        m_colorBarDecimalsSpinBox->setEnabled(precisionDigitsSpinBoxEnabled);
        
        m_colorBarNumericSubdivisionsLabel->setEnabled(subdivisionsSpinBoxEnabled);
        m_colorBarNumericSubdivisionsSpinBox->blockSignals(true);
        m_colorBarNumericSubdivisionsSpinBox->setValue(m_paletteColorMapping->getColorBarNumericSubdivisionCount());
        m_colorBarNumericSubdivisionsSpinBox->blockSignals(false);
        m_colorBarNumericSubdivisionsSpinBox->setEnabled(subdivisionsSpinBoxEnabled);
        
        m_showTickMarksCheckBox->setChecked(m_paletteColorMapping->isColorBarShowTickMarksSelected());
    }
}


