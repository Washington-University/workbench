
/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

#define __SCALE_BAR_WIDGET_DECLARE__
#include "ScaleBarWidget.h"
#undef __SCALE_BAR_WIDGET_DECLARE__

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QSpinBox>
#include <QVBoxLayout>

#include "AnnotationScaleBar.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretColorEnumComboBox.h"
#include "EnumComboBoxTemplate.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "WuQDoubleSpinBox.h"
#include "WuQMacroManager.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::ScaleBarWidget 
 * \brief Widget for editing a scale bar
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
ScaleBarWidget::ScaleBarWidget(const QString& objectNamePrefix)
: QWidget()
{
    m_objectNamePrefix = (objectNamePrefix
                          + ":ScaleBarWidget");
    setObjectName(m_objectNamePrefix);

    QWidget* barWidget        = createBarWidget();
    QWidget* lengthTextWidget = createLengthTextWidget();
    QWidget* positionWidget   = createPositionWidget();
    QWidget* ticksWidget      = createTickMarksWidget();
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(barWidget);
    layout->addWidget(lengthTextWidget);
    layout->addWidget(ticksWidget);
    layout->addWidget(positionWidget);
}

/**
 * Destructor.
 */
ScaleBarWidget::~ScaleBarWidget()
{
}


/**
 * @return Instance of the length widget
 */
QWidget*
ScaleBarWidget::createBarWidget()
{
    WuQMacroManager* macroManager = WuQMacroManager::instance();

    QLabel* lengthLabel = new QLabel("Length");
    m_lengthSpinBox = new QDoubleSpinBox();
    m_lengthSpinBox->setToolTip("Set the length of the scale bar in the selected units");
    m_lengthSpinBox->setRange(0.0, 100000.0);
    QObject::connect(m_lengthSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this, &ScaleBarWidget::lengthSpinBoxValueChanged);
    m_lengthSpinBox->setObjectName(m_objectNamePrefix
                                   + ":m_lengthSpinBox");
    macroManager->addMacroSupportToObject(m_lengthSpinBox,
                                          "Set Length");
    
    QLabel* thicknessLabel = new QLabel("Thickness");
    m_thicknessSpinBox = new QDoubleSpinBox();
    m_thicknessSpinBox->setToolTip("Set the thickness of the scale bar as percentage of viewport height");
    m_thicknessSpinBox->setRange(0.1, 100.0);
    m_thicknessSpinBox->setSingleStep(0.1);
    m_thicknessSpinBox->setDecimals(1);
    m_thicknessSpinBox->setSuffix("%");
    QObject::connect(m_thicknessSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this, &ScaleBarWidget::thicknessSpinBoxValueChanged);
    m_thicknessSpinBox->setObjectName(m_objectNamePrefix
                                      + ":m_thicknessSpinBox");
    macroManager->addMacroSupportToObject(m_thicknessSpinBox,
                                          "Set Length");
    
    
    QLabel* unitsLabel = new QLabel("Length Units");
    m_lengthUnitsComboBox = new EnumComboBoxTemplate(this);
    m_lengthUnitsComboBox->getComboBox()->setToolTip("Select the units for the length of the scale bar");
    m_lengthUnitsComboBox->setup<AnnotationScaleBarUnitsTypeEnum,AnnotationScaleBarUnitsTypeEnum::Enum>();
    QObject::connect(m_lengthUnitsComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &ScaleBarWidget::lengthEnumComboBoxItemActivated);
    m_lengthUnitsComboBox->getWidget()->setObjectName(m_objectNamePrefix
                                                      + ":m_lengthUnitsComboBox");
    macroManager->addMacroSupportToObject(m_lengthUnitsComboBox->getComboBox(),
                                          "Select Length Units");
    
    QLabel* foregroundLabel = new QLabel("Bar Color");
    m_foregroundColorComboBox = new CaretColorEnumComboBox(CaretColorEnumComboBox::CustomColorModeEnum::EDITABLE,
                                                           CaretColorEnumComboBox::NoneColorModeEnum::DISABLED,
                                                           this);
    m_foregroundColorComboBox->setToolTip("Set the color of the scale bar");
    QObject::connect(m_foregroundColorComboBox, &CaretColorEnumComboBox::colorSelected,
                     this, &ScaleBarWidget::foregroundColorComboBoxSelected);
    
    QLabel* backgroundLabel = new QLabel("Background");
    m_backgroundColorComboBox = new CaretColorEnumComboBox(CaretColorEnumComboBox::CustomColorModeEnum::EDITABLE,
                                                           CaretColorEnumComboBox::NoneColorModeEnum::ENABLED,
                                                           this);
    m_backgroundColorComboBox->setToolTip("Set the background of the scale bar to a color or NONE (no background)");
    QObject::connect(m_backgroundColorComboBox, &CaretColorEnumComboBox::colorSelected,
                     this, &ScaleBarWidget::backgroundColorComboBoxSelected);
    
    QGroupBox* groupBox = new QGroupBox("Scale Bar");
    QGridLayout* gridLayout = new QGridLayout(groupBox);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 4, 2);
    int32_t row(0);
    gridLayout->addWidget(lengthLabel, row, 0);
    gridLayout->addWidget(m_lengthSpinBox, row, 1);
    row++;
    gridLayout->addWidget(unitsLabel, row, 0);
    gridLayout->addWidget(m_lengthUnitsComboBox->getWidget(), row, 1);
    row++;
    gridLayout->addWidget(thicknessLabel, row, 0);
    gridLayout->addWidget(m_thicknessSpinBox, row, 1);
    row++;
    gridLayout->addWidget(foregroundLabel, row, 0);
    gridLayout->addWidget(m_foregroundColorComboBox->getWidget(), row, 1);
    row++;
    gridLayout->addWidget(backgroundLabel, row, 0);
    gridLayout->addWidget(m_backgroundColorComboBox->getWidget(), row, 1);
    row++;

    return groupBox;
}

