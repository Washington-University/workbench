
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

#include <QBoxLayout>
#include <QColorDialog>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSignalMapper>
#include <QTabWidget>

#define __PREFERENCES_DIALOG__H__DECLARE__
#include "PreferencesDialog.h"
#undef __PREFERENCES_DIALOG__H__DECLARE__

#include "Brain.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretPreferences.h"
#include "PreferencesDevelopOptionsWidget.h"
#include "EnumComboBoxTemplate.h"
#include "EventGraphicsPaintNowAllWindows.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventManager.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "IdentificationManager.h"
#include "ImageCaptureMethodEnum.h"
#include "OpenGLDrawingMethodEnum.h"
#include "PreferencesDevelopOptionsWidget.h"
#include "PreferencesDisplayWidget.h"
#include "PreferencesImageWidget.h"
#include "PreferencesRecentFilesWidget.h"
#include "SessionManager.h"
#include "WuQtUtilities.h"
#include "WuQFactory.h"
#include "WuQMessageBox.h"
#include "WuQTrueFalseComboBox.h"
#include "WuQWidgetObjectGroup.h"

using namespace caret;


    
/**
 * \class caret::PreferencesDialog 
 * \brief Dialog for display/editing of prefernces.
 * \ingroup GuiQt
 *
 * Presents controls for editing palettes used to color
 * scalar data.
 */

/**
 * Constructor for editing a palette selection.
 *
 * @param parent
 *    Parent widget on which this dialog is displayed.
 */
PreferencesDialog::PreferencesDialog(QWidget* parent)
: WuQDialogNonModal("Preferences",
                    parent)
{
    setDeleteWhenClosed(false);

    /*
     * No apply button
     */
    setApplyButtonText("");    
    
    /*
     * Used to block signals in all widgets
     */
    m_allWidgets = new WuQWidgetObjectGroup(this);
    
    m_recentFilesWidget = new PreferencesRecentFilesWidget();
    QObject::connect(m_recentFilesWidget, &PreferencesRecentFilesWidget::updateDialog,
                     this, &PreferencesDialog::recentFilesChanged);
    
    m_developOptionsWidget = new PreferencesDevelopOptionsWidget();
    
    m_displayWidget = new PreferencesDisplayWidget();
    
    m_imageOptionsWidget = new PreferencesImageWidget();
    
    /*
     * Create the tab widget and all tab content
     */
    QTabWidget* tabWidget = new QTabWidget();
    tabWidget->addTab(createColorsWidget(),
                      "Colors");
    tabWidget->addTab(m_developOptionsWidget,
                      "Develop");
    tabWidget->addTab(m_displayWidget,
                      "Displays");
    tabWidget->addTab(createIdentificationSymbolWidget(),
                      "ID");
    tabWidget->addTab(m_imageOptionsWidget,
                      "Image");
    tabWidget->addTab(createMiscellaneousWidget(),
                      "Misc");
    tabWidget->addTab(createTabDefaltsWidget(),
                      "New Tabs");
    tabWidget->addTab(createOpenGLWidget(),
                      "OpenGL");
    tabWidget->addTab(m_recentFilesWidget,
                      "Recent Files");
    
    setCentralWidget(tabWidget,
                           WuQDialog::SCROLL_AREA_NEVER);
    
    disableAutoDefaultForAllPushButtons();
}

/**
 * Destructor.
 */
PreferencesDialog::~PreferencesDialog()
{
    
}

/**
 * Add a color button and swatch.
 *
 * @param gridLayout
 *     Grid layout for widgets.
 * @param prefColor
 *     Enumerated value for color.
 * @param colorSignalMapper
 *     Signal mapper for buttons.
 */
void
PreferencesDialog::addColorButtonAndSwatch(QGridLayout* gridLayout,
                                           const PREF_COLOR prefColor,
                                           QSignalMapper* colorSignalMapper)
{
    QString buttonText;
    AString buttonToolTip;
    QWidget* userColorSwatchWidget(new QWidget());
    QWidget* sceneColorSwatchWidget(new QWidget());
    sceneColorSwatchWidget->setFixedWidth(50);
    
    switch (prefColor) {
        case PREF_COLOR_BACKGROUND_ALL:
            buttonText = "All Background";
            buttonToolTip = "Color for background in All Display";
            m_userPrefsColors.m_backgroundColorAllWidget = userColorSwatchWidget;
            m_scenePrefsColors.m_backgroundColorAllWidget = sceneColorSwatchWidget;
            break;
        case PREF_COLOR_BACKGROUND_CHART:
            buttonText = "Chart Background";
            buttonToolTip = "Color for background in Chart Display";
            m_userPrefsColors.m_backgroundColorChartWidget = userColorSwatchWidget;
            m_scenePrefsColors.m_backgroundColorChartWidget = sceneColorSwatchWidget;
            break;
        case PREF_COLOR_BACKGROUND_SURFACE:
            buttonText = "Surface Background";
            buttonToolTip = "Color for background in Surface Display";
            m_userPrefsColors.m_backgroundColorSurfaceWidget = userColorSwatchWidget;
            m_scenePrefsColors.m_backgroundColorSurfaceWidget = sceneColorSwatchWidget;
            break;
        case PREF_COLOR_BACKGROUND_VOLUME:
            buttonText = "Volume Background";
            buttonToolTip = "Color for background in Volume Display";
            m_userPrefsColors.m_backgroundColorVolumeWidget = userColorSwatchWidget;
            m_scenePrefsColors.m_backgroundColorVolumeWidget = sceneColorSwatchWidget;
            break;
        case PREF_COLOR_FOREGROUND_ALL:
            buttonText = "All Foreground";
            buttonToolTip = "Color for foreground (text) in All Display";
            m_userPrefsColors.m_foregroundColorAllWidget = userColorSwatchWidget;
            m_scenePrefsColors.m_foregroundColorAllWidget = sceneColorSwatchWidget;
            break;
        case PREF_COLOR_FOREGROUND_CHART:
            buttonText = "Chart Foreground";
            buttonToolTip = "Color for foreground (text) in Chart Display";
            m_userPrefsColors.m_foregroundColorChartWidget = userColorSwatchWidget;
            m_scenePrefsColors.m_foregroundColorChartWidget = sceneColorSwatchWidget;
            break;
        case PREF_COLOR_FOREGROUND_SURFACE:
            buttonText = "Surface Foreground";
            buttonToolTip = "Color for foreground (text) in Surface Display";
            m_userPrefsColors.m_foregroundColorSurfaceWidget = userColorSwatchWidget;
            m_scenePrefsColors.m_foregroundColorSurfaceWidget = sceneColorSwatchWidget;
            break;
        case PREF_COLOR_FOREGROUND_VOLUME:
            buttonText = "Volume Foreground";
            buttonToolTip = "Color for foreground (text) in Volume Display";
            m_userPrefsColors.m_foregroundColorVolumeWidget = userColorSwatchWidget;
            m_scenePrefsColors.m_foregroundColorVolumeWidget = sceneColorSwatchWidget;
            break;
        case PREF_COLOR_CHART_MATRIX_GRID_LINES:
            buttonText = "Chart Grid Lines";
            buttonToolTip = "Color for grid lines in a Chart Matrix Display";
            m_userPrefsColors.m_chartMatrixGridLinesColorWidget = userColorSwatchWidget;
            m_scenePrefsColors.m_chartMatrixGridLinesColorWidget = sceneColorSwatchWidget;
            break;
        case PREF_COLOR_CHART_THRESHOLD:
            buttonText = "Chart Threshold";
            buttonToolTip = "Color for thresholded regions in Chart Histogram Display";
            m_userPrefsColors.m_chartHistogramThresholdColorWidget = userColorSwatchWidget;
            m_scenePrefsColors.m_chartHistogramThresholdColorWidget = sceneColorSwatchWidget;
            break;
        case PREF_COLOR_BACKGROUND_WINDOW:
            buttonToolTip = "Color for background in Window";
            buttonText = "Window Background";
            m_userPrefsColors.m_backgroundColorWindowWidget = userColorSwatchWidget;
            m_scenePrefsColors.m_backgroundColorWindowWidget = sceneColorSwatchWidget;
            break;
        case PREF_COLOR_FOREGROUND_WINDOW:
            buttonToolTip = "Color for foreground (text) in Window Display";
            buttonText = "Window Foreground";
            m_userPrefsColors.m_foregroundColorWindowWidget = userColorSwatchWidget;
            m_scenePrefsColors.m_foregroundColorWindowWidget = sceneColorSwatchWidget;
            break;
        case PREF_COLOR_BACKGROUND_HISTOLOGY:
            buttonToolTip = "Color for background in Histology Display";
            buttonText    = "Histology Background";
            m_userPrefsColors.m_backgroundColorHistologyWidget = userColorSwatchWidget;
            m_scenePrefsColors.m_backgroundColorHistologyWidget = sceneColorSwatchWidget;
            break;
        case PREF_COLOR_FOREGROUND_HISTOLOGY:
            buttonToolTip = "Color for foreground (text) in Histology Display";
            buttonText    = "Histology Foreground";
            m_userPrefsColors.m_foregroundColorHistologyWidget = userColorSwatchWidget;
            m_scenePrefsColors.m_foregroundColorHistologyWidget = sceneColorSwatchWidget;
            break;
        case PREF_COLOR_BACKGROUND_MEDIA:
            buttonToolTip = "Color for background in Media Display";
            buttonText    = "Media Background";
            m_userPrefsColors.m_backgroundColorMediaWidget = userColorSwatchWidget;
            m_scenePrefsColors.m_backgroundColorMediaWidget = sceneColorSwatchWidget;
            break;
        case PREF_COLOR_FOREGROUND_MEDIA:
            buttonToolTip = "Color for foreground (text) in Media Display";
            buttonText    = "Media Foreground";
            m_userPrefsColors.m_foregroundColorMediaWidget = userColorSwatchWidget;
            m_scenePrefsColors.m_foregroundColorMediaWidget = sceneColorSwatchWidget;
            break;
        case NUMBER_OF_PREF_COLORS:
            CaretAssert(0);
            break;
    }
    
    buttonText.append("...");
    
    CaretAssert( ! buttonText.isEmpty());
    
    QPushButton* colorPushButton = new QPushButton(buttonText);
    colorPushButton->setToolTip(buttonToolTip);
    QObject::connect(colorPushButton, SIGNAL(clicked()),
                     colorSignalMapper, SLOT(map()));
    colorSignalMapper->setMapping(colorPushButton,
                                  (int)prefColor);
    
    addWidgetsToLayout(gridLayout,
                       colorPushButton,
                       userColorSwatchWidget,
                       sceneColorSwatchWidget);
}