/**
 * @return Instance of the length widget
 */
QWidget*
ScaleBarWidget::createLengthTextWidget()
{
    WuQMacroManager* macroManager = WuQMacroManager::instance();

    m_showLengthTextCheckBox = new QCheckBox("Show Length");
    m_showLengthTextCheckBox->setToolTip("Show the length of the scale bar");
    QObject::connect(m_showLengthTextCheckBox, &QCheckBox::clicked,
                     this, &ScaleBarWidget::showLengthCheckBoxClicked);
    m_showLengthTextCheckBox->setObjectName(m_objectNamePrefix
                                            + ":m_showLengthTextCheckBox");
    macroManager->addMacroSupportToObject(m_showLengthTextCheckBox,
                                          "Show Length");
    
    m_showLengthUnitsCheckbox = new QCheckBox("Show Length Units");
    m_showLengthUnitsCheckbox->setToolTip("Show the length units after the length");
    QObject::connect(m_showLengthUnitsCheckbox, &QCheckBox::clicked,
                     this, &ScaleBarWidget::showLengthUnitsCheckBoxClicked);
    m_showLengthUnitsCheckbox->setObjectName(m_objectNamePrefix
                                             + ":m_showLengthUnitsCheckbox");
    macroManager->addMacroSupportToObject(m_showLengthUnitsCheckbox,
                                          "Show Length Units");
    
    QLabel* locationLabel = new QLabel("Location");
    m_lengthTextLocationComboBox = new EnumComboBoxTemplate(this);
    m_lengthTextLocationComboBox->getComboBox()->setToolTip("Set location of length text in scale bar");
    m_lengthTextLocationComboBox->setup<AnnotationScaleBarTextLocationEnum, AnnotationScaleBarTextLocationEnum::Enum>();
    QObject::connect(m_lengthTextLocationComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &ScaleBarWidget::lengthTextLocationComboBoxActivated);
    m_lengthTextLocationComboBox->getComboBox()->setObjectName(m_objectNamePrefix
                                                               + ":m_lengthTextLocationComboBox");
    macroManager->addMacroSupportToObject(m_lengthTextLocationComboBox->getComboBox(),
                                          "Set Location of Text");
    
    QLabel* fontColorLabel = new QLabel("Text Color");
    m_fontColorComboBox = new CaretColorEnumComboBox(CaretColorEnumComboBox::CustomColorModeEnum::EDITABLE,
                                                     CaretColorEnumComboBox::NoneColorModeEnum::DISABLED,
                                                     this);
    m_fontColorComboBox->setToolTip("Set the color of the length text");
    QObject::connect(m_fontColorComboBox, &CaretColorEnumComboBox::colorSelected,
                     this, &ScaleBarWidget::fontColorComboBoxSelected);
    
    
    /*
     * Combo box for font name selection
     */
    QLabel* fontLabel = new QLabel("Font ");
    m_fontNameComboBox = new EnumComboBoxTemplate(this);
    m_fontNameComboBox->getWidget()->setToolTip("Choose font for length text");
    m_fontNameComboBox->setup<AnnotationTextFontNameEnum,AnnotationTextFontNameEnum::Enum>();
    QObject::connect(m_fontNameComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &ScaleBarWidget::fontNameChanged);
    
    QLabel* fontSizeLabel = new QLabel("Font Size");
    const AString fontSizeToolTop("<html>"
                                  "Adjusts font height (size), as a percentage of the viewport height, that is  "
                                  "converted to a pixel height when the text is drawn.  "
                                  "<p>"
                                  "The numeric value in this control will be <font color=\"red\">RED</font> "
                                  "when the pixel height is estimated to be <i>too small</i> and some or all "
                                  "characters may not be drawn.  "
                                  "Reducing the height of the text and/or the height of the window may cause "
                                  "<i>too small</i> text.  "
                                  "</html>");
    m_fontSizeSpinBox = new WuQDoubleSpinBox(this);
    m_fontSizeSpinBox->setRangePercentage(0.0, 100.0);
    WuQtUtilities::setToolTipAndStatusTip(m_fontSizeSpinBox->getWidget(),
                                          fontSizeToolTop);
    QObject::connect(m_fontSizeSpinBox, &WuQDoubleSpinBox::valueChanged,
                     this, &ScaleBarWidget::fontSizeValueChanged);
    
    /*
     * Default palette for size spin box
     */
    m_fontSizeSpinBoxDefaultPalette = m_fontSizeSpinBox->getWidget()->palette();
    
    /*
     * Palette for spin box that colors text in red when the font height is "too small"
     */
    m_fontSizeSpinBoxRedTextPalette = m_fontSizeSpinBoxDefaultPalette;
    QBrush brush = m_fontSizeSpinBoxRedTextPalette.brush(QPalette::Active, QPalette::Text);
    brush.setColor(Qt::red);
    m_fontSizeSpinBoxRedTextPalette.setBrush(QPalette::Active, QPalette::Text, brush);
    m_fontSizeSpinBoxRedTextPalette.setBrush(QPalette::Active, QPalette::WindowText, brush);
    m_fontSizeSpinBoxRedTextPalette.setBrush(QPalette::Active, QPalette::HighlightedText, brush);
    
    QGroupBox* groupBox = new QGroupBox("Length Text");
    QGridLayout* gridLayout = new QGridLayout(groupBox);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 4, 2);
    int32_t row(0);
    gridLayout->addWidget(m_showLengthTextCheckBox, row, 0, 1, 2, Qt::AlignLeft);
    row++;
    gridLayout->addWidget(m_showLengthUnitsCheckbox, row, 0, 1, 2, Qt::AlignLeft);
    row++;
    gridLayout->addWidget(fontColorLabel, row, 0);
    gridLayout->addWidget(m_fontColorComboBox->getWidget(), row, 1);
    row++;
    gridLayout->addWidget(fontLabel, row, 0);
    gridLayout->addWidget(m_fontNameComboBox->getWidget(), row, 1);
    row++;
    gridLayout->addWidget(fontSizeLabel, row, 0);
    gridLayout->addWidget(m_fontSizeSpinBox->getWidget(), row, 1);
    row++;
    gridLayout->addWidget(locationLabel, row, 0);
    gridLayout->addWidget(m_lengthTextLocationComboBox->getWidget(), row, 1);
    row++;

    return groupBox;
}

/**
 * @return Instance of the misc widget
 */
QWidget*
ScaleBarWidget::createPositionWidget()
{
    WuQMacroManager* macroManager = WuQMacroManager::instance();
    
    QLabel* positionModeLabel = new QLabel("Mode");
    m_positionModeEnumComboBox = new EnumComboBoxTemplate(this);
    m_positionModeEnumComboBox->setup<AnnotationColorBarPositionModeEnum,AnnotationColorBarPositionModeEnum::Enum>();
    QObject::connect(m_positionModeEnumComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &ScaleBarWidget::positionModeEnumComboBoxItemActivated);
    m_positionModeEnumComboBox->getWidget()->setToolTip("AUTOMATIC - color bars are stacked\n"
                                                        "   in lower left corner of Tab/Window\n"
                                                        "MANUAL - user must set the X and Y\n"
                                                        "   coordinates in Annotate mode by\n"
                                                        "   selecting and dragging the scale bar\n");
    m_positionModeEnumComboBox->getComboBox()->setObjectName(m_objectNamePrefix
                                            + ":m_positionModeEnumComboBox");
    macroManager->addMacroSupportToObject(m_positionModeEnumComboBox->getComboBox(),
                                          "Positionm Mode");

    QGroupBox* groupBox = new QGroupBox("Position");
    QGridLayout* gridLayout = new QGridLayout(groupBox);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 4, 2);
    int32_t row(0);
    gridLayout->addWidget(positionModeLabel, row, 0);
    gridLayout->addWidget(m_positionModeEnumComboBox->getWidget(), row, 1);
    row++;
    
    return groupBox;
}