/**
 * @return The colors widget.
 */
QWidget*
PreferencesDialog::createColorsWidget()
{
    QSignalMapper* colorSignalMapper = new QSignalMapper(this);
    
    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->addWidget(new QLabel("User Colors"), 0, 1, Qt::AlignHCenter);
    gridLayout->addWidget(new QLabel("(Editable)"), 1, 1, Qt::AlignHCenter);

    gridLayout->addWidget(new QLabel("From"), 0, 2, Qt::AlignHCenter);
    gridLayout->addWidget(new QLabel("Scene"), 1, 2, Qt::AlignHCenter);

    addColorButtonAndSwatch(gridLayout,
                            PREF_COLOR_FOREGROUND_WINDOW,
                            colorSignalMapper);
    addColorButtonAndSwatch(gridLayout,
                            PREF_COLOR_BACKGROUND_WINDOW,
                            colorSignalMapper);
    
    addColorButtonAndSwatch(gridLayout,
                            PREF_COLOR_FOREGROUND_ALL,
                            colorSignalMapper);
    addColorButtonAndSwatch(gridLayout,
                            PREF_COLOR_BACKGROUND_ALL,
                            colorSignalMapper);
    
    addColorButtonAndSwatch(gridLayout,
                            PREF_COLOR_FOREGROUND_CHART,
                            colorSignalMapper);
    addColorButtonAndSwatch(gridLayout,
                            PREF_COLOR_BACKGROUND_CHART,
                            colorSignalMapper);
    addColorButtonAndSwatch(gridLayout,
                            PREF_COLOR_CHART_MATRIX_GRID_LINES,
                            colorSignalMapper);
    addColorButtonAndSwatch(gridLayout,
                            PREF_COLOR_CHART_THRESHOLD,
                            colorSignalMapper);
    addColorButtonAndSwatch(gridLayout,
                            PREF_COLOR_FOREGROUND_HISTOLOGY,
                            colorSignalMapper);
    addColorButtonAndSwatch(gridLayout,
                            PREF_COLOR_BACKGROUND_HISTOLOGY,
                            colorSignalMapper);
    addColorButtonAndSwatch(gridLayout,
                            PREF_COLOR_FOREGROUND_MEDIA,
                            colorSignalMapper);
    addColorButtonAndSwatch(gridLayout,
                            PREF_COLOR_BACKGROUND_MEDIA,
                            colorSignalMapper);
    addColorButtonAndSwatch(gridLayout,
                            PREF_COLOR_FOREGROUND_SURFACE,
                            colorSignalMapper);
    addColorButtonAndSwatch(gridLayout,
                            PREF_COLOR_BACKGROUND_SURFACE,
                            colorSignalMapper);
    addColorButtonAndSwatch(gridLayout,
                            PREF_COLOR_FOREGROUND_VOLUME,
                            colorSignalMapper);
    addColorButtonAndSwatch(gridLayout,
                            PREF_COLOR_BACKGROUND_VOLUME,
                            colorSignalMapper);
    
    m_sceneColorsActiveCheckBox = new QCheckBox("Scene Colors Active (this can only be turned off)");
    QObject::connect(m_sceneColorsActiveCheckBox, &QCheckBox::clicked,
                     this, &PreferencesDialog::sceneColorsActiveCheckBoxClicked);
    
#if QT_VERSION >= 0x060000
    QObject::connect(colorSignalMapper, &QSignalMapper::mappedInt,
                     this, &PreferencesDialog::colorPushButtonClicked);
#else
    QObject::connect(colorSignalMapper, SIGNAL(mapped(int)),
                     this, SLOT(colorPushButtonClicked(int)));
#endif
    m_allWidgets->add(colorSignalMapper);
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addLayout(gridLayout);
    layout->addWidget(m_sceneColorsActiveCheckBox);
    layout->addStretch();
    return widget;
}

/**
 * Update the color widget's items.
 *
 * @param prefs
 *     The preferences
 * @param colors
 *     The colors
 * @param colorWidgets
 *     The widgets for displaying the colors
 */