/**
 * @return Instance of the tick marks widget
 */
QWidget*
ScaleBarWidget::createTickMarksWidget()
{
    WuQMacroManager* macroManager = WuQMacroManager::instance();

    m_showTickMarksCheckBox = new QCheckBox("Show Tick Marks");
    m_showTickMarksCheckBox->setToolTip("Show tick marks on the scale bar");
    QObject::connect(m_showTickMarksCheckBox, &QCheckBox::clicked,
                     this, &ScaleBarWidget::showTickMarksCheckBoxClicked);
    m_showTickMarksCheckBox->setObjectName(m_objectNamePrefix
                                            + ":m_showTickMarksCheckBox");
    macroManager->addMacroSupportToObject(m_showTickMarksCheckBox,
                                          "Show Tick Marks");

    QLabel* subdivisionsLabel = new QLabel("Subdivisions");
    m_tickMarksSubdivisionsSpinBox = new QSpinBox();
    m_tickMarksSubdivisionsSpinBox->setToolTip("Set the number of subdivisions (space between tick marks)");
    m_tickMarksSubdivisionsSpinBox->setRange(0, 10);
    QObject::connect(m_tickMarksSubdivisionsSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                     this, &ScaleBarWidget::tickMarksSubdivsionsSpinBoxValueChanged);
    m_tickMarksSubdivisionsSpinBox->setObjectName(m_objectNamePrefix
                                            + ":m_tickMarksSubdivisionsSpinBox");
    macroManager->addMacroSupportToObject(m_tickMarksSubdivisionsSpinBox,
                                          "Set Number of Subdivisions");

    
    QGroupBox* groupBox = new QGroupBox("Tick Marks");
    QGridLayout* gridLayout = new QGridLayout(groupBox);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 4, 2);
    int32_t row(0);
    gridLayout->addWidget(m_showTickMarksCheckBox, row, 0, 1, 2, Qt::AlignLeft);
    row++;
    gridLayout->addWidget(subdivisionsLabel, row, 0);
    gridLayout->addWidget(m_tickMarksSubdivisionsSpinBox, row, 1);
    row++;
    
    return groupBox;
}

/**
 * Update the widgets content
 *  @param browserTabContent
 *       The tab's content
 */