void
PreferencesDialog::updateColorWidget(CaretPreferences* prefs,
                                     const BackgroundAndForegroundColors& colors,
                                     ColorWidgets& colorWidgets)
{
    for (int32_t i = 0; i < NUMBER_OF_PREF_COLORS; i++) {
        const PREF_COLOR prefColor = (PREF_COLOR)i;
        
        uint8_t rgb[3] = { 0, 0, 0 };
        QWidget* colorSwatchWidget = NULL;
        
        switch (prefColor) {
            case PREF_COLOR_BACKGROUND_ALL:
                colors.getColorBackgroundAllView(rgb);
                colorSwatchWidget = colorWidgets.m_backgroundColorAllWidget;
                break;
            case PREF_COLOR_BACKGROUND_CHART:
                colors.getColorBackgroundChartView(rgb);
                colorSwatchWidget = colorWidgets.m_backgroundColorChartWidget;
                break;
            case PREF_COLOR_BACKGROUND_SURFACE:
                colors.getColorBackgroundSurfaceView(rgb);
                colorSwatchWidget = colorWidgets.m_backgroundColorSurfaceWidget;
                break;
            case PREF_COLOR_BACKGROUND_VOLUME:
                colors.getColorBackgroundVolumeView(rgb);
                colorSwatchWidget = colorWidgets.m_backgroundColorVolumeWidget;
                break;
            case PREF_COLOR_FOREGROUND_ALL:
                colors.getColorForegroundAllView(rgb);
                colorSwatchWidget = colorWidgets.m_foregroundColorAllWidget;
                break;
            case PREF_COLOR_FOREGROUND_CHART:
                colors.getColorForegroundChartView(rgb);
                colorSwatchWidget = colorWidgets.m_foregroundColorChartWidget;
                break;
            case PREF_COLOR_FOREGROUND_SURFACE:
                colors.getColorForegroundSurfaceView(rgb);
                colorSwatchWidget = colorWidgets.m_foregroundColorSurfaceWidget;
                break;
            case PREF_COLOR_FOREGROUND_VOLUME:
                colors.getColorForegroundVolumeView(rgb);
                colorSwatchWidget = colorWidgets.m_foregroundColorVolumeWidget;
                break;
            case PREF_COLOR_CHART_MATRIX_GRID_LINES:
                colors.getColorChartMatrixGridLines(rgb);
                colorSwatchWidget = colorWidgets.m_chartMatrixGridLinesColorWidget;
                break;
            case PREF_COLOR_CHART_THRESHOLD:
                colors.getColorChartHistogramThreshold(rgb);
                colorSwatchWidget = colorWidgets.m_chartHistogramThresholdColorWidget;
                break;
            case PREF_COLOR_BACKGROUND_WINDOW:
                colors.getColorBackgroundWindow(rgb);
                colorSwatchWidget = colorWidgets.m_backgroundColorWindowWidget;
                break;
            case PREF_COLOR_FOREGROUND_WINDOW:
                colors.getColorForegroundWindow(rgb);
                colorSwatchWidget = colorWidgets.m_foregroundColorWindowWidget;
                break;
            case PREF_COLOR_FOREGROUND_HISTOLOGY:
                colors.getColorForegroundHistologyView(rgb);
                colorSwatchWidget = colorWidgets.m_foregroundColorHistologyWidget;
                break;
            case PREF_COLOR_BACKGROUND_HISTOLOGY:
                colors.getColorBackgroundHistologyView(rgb);
                colorSwatchWidget = colorWidgets.m_backgroundColorHistologyWidget;
                break;
            case PREF_COLOR_FOREGROUND_MEDIA:
                colors.getColorForegroundMediaView(rgb);
                colorSwatchWidget = colorWidgets.m_foregroundColorMediaWidget;
                break;
            case PREF_COLOR_BACKGROUND_MEDIA:
                colors.getColorBackgroundMediaView(rgb);
                colorSwatchWidget = colorWidgets.m_backgroundColorMediaWidget;
                break;
            case NUMBER_OF_PREF_COLORS:
                CaretAssert(0);
                break;
        }

        CaretAssert(colorSwatchWidget);
        
        colorSwatchWidget->setStyleSheet("background-color: rgb("
                                         + AString::number(rgb[0])
                                         + ", " + AString::number(rgb[1])
                                         + ", " + AString::number(rgb[2])
                                         + ");");
    }
    
    switch (prefs->getBackgroundAndForegroundColorsMode()) {
        case BackgroundAndForegroundColorsModeEnum::SCENE:
            m_sceneColorsActiveCheckBox->setChecked(true);
            m_sceneColorsActiveCheckBox->setEnabled(true);
            break;
        case BackgroundAndForegroundColorsModeEnum::USER_PREFERENCES:
            m_sceneColorsActiveCheckBox->setChecked(false);
            m_sceneColorsActiveCheckBox->setEnabled(false);
            break;
    }
}

/**
 * Called when scene colors active checkbox is clicked
 */
void
PreferencesDialog::sceneColorsActiveCheckBoxClicked(bool checked)
{
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    if (checked) {
        prefs->setBackgroundAndForegroundColorsMode(BackgroundAndForegroundColorsModeEnum::SCENE);
    }
    else {
        prefs->setBackgroundAndForegroundColorsMode(BackgroundAndForegroundColorsModeEnum::USER_PREFERENCES);
        m_sceneColorsActiveCheckBox->setEnabled(false);
    }
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}

/**
 * @return The miscellaneous widget.
 */
QWidget*
PreferencesDialog::createMiscellaneousWidget()
{
    /*
     * Dynamic connectivity defautl
     */
    m_dynamicConnectivityComboBox = new WuQTrueFalseComboBox("On",
                                                             "Off",
                                                             this);
    QObject::connect(m_dynamicConnectivityComboBox, SIGNAL(statusChanged(bool)),
                     this, SLOT(miscDynamicConnectivityComboBoxChanged(bool)));
    m_allWidgets->add(m_dynamicConnectivityComboBox);
    m_dynamicConnectivityComboBox->setToolTip("Sets default (checked or unchecked) for dynamic connectivity files "
                                              "on the Overlay ToolBox --> Connectivity tab.");
    
    /*
     * Logging Level
     */
    m_miscLoggingLevelComboBox = new QComboBox();
    std::vector<LogLevelEnum::Enum> loggingLevels;
    LogLevelEnum::getAllEnums(loggingLevels);
    const int32_t numLogLevels = static_cast<int32_t>(loggingLevels.size());
    for (int32_t i = 0; i < numLogLevels; i++) {
        const LogLevelEnum::Enum logLevel = loggingLevels[i];
        m_miscLoggingLevelComboBox->addItem(LogLevelEnum::toGuiName(logLevel));
        m_miscLoggingLevelComboBox->setItemData(i, LogLevelEnum::toIntegerCode(logLevel));
    }
    QObject::connect(m_miscLoggingLevelComboBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(miscLoggingLevelComboBoxChanged(int)));
    
    m_allWidgets->add(m_miscLoggingLevelComboBox);
    
    /*
     * Developer Menu
     */
    m_miscDevelopMenuEnabledComboBox = new WuQTrueFalseComboBox("On",
                                                                "Off",
                                                                this);
    QObject::connect(m_miscDevelopMenuEnabledComboBox, SIGNAL(statusChanged(bool)),
                     this, SLOT(miscDevelopMenuEnabledComboBoxChanged(bool)));
    m_allWidgets->add(m_miscDevelopMenuEnabledComboBox);
    
    /*
     * Gestures enabled
     */
    const QString gesturesToolTip("Pinch two fingers to zoom; Rotate with two fingers");
    m_guiGesturesEnabledComboBox = new WuQTrueFalseComboBox("On",
                                                            "Off",
                                                            this);
    WuQtUtilities::setWordWrappedToolTip(m_guiGesturesEnabledComboBox->getWidget(),
                                         gesturesToolTip);
    QObject::connect(m_guiGesturesEnabledComboBox, &WuQTrueFalseComboBox::statusChanged,
                     this, &PreferencesDialog::miscGuiGesturesEnabledComboBoxChanged);
    m_allWidgets->add(m_guiGesturesEnabledComboBox);
    
    /*
     * Cross at viewport center
     */
    const QString crossToolTip("Display a yellow cross at center of histology and volume views");
    m_crossAtViewportCenterEnabledComboBox = new WuQTrueFalseComboBox("On",
                                                            "Off",
                                                            this);
    WuQtUtilities::setWordWrappedToolTip(m_crossAtViewportCenterEnabledComboBox->getWidget(),
                                         crossToolTip);
    QObject::connect(m_crossAtViewportCenterEnabledComboBox, &WuQTrueFalseComboBox::statusChanged,
                     this, &PreferencesDialog::miscCrossAtViewportCenterEnabledComboBoxChanged);
    m_allWidgets->add(m_crossAtViewportCenterEnabledComboBox);

    /*
     * Manage Files View Files Type
     */
    m_miscSpecFileDialogViewFilesTypeEnumComboBox = new EnumComboBoxTemplate(this);
    m_miscSpecFileDialogViewFilesTypeEnumComboBox->setup<SpecFileDialogViewFilesTypeEnum,SpecFileDialogViewFilesTypeEnum::Enum>();
    QObject::connect(m_miscSpecFileDialogViewFilesTypeEnumComboBox, SIGNAL(itemActivated()),
                     this, SLOT(miscSpecFileDialogViewFilesTypeEnumComboBoxItemActivated()));
    m_allWidgets->add(m_miscSpecFileDialogViewFilesTypeEnumComboBox->getWidget());
    
    /*
     * Toolbar mode
     */
    const QString widthToolTip(ToolBarWidthModeEnum::toGuiName(ToolBarWidthModeEnum::WIDE)
                               + " mode will show \"View\" toolbar components in all Modes but requires  "
                               "a wide monitor (1920 width or greater).");
    m_windowToolBarWidthModeComboBox = new EnumComboBoxTemplate(this);
    m_windowToolBarWidthModeComboBox->setup<ToolBarWidthModeEnum, ToolBarWidthModeEnum::Enum>();
    m_windowToolBarWidthModeComboBox->setToolTip(WuQtUtilities::createWordWrappedToolTipText(widthToolTip));
    QObject::connect(m_windowToolBarWidthModeComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &PreferencesDialog::miscWindowToolBarWidthModeComboBoxItemActivated);

    const QString fileOpenTip("What to do when a file is opened outside of wb_view from the GUI.  "
                              "On MacOS: Double-clicked in Finder");
    m_fileOpenFromOpSysTypeComboBox = new EnumComboBoxTemplate(this);
    m_fileOpenFromOpSysTypeComboBox->setup<FileOpenFromOpSysTypeEnum, FileOpenFromOpSysTypeEnum::Enum>();
    m_fileOpenFromOpSysTypeComboBox->setToolTip(WuQtUtilities::createWordWrappedToolTipText(fileOpenTip));
    QObject::connect(m_fileOpenFromOpSysTypeComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &PreferencesDialog::miscFileOpenFromOpSysTypeComboBoxItemActivated);
    
    m_volumeSurfaceOutlineSeparationSpinBox = new QDoubleSpinBox();
    m_volumeSurfaceOutlineSeparationSpinBox->setRange(0.0, 10000.0);
    m_volumeSurfaceOutlineSeparationSpinBox->setSingleStep(0.01);
    m_volumeSurfaceOutlineSeparationSpinBox->setDecimals(2);
    /*
     * Default is displayed when the spin box is set to the minimum value (0.0)
     * which allows Workbench to compute the separation
     */
    m_volumeSurfaceOutlineSeparationSpinBox->setSpecialValueText("Default"); // Displayed when value is minimum
    
    QObject::connect(m_volumeSurfaceOutlineSeparationSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                     this, &PreferencesDialog::volumeSurfaceOutlineSeparationValueChanged);

    QGridLayout* gridLayout = new QGridLayout();
    addWidgetToLayout(gridLayout,
                      "Dynconn As Layer Default: ",
                      m_dynamicConnectivityComboBox->getWidget());
    addWidgetToLayout(gridLayout,
                      "Logging Level: ",
                      m_miscLoggingLevelComboBox);
    addWidgetToLayout(gridLayout,
                      "Save/Manage View Files: ",
                      m_miscSpecFileDialogViewFilesTypeEnumComboBox->getWidget());
    addWidgetToLayout(gridLayout,
                      "Show Develop Menu in Menu Bar: ",
                      m_miscDevelopMenuEnabledComboBox->getWidget());
    addWidgetToLayout(gridLayout,
                      "Enable Trackpad Gestures: ",
                      m_guiGesturesEnabledComboBox->getWidget());
    addWidgetToLayout(gridLayout,
                      "Open File from MacOS Finder: ",
                      m_fileOpenFromOpSysTypeComboBox->getWidget());
    addWidgetToLayout(gridLayout,
                      "Window ToolBar Width Mode: ",
                      m_windowToolBarWidthModeComboBox->getWidget());
    addWidgetToLayout(gridLayout,
                      "Display Cross at Histology/Volume Center",
                      m_crossAtViewportCenterEnabledComboBox->getWidget());
    addWidgetToLayout(gridLayout,
                      "Volume Surface Outline Separation",
                      m_volumeSurfaceOutlineSeparationSpinBox);
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addLayout(gridLayout);
    layout->addStretch();
    return widget;
}