void
ScaleBarWidget::updateContent(BrowserTabContent* browserTabContent)
{
    m_scaleBar = NULL;
    if (browserTabContent != NULL) {
        m_scaleBar = browserTabContent->getScaleBar();
    }
    
    if (m_scaleBar != NULL) {
        const AnnotationColorBarPositionModeEnum::Enum positionMode = m_scaleBar->getPositionMode();
        m_positionModeEnumComboBox->setSelectedItem<AnnotationColorBarPositionModeEnum,AnnotationColorBarPositionModeEnum::Enum>(positionMode);
    
        const AnnotationScaleBarUnitsTypeEnum::Enum unitsType = m_scaleBar->getLengthUnits();
        m_lengthUnitsComboBox->setSelectedItem<AnnotationScaleBarUnitsTypeEnum,AnnotationScaleBarUnitsTypeEnum::Enum>(unitsType);
        
        m_showLengthTextCheckBox->setChecked(m_scaleBar->isShowLengthText());
        
        m_showLengthUnitsCheckbox->setChecked(m_scaleBar->isShowLengthUnitsText());
        
        QSignalBlocker lengthBlocker(m_lengthSpinBox);
        m_lengthSpinBox->setValue(m_scaleBar->getLength());
        
        QSignalBlocker thicknessBlocker(m_thicknessSpinBox);
        m_thicknessSpinBox->setValue(m_scaleBar->getLineWidthPercentage());
        
        m_showTickMarksCheckBox->setChecked(m_scaleBar->isShowTickMarks());
        QSignalBlocker ticksSignalBlocker(m_tickMarksSubdivisionsSpinBox);
        m_tickMarksSubdivisionsSpinBox->setValue(m_scaleBar->getTickMarksSubdivsions());
        
        const AnnotationScaleBarTextLocationEnum::Enum textLocation = m_scaleBar->getLengthTextLocation();
        m_lengthTextLocationComboBox->setSelectedItem<AnnotationScaleBarTextLocationEnum, AnnotationScaleBarTextLocationEnum::Enum>(textLocation);
        
        const CaretColorEnum::Enum foregroundColor = m_scaleBar->getLineColor();
        m_foregroundColorComboBox->setSelectedColor(foregroundColor);
        std::array<uint8_t, 4> customRGBA;
        m_scaleBar->getCustomLineColor(&customRGBA[0]);
        m_foregroundColorComboBox->setCustomColor(customRGBA);
        
        const CaretColorEnum::Enum backgroundColor = m_scaleBar->getBackgroundColor();
        m_backgroundColorComboBox->setSelectedColor(backgroundColor);
        m_scaleBar->getCustomBackgroundColor(&customRGBA[0]);
        m_backgroundColorComboBox->setCustomColor(customRGBA);
        
        const CaretColorEnum::Enum fontColor = m_scaleBar->getTextColor();
        m_fontColorComboBox->setSelectedColor(fontColor);
        m_scaleBar->getCustomTextColor(&customRGBA[0]);
        m_fontColorComboBox->setCustomColor(customRGBA);
                
        m_fontNameComboBox->setSelectedItem<AnnotationTextFontNameEnum,AnnotationTextFontNameEnum::Enum>(m_scaleBar->getFont());
        
        QSignalBlocker fontSizeBlocker(m_fontSizeSpinBox);
        m_fontSizeSpinBox->setValue(m_scaleBar->getFontPercentViewportSize());
    }

    setEnabled(m_scaleBar != NULL);
}

/**
 * Called when show length check box is clicked
 *
 *  @param status
 *    New checked status
 */
void
ScaleBarWidget::showTickMarksCheckBoxClicked(bool status)
{
    if (m_scaleBar != NULL) {
        m_scaleBar->setShowTickMarks(status);
        updateGraphics();
    }
}

/**
 * Called when tick marks subdivisions spin box value is changed
 *
 *  @param value
 *    New value
 */
void
ScaleBarWidget::tickMarksSubdivsionsSpinBoxValueChanged(int value)
{
    if (m_scaleBar != NULL) {
        m_scaleBar->setTickMarksSubdivisions(value);
        updateGraphics();
    }
}

/**
 * Called length text location value is changed
 *
 */
void
ScaleBarWidget::lengthTextLocationComboBoxActivated()
{
    if (m_scaleBar != NULL) {
        const AnnotationScaleBarTextLocationEnum::Enum location = m_lengthTextLocationComboBox->getSelectedItem<AnnotationScaleBarTextLocationEnum, AnnotationScaleBarTextLocationEnum::Enum>();
        m_scaleBar->setLengthTextLocation(location);
        updateGraphics();
    }
}

/**
 * Called when show length check box is clicked
 *
 *  @param status
 *    New checked status
 */
void
ScaleBarWidget::showLengthCheckBoxClicked(bool status)
{
    if (m_scaleBar != NULL) {
        m_scaleBar->setShowLengthText(status);
        updateGraphics();
    }
}

/**
 * Called when show length units check box is clicked
 *
 *  @param status
 *    New checked status
 */
void
ScaleBarWidget::showLengthUnitsCheckBoxClicked(bool status)
{
    if (m_scaleBar != NULL) {
        m_scaleBar->setShowLengthUnitsText(status);
        updateGraphics();
    }
}