/**
 * Update the miscellaneous widget's items.
 *
 * @param prefs
 *     The Caret preferences.
 */
void
PreferencesDialog::updateMiscellaneousWidget(CaretPreferences* prefs)
{
    m_dynamicConnectivityComboBox->setStatus(prefs->isDynamicConnectivityDefaultedOn());
    
    const LogLevelEnum::Enum loggingLevel = prefs->getLoggingLevel();
    int indx = m_miscLoggingLevelComboBox->findData(LogLevelEnum::toIntegerCode(loggingLevel));
    if (indx >= 0) {
        m_miscLoggingLevelComboBox->setCurrentIndex(indx);
    }
    
    m_miscDevelopMenuEnabledComboBox->setStatus(prefs->isDevelopMenuEnabled());
    
    m_yokingDefaultComboBox->setStatus(prefs->isYokingDefaultedOn());
    
    m_miscSpecFileDialogViewFilesTypeEnumComboBox->setSelectedItem<SpecFileDialogViewFilesTypeEnum,SpecFileDialogViewFilesTypeEnum::Enum>(prefs->getManageFilesViewFileType());

    m_guiGesturesEnabledComboBox->setStatus(prefs->isGuiGesturesEnabled());
    
    m_windowToolBarWidthModeComboBox->setSelectedItem<ToolBarWidthModeEnum, ToolBarWidthModeEnum::Enum>(prefs->getToolBarWidthMode());
    
    m_fileOpenFromOpSysTypeComboBox->setSelectedItem<FileOpenFromOpSysTypeEnum, FileOpenFromOpSysTypeEnum::Enum>(prefs->getFileOpenFromOpSysType());
    
    m_crossAtViewportCenterEnabledComboBox->setStatus(prefs->isCrossAtViewportCenterEnabled());
    
    QSignalBlocker vsoBlocker(m_volumeSurfaceOutlineSeparationSpinBox);
    m_volumeSurfaceOutlineSeparationSpinBox->setValue(prefs->getVolumeSurfaceOutlineSeparation());
}

/**
 * @return The identification symbol widget.
 */
QWidget*
PreferencesDialog::createIdentificationSymbolWidget()
{
    QLabel* infoLabel = new QLabel("These are defaults for Information Properties");
    infoLabel->setWordWrap(true);
    
    m_histologyIdentificationSymbolComboBox = new WuQTrueFalseComboBox("On", "Off", this);
    QObject::connect(m_histologyIdentificationSymbolComboBox, SIGNAL(statusChanged(bool)),
                     this, SLOT(identificationSymbolToggled()));
    
    m_mediaIdentificationSymbolComboBox = new WuQTrueFalseComboBox("On", "Off", this);
    QObject::connect(m_mediaIdentificationSymbolComboBox, SIGNAL(statusChanged(bool)),
                     this, SLOT(identificationSymbolToggled()));

    m_surfaceIdentificationSymbolComboBox = new WuQTrueFalseComboBox("On", "Off", this);
    QObject::connect(m_surfaceIdentificationSymbolComboBox, SIGNAL(statusChanged(bool)),
                     this, SLOT(identificationSymbolToggled()));
    
    m_volumeIdentificationSymbolComboBox = new WuQTrueFalseComboBox("On", "Off", this);
    QObject::connect(m_volumeIdentificationSymbolComboBox, SIGNAL(statusChanged(bool)),
                     this, SLOT(identificationSymbolToggled()));
    
    m_dataToolTipsComboBox = new WuQTrueFalseComboBox("On", "Off", this);
    QObject::connect(m_dataToolTipsComboBox, SIGNAL(statusChanged(bool)),
                     this, SLOT(identificationSymbolToggled()));
    
    /*
     * Identification Mode
     */
    m_identificationModeComboBox = new EnumComboBoxTemplate(this);
    m_identificationModeComboBox->setup<IdentificationDisplayModeEnum,IdentificationDisplayModeEnum::Enum>();
    QObject::connect(m_identificationModeComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &PreferencesDialog::identificationModeEnumComboBoxItemActivated);
    m_allWidgets->add(m_identificationModeComboBox->getWidget());
    
    /*
     * Max stereotaxic distance for id symbol
     */
    const QString mediaSteretotaxicDistanceToolTip("When an identification symbol is from another source "
                                                   "(eg: showing media symbol on a surface), this value is "
                                                   "the maximum distance that allows display of the symbol.");
    m_identificationStereotaxicDistanceSpinBox = new QDoubleSpinBox();
    m_identificationStereotaxicDistanceSpinBox->setRange(0.0, 100000.0);
    m_identificationStereotaxicDistanceSpinBox->setSingleStep(1.0);
    QObject::connect(m_identificationStereotaxicDistanceSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                     this, &PreferencesDialog::identificationStereotaxicDistanceValueChanged);
    WuQtUtilities::setWordWrappedToolTip(m_identificationStereotaxicDistanceSpinBox,
                                         mediaSteretotaxicDistanceToolTip);
    

    QGridLayout* gridLayout = new QGridLayout();
    int row = gridLayout->rowCount();
    gridLayout->addWidget(infoLabel,
                          row, 0, 1, 2);
    addWidgetToLayout(gridLayout,
                      IdentificationManager::getShowSymbolOnTypeLabel(IdentifiedItemUniversalTypeEnum::HISTOLOGY_PLANE_COORDINATE) + ": ",
                      m_histologyIdentificationSymbolComboBox->getWidget());
    addWidgetToLayout(gridLayout,
                      IdentificationManager::getShowSymbolOnTypeLabel(IdentifiedItemUniversalTypeEnum::MEDIA_LOGICAL_COORDINATE) + ": ",
                      m_mediaIdentificationSymbolComboBox->getWidget());
    addWidgetToLayout(gridLayout,
                      IdentificationManager::getShowSymbolOnTypeLabel(IdentifiedItemUniversalTypeEnum::SURFACE) + ": ",
                      m_surfaceIdentificationSymbolComboBox->getWidget());
    addWidgetToLayout(gridLayout,
                      IdentificationManager::getShowSymbolOnTypeLabel(IdentifiedItemUniversalTypeEnum::VOLUME_SLICES) + ": ",
                      m_volumeIdentificationSymbolComboBox->getWidget());
    addWidgetToLayout(gridLayout,
                      "Show Data Tool Tips: ",
                      m_dataToolTipsComboBox->getWidget());
    addWidgetToLayout(gridLayout,
                      "Identification Display: ",
                      m_identificationModeComboBox->getWidget());
    addWidgetToLayout(gridLayout,
                      "Stereotaxic Distance: ",
                      m_identificationStereotaxicDistanceSpinBox);

    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addLayout(gridLayout);
    layout->addStretch();
    return widget;
}

/**
 * Update the identification widget.
 * 
 * @param prefs
 *     The Caret preferences.
 */
void
PreferencesDialog::updateIdentificationWidget(CaretPreferences* prefs)
{
    m_histologyIdentificationSymbolComboBox->setStatus(prefs->isShowHistologyIdentificationSymbols());
    m_mediaIdentificationSymbolComboBox->setStatus(prefs->isShowMediaIdentificationSymbols());
    m_surfaceIdentificationSymbolComboBox->setStatus(prefs->isShowSurfaceIdentificationSymbols());
    m_volumeIdentificationSymbolComboBox->setStatus(prefs->isShowVolumeIdentificationSymbols());
    m_dataToolTipsComboBox->setStatus(prefs->isShowDataToolTipsEnabled());
    m_identificationModeComboBox->setSelectedItem<IdentificationDisplayModeEnum, IdentificationDisplayModeEnum::Enum>(prefs->getIdentificationDisplayMode());
    QSignalBlocker distBlocker(m_identificationStereotaxicDistanceSpinBox);
    m_identificationStereotaxicDistanceSpinBox->setValue(prefs->getIdentificationStereotaxicDistance());
}

/**
 * Gets called when an identification symbol check box is toggled.
 */
void
PreferencesDialog::identificationSymbolToggled()
{
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->setShowHistologyIdentificationSymbols(m_histologyIdentificationSymbolComboBox->isTrue());
    prefs->setShowMediaIdentificationSymbols(m_mediaIdentificationSymbolComboBox->isTrue());
    prefs->setShowSurfaceIdentificationSymbols(m_surfaceIdentificationSymbolComboBox->isTrue());
    prefs->setShowVolumeIdentificationSymbols(m_volumeIdentificationSymbolComboBox->isTrue());
    prefs->setShowDataToolTipsEnabled(m_dataToolTipsComboBox->isTrue());
}

/**
 * Gets called when an identification display mode is changed
 */
void
PreferencesDialog::identificationModeEnumComboBoxItemActivated()
{
    const IdentificationDisplayModeEnum::Enum idMode = m_identificationModeComboBox->getSelectedItem<IdentificationDisplayModeEnum, IdentificationDisplayModeEnum::Enum>();
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->setIdentificationDisplayMode(idMode);
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
}

/**
 * Gets called when  maximum stereotaxic distance value is changed
 */
void
PreferencesDialog::identificationStereotaxicDistanceValueChanged(double value)
{
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->setIdentificationStereotaxicDistance(value);
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}

/**
 * @return The OpenGL widget.
 */
QWidget*
PreferencesDialog::createOpenGLWidget()
{
    /*
     * Image Capture Method
     */
    m_openGLImageCaptureMethodEnumComboBox = new EnumComboBoxTemplate(this);
    m_openGLImageCaptureMethodEnumComboBox->setup<ImageCaptureMethodEnum,ImageCaptureMethodEnum::Enum>();
    QObject::connect(m_openGLImageCaptureMethodEnumComboBox, SIGNAL(itemActivated()),
                     this, SLOT(openGLImageCaptureMethodEnumComboBoxItemActivated()));
    const AString captureMethodToolTip = ("Sometimes, the default image capture method fails to "
                                          "function correctly and the captured image does not match "
                                          "the content of the graphics window.  If this occurs, "
                                          "try changing the Capture Method to Grab Frame Buffer.");
    WuQtUtilities::setWordWrappedToolTip(m_openGLImageCaptureMethodEnumComboBox->getComboBox(),
                                         captureMethodToolTip);
    m_allWidgets->add(m_openGLImageCaptureMethodEnumComboBox->getWidget());
    
    /*
     * OpenGL Drawing Method
     */
    m_openGLDrawingMethodEnumComboBox = new EnumComboBoxTemplate(this);
    m_openGLDrawingMethodEnumComboBox->setup<OpenGLDrawingMethodEnum,OpenGLDrawingMethodEnum::Enum>();
    QObject::connect(m_openGLDrawingMethodEnumComboBox, SIGNAL(itemActivated()),
                     this, SLOT(openGLDrawingMethodEnumComboBoxItemActivated()));
    m_allWidgets->add(m_openGLDrawingMethodEnumComboBox->getWidget());
    
    
    QGridLayout* gridLayout = new QGridLayout();
    addWidgetToLayout(gridLayout,
                      "Image Capture Method: ",
                      m_openGLImageCaptureMethodEnumComboBox->getWidget());
    QLabel* vertexBuffersLabel = addWidgetToLayout(gridLayout,
                                                         "OpenGL Vertex Buffers: ",
                                                         m_openGLDrawingMethodEnumComboBox->getWidget());
    
    /*
     * HIDE THE VERTEX BUFFERS OPTION
     */
    vertexBuffersLabel->setHidden(true);
    m_openGLDrawingMethodEnumComboBox->getWidget()->setHidden(true);
    
    /*
     * Graphics timing
     */
    m_openGLGraphicsTimingEnabledComboBox = new WuQTrueFalseComboBox("On",
                                                       "Off",
                                                       this);
    QObject::connect(m_openGLGraphicsTimingEnabledComboBox, &WuQTrueFalseComboBox::statusChanged,
                     this, &PreferencesDialog::openGLGraphicsTimingComboBoxToggled);
    addWidgetToLayout(gridLayout,
                      "Show Frames Per Second",
                      m_openGLGraphicsTimingEnabledComboBox->getWidget());
    m_allWidgets->add(m_openGLGraphicsTimingEnabledComboBox);
    
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addLayout(gridLayout);
    layout->addStretch();
    return widget;
}

/**
 * Called when graphics timing combo value changed
 */
void
PreferencesDialog::openGLGraphicsTimingComboBoxToggled(bool value)
{
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->setGraphicsFramesPerSecondEnabled(value);
    
    /*
     * Need to draw a few frames to force creation of frame times so that
     * the text is dislayed in the window
     */
    EventManager::get()->sendEvent(EventGraphicsPaintNowAllWindows().getPointer());
    EventManager::get()->sendEvent(EventGraphicsPaintNowAllWindows().getPointer());
    EventManager::get()->sendEvent(EventGraphicsPaintNowAllWindows().getPointer());
}