/**
 * Called when  length double spin box value is changed
 *
 *  @param value
 *    New value
 */
void
ScaleBarWidget::lengthSpinBoxValueChanged(double value)
{
    if (m_scaleBar != NULL) {
        m_scaleBar->setLength(value);
        updateGraphics();
    }
}

/**
 * Called when  thickness double spin box value is changed
 *
 *  @param value
 *    New value
 */
void
ScaleBarWidget::thicknessSpinBoxValueChanged(double value)
{
    if (m_scaleBar != NULL) {
        m_scaleBar->setLineWidthPercentage(value);
        updateGraphics();
    }
}

/**
 * Caleld when position mode combo box selection is made
 */
void
ScaleBarWidget::positionModeEnumComboBoxItemActivated()
{
    if (m_scaleBar != NULL) {
        m_scaleBar->setPositionMode(m_positionModeEnumComboBox->getSelectedItem<AnnotationColorBarPositionModeEnum, AnnotationColorBarPositionModeEnum::Enum>());
        updateGraphics();
    }
}

/**
 * Caleld when length units combo box selection is made
 */
void
ScaleBarWidget::lengthEnumComboBoxItemActivated()
{
    if (m_scaleBar != NULL) {
        m_scaleBar->setLengthUnits(m_lengthUnitsComboBox->getSelectedItem<AnnotationScaleBarUnitsTypeEnum, AnnotationScaleBarUnitsTypeEnum::Enum>());
        updateGraphics();
    }
}

/**
 * Called when background color is selected
 * @param color
 *   Color selected
 */
void
ScaleBarWidget::backgroundColorComboBoxSelected(const CaretColorEnum::Enum color)
{
    if (m_scaleBar != NULL) {
        m_scaleBar->setBackgroundColor(color);
        if (color == CaretColorEnum::CUSTOM) {
            std::array<uint8_t, 4> rgba;
            m_backgroundColorComboBox->getCustomColor(rgba);
            m_scaleBar->setCustomBackgroundColor(&rgba[0]);
        }
        updateGraphics();
    }
}

/**
 * Called when foreground color is selected
 * @param color
 *   Color selected
 */
void
ScaleBarWidget::foregroundColorComboBoxSelected(const CaretColorEnum::Enum color)
{
    if (m_scaleBar != NULL) {
        m_scaleBar->setLineColor(color);
        if (color == CaretColorEnum::CUSTOM) {
            std::array<uint8_t, 4> rgba;
            m_foregroundColorComboBox->getCustomColor(rgba);
            m_scaleBar->setCustomLineColor(&rgba[0]);
        }
        updateGraphics();
    }
}

/**
 * Called when font color is selected
 * @param color
 *   Color selected
 */
void
ScaleBarWidget::fontColorComboBoxSelected(const CaretColorEnum::Enum color)
{
    if (m_scaleBar != NULL) {
        m_scaleBar->setTextColor(color);
        if (color == CaretColorEnum::CUSTOM) {
            std::array<uint8_t, 4> rgba;
            m_fontColorComboBox->getCustomColor(rgba);
            m_scaleBar->setCustomTextColor(&rgba[0]);
        }
        updateGraphics();
    }
}

/**
 * Gets called when font name changed.
 */
void
ScaleBarWidget::fontNameChanged()
{
    if (m_scaleBar != NULL) {
        const AnnotationTextFontNameEnum::Enum fontName = m_fontNameComboBox->getSelectedItem<AnnotationTextFontNameEnum,AnnotationTextFontNameEnum::Enum>();
        m_scaleBar->setFont(fontName);
        updateGraphics();
    }
}

/**
 * Called when font size spin box value is changed
 *
 *  @param value
 *    New value
 */
void
ScaleBarWidget::fontSizeValueChanged(double value)
{
    if (m_scaleBar != NULL) {
        m_scaleBar->setFontPercentViewportSize(value);
        
        if (m_scaleBar->isFontTooSmallWhenLastDrawn()) {
            m_fontSizeSpinBox->getWidget()->setPalette(m_fontSizeSpinBoxRedTextPalette);
        }
        else {
            m_fontSizeSpinBox->getWidget()->setPalette(m_fontSizeSpinBoxDefaultPalette);
        }

        updateGraphics();
    }
}

/**
 * Update graphics
 */
void
ScaleBarWidget::updateGraphics()
{
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}