/**
 * Update the OpenGL widget's items.
 *
 * @param prefs
 *     The Caret preferences.
 */
void
PreferencesDialog::updateOpenGLWidget(CaretPreferences* prefs)
{
    const ImageCaptureMethodEnum::Enum captureMethod = prefs->getImageCaptureMethod();
    m_openGLImageCaptureMethodEnumComboBox->setSelectedItem<ImageCaptureMethodEnum,ImageCaptureMethodEnum::Enum>(captureMethod);
    
    const OpenGLDrawingMethodEnum::Enum drawingMethod = prefs->getOpenDrawingMethod();
    m_openGLDrawingMethodEnumComboBox->setSelectedItem<OpenGLDrawingMethodEnum,OpenGLDrawingMethodEnum::Enum>(drawingMethod);
    
    m_openGLGraphicsTimingEnabledComboBox->setStatus(prefs->isGraphicsFramesPerSecondEnabled());
}

/**
 * @return The volume widget.
 */
QWidget*
PreferencesDialog::createTabDefaltsWidget()
{
    /*
     * Yoking
     */
    m_yokingDefaultComboBox = new WuQTrueFalseComboBox("On",
                                                       "Off",
                                                       this);
    QObject::connect(m_yokingDefaultComboBox, SIGNAL(statusChanged(bool)),
                     this, SLOT(yokingComboBoxToggled(bool)));
    m_allWidgets->add(m_yokingDefaultComboBox);
    
    /*
     * Crosshairs On/Off
     */
    m_volumeAxesCrosshairsComboBox = new WuQTrueFalseComboBox("On", "Off", this);
    QObject::connect(m_volumeAxesCrosshairsComboBox, SIGNAL(statusChanged(bool)),
                     this, SLOT(volumeAxesCrosshairsComboBoxToggled(bool)));
    m_allWidgets->add(m_volumeAxesCrosshairsComboBox);
    
    /*
     * Axes Labels On/Off
     */
    m_volumeAxesLabelsComboBox = new WuQTrueFalseComboBox("On", "Off", this);
    QObject::connect(m_volumeAxesLabelsComboBox, SIGNAL(statusChanged(bool)),
                     this, SLOT(volumeAxesLabelsComboBoxToggled(bool)));
    m_allWidgets->add(m_volumeAxesLabelsComboBox);
    
    /*
     * Identification On/Off
     */
    m_volumeIdentificationComboBox = new WuQTrueFalseComboBox("On", "Off", this);
    QObject::connect(m_volumeIdentificationComboBox, SIGNAL(statusChanged(bool)),
                     this, SLOT(volumeIdentificationComboBoxToggled(bool)));
    m_allWidgets->add(m_volumeIdentificationComboBox);

    /*
     * Montage Coordinates On/Off
     */
    m_volumeAxesMontageCoordinatesComboBox = new WuQTrueFalseComboBox("On", "Off", this);
    QObject::connect(m_volumeAxesMontageCoordinatesComboBox, SIGNAL(statusChanged(bool)),
                     this, SLOT(volumeAxesMontageCoordinatesComboBoxToggled(bool)));
    m_allWidgets->add(m_volumeAxesMontageCoordinatesComboBox);
    
    /*
     * Montage Slice Coordinate Precision
     */
    m_volumeMontageCoordinatePrecisionSpinBox = WuQFactory::newSpinBoxWithMinMaxStepSignalInt(0,
                                                                                              5,
                                                                                              1,
                                                                                              this,
                                                                                              SLOT(volumeMontageCoordinatePrecisionChanged(int)));
    m_allWidgets->add(m_volumeMontageCoordinatePrecisionSpinBox);
    
    /*
     * All slice planes layout default
     */
    m_volumeAllSlicePlanesLayoutComboBox = new EnumComboBoxTemplate(this);
    m_volumeAllSlicePlanesLayoutComboBox->setup<VolumeSliceViewAllPlanesLayoutEnum,VolumeSliceViewAllPlanesLayoutEnum::Enum>();
    QObject::connect(m_volumeAllSlicePlanesLayoutComboBox, SIGNAL(itemActivated()),
                     this, SLOT(volumeAllSlicePlanesLayoutItemActivated()));
    m_allWidgets->add(m_volumeAllSlicePlanesLayoutComboBox->getWidget());

    
    QGridLayout* gridLayout = new QGridLayout();
    
    QLabel* infoLabel = new QLabel("<html>"
                                   "These are the <b>default settings</b> for new Tabs.  If "
                                   "Yoked to Group A is on, the volume settings are overriden "
                                   "by the yoked Tabs."
                                   "</html>");
    infoLabel->setWordWrap(true);
    
    gridLayout->addWidget(infoLabel,
                          0, 0, 1, 2);
    
    addWidgetToLayout(gridLayout,
                      "Yoked to Group A: ",
                      m_yokingDefaultComboBox->getWidget());
    addWidgetToLayout(gridLayout,
                      "Volume Axes Crosshairs: ",
                      m_volumeAxesCrosshairsComboBox->getWidget());
    addWidgetToLayout(gridLayout,
                      "Volume Axes Labels: ",
                      m_volumeAxesLabelsComboBox->getWidget());
    addWidgetToLayout(gridLayout,
                      "Volume ID Moves Slice Selection: ",
                      m_volumeIdentificationComboBox->getWidget());
    addWidgetToLayout(gridLayout,
                      "Volume Montage Slice Coord: ",
                      m_volumeAxesMontageCoordinatesComboBox->getWidget());
    addWidgetToLayout(gridLayout,
                      "Volume Montage Precision: ",
                      m_volumeMontageCoordinatePrecisionSpinBox);
    addWidgetToLayout(gridLayout,
                      "All Slice Planes Layout: ",
                      m_volumeAllSlicePlanesLayoutComboBox->getWidget());
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addLayout(gridLayout);
    layout->addStretch();
    return widget;
}

/**
 * Update the Volume widget's items.
 *
 * @param prefs
 *     The Caret preferences.
 */
void
PreferencesDialog::updateVolumeWidget(CaretPreferences* prefs)
{
    m_volumeAxesCrosshairsComboBox->setStatus(prefs->isVolumeAxesCrosshairsDisplayed());
    m_volumeAxesLabelsComboBox->setStatus(prefs->isVolumeAxesLabelsDisplayed());
    m_volumeAxesMontageCoordinatesComboBox->setStatus(prefs->isVolumeMontageAxesCoordinatesDisplayed());
    m_volumeIdentificationComboBox->setStatus(prefs->isVolumeIdentificationDefaultedOn());
    m_volumeMontageCoordinatePrecisionSpinBox->setValue(prefs->getVolumeMontageCoordinatePrecision());
    m_volumeAllSlicePlanesLayoutComboBox->setSelectedItem<VolumeSliceViewAllPlanesLayoutEnum, VolumeSliceViewAllPlanesLayoutEnum::Enum>(prefs->getVolumeAllSlicePlanesLayout());
}

/**
 * Add a label in the left column and the widget in the right column.
 *
 * @param gridLayout
 *    The grid layout to which the widgets are added.
 * @param labelText
 *    Text for label.
 * @param widget
 *    Widget for right column.
 * @return
 *    The label that corresponds to the widget.
 */
QLabel*
PreferencesDialog::addWidgetToLayout(QGridLayout* gridLayout,
                                     const QString& labelText,
                                     QWidget* widget)
{
    QLabel* label = new QLabel(labelText);
    label->setAlignment(Qt::AlignRight);
    addWidgetsToLayout(gridLayout,
                             label,
                             widget);
    
    return label;
}

/**
 * Add widgets to the layout.  If rightWidget is NULL,
 * leftItem spans both columns.
 * @param gridLayout
 *    The grid layout
 * @param leftWidget
 *    Widget for left column.
 * @param rightWidget
 *    Widget for right column.
 */
void 
PreferencesDialog::addWidgetsToLayout(QGridLayout* gridLayout,
                                      QWidget* leftWidget,
                                      QWidget* rightWidget)
{
    int row = gridLayout->rowCount();
    if (rightWidget != NULL) {
        gridLayout->addWidget(leftWidget, row, 0);
        gridLayout->addWidget(rightWidget, row, 1);
    }
    else {
        gridLayout->addWidget(leftWidget, row, 0, 1, 2, Qt::AlignLeft);
    }
}

/**
 * Add widgets to the layout.
 * @param gridLayout
 *    The grid layout
 * @param leftWidget
 *    Widget for left column.
 * @param rightWidget
 *    Widget for right column.
 * @param farRightWidget
 *    Widget for far right column.
 */
void
PreferencesDialog::addWidgetsToLayout(QGridLayout* gridLayout,
                                      QWidget* leftWidget,
                                      QWidget* rightWidget,
                                      QWidget* farRightWidget)
{
    CaretAssert(gridLayout);
    CaretAssert(leftWidget);
    CaretAssert(rightWidget);
    CaretAssert(farRightWidget);
    int row = gridLayout->rowCount();
    gridLayout->addWidget(leftWidget, row, 0);
    gridLayout->addWidget(rightWidget, row, 1);
    gridLayout->addWidget(farRightWidget, row, 2);
}

/**
 * Called when changes are made to recent files
 */
void
PreferencesDialog::recentFilesChanged()
{
    updateDialog();
}

/**
 * May be called to update the dialog's content.
 */
void 
PreferencesDialog::updateDialog()
{
    m_allWidgets->blockAllSignals(true);
    
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    updateColorWidget(prefs,
                      prefs->getUserBackgroundAndForegroundColors(),
                      m_userPrefsColors);
    updateColorWidget(prefs,
                      prefs->getSceneBackgroundAndForegroundColors(),
                      m_scenePrefsColors);
    
    updateMiscellaneousWidget(prefs);
    updateIdentificationWidget(prefs);
    updateOpenGLWidget(prefs);
    updateVolumeWidget(prefs);
    m_imageOptionsWidget->updateContent(prefs);
    m_developOptionsWidget->updateContent(prefs);
    m_displayWidget->updateContent(prefs);
    m_recentFilesWidget->updateContent(prefs);
    
    m_allWidgets->blockAllSignals(false);
}

/**
 * Update the colors in the dialog.
 *
 * @param prefColor
 *     Color that will be updated.
 */
void
PreferencesDialog::updateColorWithDialog(const PREF_COLOR prefColor)
{
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    
    const BackgroundAndForegroundColors colors = prefs->getUserBackgroundAndForegroundColors();
    
    uint8_t rgb[3] = { 0, 0, 0 };
    uint8_t otherRGB[3] = { 0, 0, 0 };
    bool otherValidFlag(true);
    AString prefColorName;
    switch (prefColor) {
        case PREF_COLOR_BACKGROUND_ALL:
            colors.getColorBackgroundAllView(rgb);
            colors.getColorForegroundAllView(otherRGB);
            prefColorName = "Background - All";
            break;
        case PREF_COLOR_BACKGROUND_CHART:
            colors.getColorBackgroundChartView(rgb);
            colors.getColorForegroundChartView(otherRGB);
            prefColorName = "Background - Chart";
            break;
        case PREF_COLOR_BACKGROUND_SURFACE:
            colors.getColorBackgroundSurfaceView(rgb);
            colors.getColorForegroundSurfaceView(otherRGB);
            prefColorName = "Background - Surface";
            break;
        case PREF_COLOR_BACKGROUND_VOLUME:
            colors.getColorBackgroundVolumeView(rgb);
            colors.getColorForegroundVolumeView(otherRGB);
            prefColorName = "Background - Volume";
            break;
        case PREF_COLOR_FOREGROUND_ALL:
            colors.getColorForegroundAllView(rgb);
            colors.getColorBackgroundAllView(otherRGB);
            prefColorName = "Foreground - All";
            break;
        case PREF_COLOR_FOREGROUND_CHART:
            colors.getColorForegroundChartView(rgb);
            colors.getColorBackgroundChartView(otherRGB);
            prefColorName = "Foreground - Chart";
            break;
        case PREF_COLOR_FOREGROUND_SURFACE:
            colors.getColorForegroundSurfaceView(rgb);
            colors.getColorBackgroundSurfaceView(otherRGB);
            prefColorName = "Foreground - Surface";
            break;
        case PREF_COLOR_FOREGROUND_VOLUME:
            colors.getColorForegroundVolumeView(rgb);
            colors.getColorBackgroundVolumeView(otherRGB);
            prefColorName = "Foreground - Volume";
            break;
        case PREF_COLOR_CHART_MATRIX_GRID_LINES:
            colors.getColorChartMatrixGridLines(rgb);
            prefColorName = "Chart Matrix Grid Lines";
            otherValidFlag = false;
            break;
        case PREF_COLOR_CHART_THRESHOLD:
            colors.getColorChartHistogramThreshold(rgb);
            prefColorName = "Chart Histogram Threshold";
            otherValidFlag = false;
            break;
        case PREF_COLOR_BACKGROUND_WINDOW:
            colors.getColorBackgroundWindow(rgb);
            colors.getColorForegroundWindow(otherRGB);
            prefColorName = "Background - Window";
            break;
        case PREF_COLOR_FOREGROUND_WINDOW:
            colors.getColorForegroundWindow(rgb);
            colors.getColorBackgroundWindow(otherRGB);
            prefColorName = "Foreground - Window";
            break;
        case PREF_COLOR_BACKGROUND_HISTOLOGY:
            colors.getColorBackgroundHistologyView(rgb);
            colors.getColorForegroundHistologyView(otherRGB);
            prefColorName = "Background - Histology";
            break;
        case PREF_COLOR_FOREGROUND_HISTOLOGY:
            colors.getColorForegroundHistologyView(rgb);
            colors.getColorBackgroundHistologyView(otherRGB);
            prefColorName = "Foreground - Histology";
            break;
        case PREF_COLOR_BACKGROUND_MEDIA:
            colors.getColorBackgroundMediaView(rgb);
            colors.getColorForegroundMediaView(otherRGB);
            prefColorName = "Background - Media";
            break;
        case PREF_COLOR_FOREGROUND_MEDIA:
            colors.getColorForegroundMediaView(rgb);
            colors.getColorBackgroundMediaView(otherRGB);
            prefColorName = "Foreground - Media";
            break;
        case NUMBER_OF_PREF_COLORS:
            CaretAssert(0);
            break;
    }
    
    const QColor initialColor(rgb[0],
                              rgb[1],
                              rgb[2]);
    
    QColorDialog colorDialog(this);
    colorDialog.setOption(QColorDialog::DontUseNativeDialog);
    colorDialog.setWindowTitle(prefColorName);
    colorDialog.setCurrentColor(initialColor);
    
    if (colorDialog.exec() == QColorDialog::Accepted) {
        const QColor newColor = colorDialog.currentColor();
        rgb[0] = newColor.red();
        rgb[1] = newColor.green();
        rgb[2] = newColor.blue();
        
        if (otherValidFlag) {
            if ((rgb[0] == otherRGB[0])
                && (rgb[1] == otherRGB[1])
                && (rgb[2] == otherRGB[2])) {
                const AString msg("Background and Foreground Colors are the same.  Continue?");
                if ( ! WuQMessageBox::warningYesNo(this, msg)) {
                    return;
                }
            }
        }
        
        BackgroundAndForegroundColors colors = prefs->getUserBackgroundAndForegroundColors();
        
        switch (prefColor) {
            case PREF_COLOR_BACKGROUND_ALL:
                colors.setColorBackgroundAllView(rgb);
                break;
            case PREF_COLOR_BACKGROUND_CHART:
                colors.setColorBackgroundChartView(rgb);
                break;
            case PREF_COLOR_BACKGROUND_SURFACE:
                colors.setColorBackgroundSurfaceView(rgb);
                break;
            case PREF_COLOR_BACKGROUND_VOLUME:
                colors.setColorBackgroundVolumeView(rgb);
                break;
            case PREF_COLOR_FOREGROUND_ALL:
                colors.setColorForegroundAllView(rgb);
                break;
            case PREF_COLOR_FOREGROUND_CHART:
                colors.setColorForegroundChartView(rgb);
                break;
            case PREF_COLOR_FOREGROUND_SURFACE:
                colors.setColorForegroundSurfaceView(rgb);
                break;
            case PREF_COLOR_FOREGROUND_VOLUME:
                colors.setColorForegroundVolumeView(rgb);
                break;
            case PREF_COLOR_CHART_MATRIX_GRID_LINES:
                colors.setColorChartMatrixGridLines(rgb);
                break;
            case PREF_COLOR_CHART_THRESHOLD:
                colors.setColorChartHistogramThreshold(rgb);
                break;
            case PREF_COLOR_BACKGROUND_WINDOW:
                colors.setColorBackgroundWindow(rgb);
                break;
            case PREF_COLOR_FOREGROUND_WINDOW:
                colors.setColorForegroundWindow(rgb);
                break;
            case PREF_COLOR_BACKGROUND_HISTOLOGY:
                colors.setColorBackgroundHistologyView(rgb);
                break;
            case PREF_COLOR_FOREGROUND_HISTOLOGY:
                colors.setColorForegroundHistologyView(rgb);
                break;
            case PREF_COLOR_BACKGROUND_MEDIA:
                colors.setColorBackgroundMediaView(rgb);
                break;
            case PREF_COLOR_FOREGROUND_MEDIA:
                colors.setColorForegroundMediaView(rgb);
                break;
            case NUMBER_OF_PREF_COLORS:
                CaretAssert(0);
                break;
        }
        
        prefs->setUserBackgroundAndForegroundColors(colors);
        prefs->setBackgroundAndForegroundColorsMode(BackgroundAndForegroundColorsModeEnum::USER_PREFERENCES);
        
        updateColorWidget(prefs,
                          prefs->getUserBackgroundAndForegroundColors(),
                          m_userPrefsColors);
        
        EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
        EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
    }
}

/**
 * Called when a color button is clicked.
 *
 * @param enumIndex
 *     color enum integer value indicating button that was clicked.
 */
void
PreferencesDialog::colorPushButtonClicked(int enumIndex)
{
    const PREF_COLOR prefColor = (PREF_COLOR)enumIndex;
    updateColorWithDialog(prefColor);
}

/**
 * Called when the logging level is changed.
 * @param int indx
 *   New index of logging level combo box.
 */
void 
PreferencesDialog::miscLoggingLevelComboBoxChanged(int indx)
{    
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    const int32_t logLevelIntegerCode = m_miscLoggingLevelComboBox->itemData(indx).toInt();
    prefs->setLoggingLevel(LogLevelEnum::fromIntegerCode(logLevelIntegerCode, NULL));
}

/**
 * Called when the apply button is pressed.
 */
void PreferencesDialog::applyButtonClicked()
{
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}

/**
 * Called when the OpenGL drawing method is changed.
 */
void
PreferencesDialog::openGLDrawingMethodEnumComboBoxItemActivated()
{
    const OpenGLDrawingMethodEnum::Enum drawingMethod = m_openGLDrawingMethodEnumComboBox->getSelectedItem<OpenGLDrawingMethodEnum,OpenGLDrawingMethodEnum::Enum>();
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->setOpenGLDrawingMethod(drawingMethod);
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}

/**
 * Called when the image capture method is changed.
 */
void
PreferencesDialog::openGLImageCaptureMethodEnumComboBoxItemActivated()
{
    const ImageCaptureMethodEnum::Enum imageCaptureMethod = m_openGLImageCaptureMethodEnumComboBox->getSelectedItem<ImageCaptureMethodEnum,ImageCaptureMethodEnum::Enum>();
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->setImageCaptureMethod(imageCaptureMethod);
}

/**
 * Called when volume crosshairs is toggled.
 * @param value
 *    New value.
 */
void 
PreferencesDialog::volumeAxesCrosshairsComboBoxToggled(bool value)
{
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->setVolumeAxesCrosshairsDisplayed(value);    
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}

/**
 * Called when volume labels is toggled.
 * @param value
 *    New value.
 */
void 
PreferencesDialog::volumeAxesLabelsComboBoxToggled(bool value)
{
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->setVolumeAxesLabelsDisplayed(value);    
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}

/**
 * Called when volume labels is toggled.
 * @param value
 *    New value.
 */
void
PreferencesDialog::volumeAxesMontageCoordinatesComboBoxToggled(bool value)
{
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->setVolumeMontageAxesCoordinatesDisplayed(value);
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}

/**
 * Called when ALL view slice plane layout changed by user
 */
void
PreferencesDialog::volumeAllSlicePlanesLayoutItemActivated()
{
    VolumeSliceViewAllPlanesLayoutEnum::Enum layoutValue = m_volumeAllSlicePlanesLayoutComboBox->getSelectedItem<VolumeSliceViewAllPlanesLayoutEnum, VolumeSliceViewAllPlanesLayoutEnum::Enum>();
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->setVolumeAllSlicePlanesLayout(layoutValue);
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());

}
/**
 * Called when volume montage coordinate precision value is changed.
 */
void
PreferencesDialog::volumeMontageCoordinatePrecisionChanged(int value)
{
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->setVolumeMontageCoordinatePrecision(value);
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}

/**
 * Called when volume identification value is changed.
 */
void
PreferencesDialog::volumeIdentificationComboBoxToggled(bool value)
{
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->setVolumeIdentificationDefaultedOn(value);
}

/**
 * Called when yoking default value is changed.
 */
void
PreferencesDialog::yokingComboBoxToggled(bool value)
{
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->setYokingDefaultedOn(value);
}

/**
 * Called when dynamic connectivity option changed.
 * @param value
 *   New value.
 */
void PreferencesDialog::miscDynamicConnectivityComboBoxChanged(bool value)
{
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->setDynamicConnectivityDefaultedOn(value);
}

/**
 * Called when show develop menu option changed.
 * @param value
 *   New value.
 */
void
PreferencesDialog::miscDevelopMenuEnabledComboBoxChanged(bool value)
{
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->setDevelopMenuEnabled(value);
    
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_BROWSER_WINDOW_MENUS_UPDATE);
}

/**
 * Called when gui gestures enabled changed.
 *
 * @param value
 *   New value.
 */
void
PreferencesDialog::miscGuiGesturesEnabledComboBoxChanged(bool value)
{
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->setGuiGesturesEnabled(value);
}

/**
 * Called when cross at viewport center enabled changed.
 *
 * @param value
 *   New value.
 */
void
PreferencesDialog::miscCrossAtViewportCenterEnabledComboBoxChanged(bool value)
{
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->setCrossAtViewportCenterEnabled(value);
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}

/**
 * Gets called window toolbar mode is changed
 */
void
PreferencesDialog::miscWindowToolBarWidthModeComboBoxItemActivated()
{
    const ToolBarWidthModeEnum::Enum widthMode = m_windowToolBarWidthModeComboBox->getSelectedItem<ToolBarWidthModeEnum, ToolBarWidthModeEnum::Enum>();
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->setToolBarWidthMode(widthMode);
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
}

/**
 * Gets called when file open from O/S type is changed
 */
void
PreferencesDialog::miscFileOpenFromOpSysTypeComboBoxItemActivated()
{
    const FileOpenFromOpSysTypeEnum::Enum openType = m_fileOpenFromOpSysTypeComboBox->getSelectedItem<FileOpenFromOpSysTypeEnum, FileOpenFromOpSysTypeEnum::Enum>();
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->setFileOpenFromOpSysType(openType);
}
/**
 * Gets called when view files type is changed.
 */
void
PreferencesDialog::miscSpecFileDialogViewFilesTypeEnumComboBoxItemActivated()
{
    const SpecFileDialogViewFilesTypeEnum::Enum viewFilesType = m_miscSpecFileDialogViewFilesTypeEnumComboBox->getSelectedItem<SpecFileDialogViewFilesTypeEnum,SpecFileDialogViewFilesTypeEnum::Enum>();
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->setManageFilesViewFileType(viewFilesType);
}

/**
 * Called when volume surface outline separation is changed
 */
void
PreferencesDialog::volumeSurfaceOutlineSeparationValueChanged(double value)
{
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->setVolumeSurfaceOutlineSeparation(value);
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}



