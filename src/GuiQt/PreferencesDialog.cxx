
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
#include "EnumComboBoxTemplate.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "ImageCaptureMethodEnum.h"
#include "OpenGLDrawingMethodEnum.h"
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
    
    /*
     * Create the tab widget and all tab content
     */
    QTabWidget* tabWidget = new QTabWidget();
    tabWidget->addTab(createColorsWidget(),
                      "Colors");
    tabWidget->addTab(createIdentificationSymbolWidget(),
                      "ID");
    tabWidget->addTab(createMiscellaneousWidget(),
                      "Misc");
    tabWidget->addTab(createTabDefaltsWidget(),
                      "New Tabs");
    tabWidget->addTab(createOpenGLWidget(),
                      "OpenGL");
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
    QWidget* colorSwatchWidget = new QWidget();
    
    switch (prefColor) {
        case PREF_COLOR_BACKGROUND_ALL:
            buttonText = "All Background";
            buttonToolTip = "Color for background in All Display";
            m_backgroundColorAllWidget = colorSwatchWidget;
            break;
        case PREF_COLOR_BACKGROUND_CHART:
            buttonText = "Chart Background";
            buttonToolTip = "Color for background in Chart Display";
            m_backgroundColorChartWidget = colorSwatchWidget;
            break;
        case PREF_COLOR_BACKGROUND_SURFACE:
            buttonText = "Surface Background";
            buttonToolTip = "Color for background in Surface Display";
            m_backgroundColorSurfaceWidget = colorSwatchWidget;
            break;
        case PREF_COLOR_BACKGROUND_VOLUME:
            buttonText = "Volume Background";
            buttonToolTip = "Color for background in Volume Display";
            m_backgroundColorVolumeWidget = colorSwatchWidget;
            break;
        case PREF_COLOR_FOREGROUND_ALL:
            buttonText = "All Foreground";
            buttonToolTip = "Color for foreground (text) in All Display";
            m_foregroundColorAllWidget = colorSwatchWidget;
            break;
        case PREF_COLOR_FOREGROUND_CHART:
            buttonText = "Chart Foreground";
            buttonToolTip = "Color for foreground (text) in Chart Display";
            m_foregroundColorChartWidget = colorSwatchWidget;
            break;
        case PREF_COLOR_FOREGROUND_SURFACE:
            buttonText = "Surface Foreground";
            buttonToolTip = "Color for foreground (text) in Surface Display";
            m_foregroundColorSurfaceWidget = colorSwatchWidget;
            break;
        case PREF_COLOR_FOREGROUND_VOLUME:
            buttonText = "Volume Foreground";
            buttonToolTip = "Color for foreground (text) in Volume Display";
            m_foregroundColorVolumeWidget = colorSwatchWidget;
            break;
        case PREF_COLOR_CHART_MATRIX_GRID_LINES:
            buttonText = "Chart Grid Lines";
            buttonToolTip = "Color for grid lines in a Chart Matrix Display";
            m_chartMatrixGridLinesColorWidget = colorSwatchWidget;
            break;
        case PREF_COLOR_CHART_THRESHOLD:
            buttonText = "Chart Threshold";
            buttonToolTip = "Color for thresholded regions in Chart Histogram Display";
            m_chartHistogramThresholdColorWidget = colorSwatchWidget;
            break;
        case PREF_COLOR_BACKGROUND_WINDOW:
            buttonToolTip = "Color for background in Window";
            buttonText = "Window Background";
            m_backgroundColorWindowWidget = colorSwatchWidget;
            break;
        case PREF_COLOR_FOREGROUND_WINDOW:
            buttonToolTip = "Color for foreground (text) in Window Display";
            buttonText = "Window Foreground";
            m_foregroundColorWindowWidget = colorSwatchWidget;
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
                       colorSwatchWidget);
}


/**
 * @return The colors widget.
 */
QWidget*
PreferencesDialog::createColorsWidget()
{
    QSignalMapper* colorSignalMapper = new QSignalMapper(this);
    
    QGridLayout* gridLayout = new QGridLayout();
    
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
    
    
    
    QObject::connect(colorSignalMapper, SIGNAL(mapped(int)),
                     this, SLOT(colorPushButtonClicked(int)));
    m_allWidgets->add(colorSignalMapper);
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addLayout(gridLayout);
    layout->addStretch();
    return widget;
}

/**
 * Update the color widget's items.
 *
 * @param prefs
 *     The Caret preferences.
 */
void
PreferencesDialog::updateColorWidget(CaretPreferences* prefs)
{
    const BackgroundAndForegroundColors colors = prefs->getUserBackgroundAndForegroundColors();
    
    for (int32_t i = 0; i < NUMBER_OF_PREF_COLORS; i++) {
        const PREF_COLOR prefColor = (PREF_COLOR)i;
        
        uint8_t rgb[3] = { 0, 0, 0 };
        QWidget* colorSwatchWidget = NULL;
        
        switch (prefColor) {
            case PREF_COLOR_BACKGROUND_ALL:
                colors.getColorBackgroundAllView(rgb);
                colorSwatchWidget = m_backgroundColorAllWidget;
                break;
            case PREF_COLOR_BACKGROUND_CHART:
                colors.getColorBackgroundChartView(rgb);
                colorSwatchWidget = m_backgroundColorChartWidget;
                break;
            case PREF_COLOR_BACKGROUND_SURFACE:
                colors.getColorBackgroundSurfaceView(rgb);
                colorSwatchWidget = m_backgroundColorSurfaceWidget;
                break;
            case PREF_COLOR_BACKGROUND_VOLUME:
                colors.getColorBackgroundVolumeView(rgb);
                colorSwatchWidget = m_backgroundColorVolumeWidget;
                break;
            case PREF_COLOR_FOREGROUND_ALL:
                colors.getColorForegroundAllView(rgb);
                colorSwatchWidget = m_foregroundColorAllWidget;
                break;
            case PREF_COLOR_FOREGROUND_CHART:
                colors.getColorForegroundChartView(rgb);
                colorSwatchWidget = m_foregroundColorChartWidget;
                break;
            case PREF_COLOR_FOREGROUND_SURFACE:
                colors.getColorForegroundSurfaceView(rgb);
                colorSwatchWidget = m_foregroundColorSurfaceWidget;
                break;
            case PREF_COLOR_FOREGROUND_VOLUME:
                colors.getColorForegroundVolumeView(rgb);
                colorSwatchWidget = m_foregroundColorVolumeWidget;
                break;
            case PREF_COLOR_CHART_MATRIX_GRID_LINES:
                colors.getColorChartMatrixGridLines(rgb);
                colorSwatchWidget = m_chartMatrixGridLinesColorWidget;
                break;
            case PREF_COLOR_CHART_THRESHOLD:
                colors.getColorChartHistogramThreshold(rgb);
                colorSwatchWidget = m_chartHistogramThresholdColorWidget;
                break;
            case PREF_COLOR_BACKGROUND_WINDOW:
                colors.getColorBackgroundWindow(rgb);
                colorSwatchWidget = m_backgroundColorWindowWidget;
                break;
            case PREF_COLOR_FOREGROUND_WINDOW:
                colors.getColorForegroundWindow(rgb);
                colorSwatchWidget = m_foregroundColorWindowWidget;
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
     * Splash Screen
     */
    m_miscSplashScreenShowAtStartupComboBox = new WuQTrueFalseComboBox("On",
                                                                       "Off",
                                                                       this);
    QObject::connect(m_miscSplashScreenShowAtStartupComboBox, SIGNAL(statusChanged(bool)),
                     this, SLOT(miscSplashScreenShowAtStartupComboBoxChanged(bool)));
    m_allWidgets->add(m_miscSplashScreenShowAtStartupComboBox);
    
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
                      "Show Splash Screen at Startup: ",
                      m_miscSplashScreenShowAtStartupComboBox->getWidget());
    addWidgetToLayout(gridLayout,
                      "Enable Trackpad Gestures",
                      m_guiGesturesEnabledComboBox->getWidget());
    addWidgetToLayout(gridLayout,
                      "Window ToolBar Width Mode",
                      m_windowToolBarWidthModeComboBox->getWidget());
    
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
    
    m_miscSplashScreenShowAtStartupComboBox->setStatus(prefs->isSplashScreenEnabled());
    
    m_yokingDefaultComboBox->setStatus(prefs->isYokingDefaultedOn());
    
    m_miscSpecFileDialogViewFilesTypeEnumComboBox->setSelectedItem<SpecFileDialogViewFilesTypeEnum,SpecFileDialogViewFilesTypeEnum::Enum>(prefs->getManageFilesViewFileType());

    m_guiGesturesEnabledComboBox->setStatus(prefs->isGuiGesturesEnabled());
    
    m_windowToolBarWidthModeComboBox->setSelectedItem<ToolBarWidthModeEnum, ToolBarWidthModeEnum::Enum>(prefs->getToolBarWidthMode());
}

/**
 * @return The identification symbol widget.
 */
QWidget*
PreferencesDialog::createIdentificationSymbolWidget()
{
    QLabel* infoLabel = new QLabel("These are defaults for Information Properties");
    infoLabel->setWordWrap(true);
    
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
    
    
    QGridLayout* gridLayout = new QGridLayout();
    int row = gridLayout->rowCount();
    gridLayout->addWidget(infoLabel,
                          row, 0, 1, 2);
    addWidgetToLayout(gridLayout,
                      "Show Surface ID Symbols: ",
                      m_surfaceIdentificationSymbolComboBox->getWidget());
    addWidgetToLayout(gridLayout,
                      "Show Volume ID Symbols: ",
                      m_volumeIdentificationSymbolComboBox->getWidget());
    addWidgetToLayout(gridLayout,
                      "Show Data Tool Tips: ",
                      m_dataToolTipsComboBox->getWidget());
    addWidgetToLayout(gridLayout,
                      "Identification Display: ",
                      m_identificationModeComboBox->getWidget());


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
    m_surfaceIdentificationSymbolComboBox->setStatus(prefs->isShowSurfaceIdentificationSymbols());
    m_volumeIdentificationSymbolComboBox->setStatus(prefs->isShowVolumeIdentificationSymbols());
    m_dataToolTipsComboBox->setStatus(prefs->isShowDataToolTipsEnabled());
    m_identificationModeComboBox->setSelectedItem<IdentificationDisplayModeEnum, IdentificationDisplayModeEnum::Enum>(prefs->getIdentificationDisplayMode());
}

/**
 * Gets called when an identification symbol check box is toggled.
 */
void
PreferencesDialog::identificationSymbolToggled()
{
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
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
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addLayout(gridLayout);
    layout->addStretch();
    return widget;
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
 *
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
 * May be called to update the dialog's content.
 */
void 
PreferencesDialog::updateDialog()
{
    m_allWidgets->blockAllSignals(true);
    
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    updateColorWidget(prefs);
    updateMiscellaneousWidget(prefs);
    updateIdentificationWidget(prefs);
    updateOpenGLWidget(prefs);
    updateVolumeWidget(prefs);
    
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
    AString prefColorName;
    switch (prefColor) {
        case PREF_COLOR_BACKGROUND_ALL:
            colors.getColorBackgroundAllView(rgb);
            prefColorName = "Background - All";
            break;
        case PREF_COLOR_BACKGROUND_CHART:
            colors.getColorBackgroundChartView(rgb);
            prefColorName = "Background - Chart";
            break;
        case PREF_COLOR_BACKGROUND_SURFACE:
            colors.getColorBackgroundSurfaceView(rgb);
            prefColorName = "Background - Surface";
            break;
        case PREF_COLOR_BACKGROUND_VOLUME:
            colors.getColorBackgroundVolumeView(rgb);
            prefColorName = "Background - Volume";
            break;
        case PREF_COLOR_FOREGROUND_ALL:
            colors.getColorForegroundAllView(rgb);
            prefColorName = "Foreground - All";
            break;
        case PREF_COLOR_FOREGROUND_CHART:
            colors.getColorForegroundChartView(rgb);
            prefColorName = "Foreground - Chart";
            break;
        case PREF_COLOR_FOREGROUND_SURFACE:
            colors.getColorForegroundSurfaceView(rgb);
            prefColorName = "Foreground - Surface";
            break;
        case PREF_COLOR_FOREGROUND_VOLUME:
            colors.getColorForegroundVolumeView(rgb);
            prefColorName = "Foreground - Volume";
            break;
        case PREF_COLOR_CHART_MATRIX_GRID_LINES:
            colors.getColorChartMatrixGridLines(rgb);
            prefColorName = "Chart Matrix Grid Lines";
            break;
        case PREF_COLOR_CHART_THRESHOLD:
            colors.getColorChartHistogramThreshold(rgb);
            prefColorName = "Chart Histogram Threshold";
            break;
        case PREF_COLOR_BACKGROUND_WINDOW:
            colors.getColorBackgroundWindow(rgb);
            prefColorName = "Background - Window";
            break;
        case PREF_COLOR_FOREGROUND_WINDOW:
            colors.getColorForegroundWindow(rgb);
            prefColorName = "Foreground - Window";
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
            case NUMBER_OF_PREF_COLORS:
                CaretAssert(0);
                break;
        }
        
        prefs->setUserBackgroundAndForegroundColors(colors);
        prefs->setBackgroundAndForegroundColorsMode(BackgroundAndForegroundColorsModeEnum::USER_PREFERENCES);
        
        updateColorWidget(prefs);
        
        EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
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
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
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
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
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
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
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
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
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
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
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
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());

}
/**
 * Called when volume montage coordinate precision value is changed.
 */
void
PreferencesDialog::volumeMontageCoordinatePrecisionChanged(int value)
{
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->setVolumeMontageCoordinatePrecision(value);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
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
 * Called when show splash screen option changed.
 * @param value
 *   New value.
 */
void PreferencesDialog::miscSplashScreenShowAtStartupComboBoxChanged(bool value)
{
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->setSplashScreenEnabled(value);
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
 * Gets called window toolbar mode is changed
 */
void
PreferencesDialog::miscWindowToolBarWidthModeComboBoxItemActivated()
{
    const ToolBarWidthModeEnum::Enum widthMode = m_windowToolBarWidthModeComboBox->getSelectedItem<ToolBarWidthModeEnum, ToolBarWidthModeEnum::Enum>();
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->setToolBarWidthMode(widthMode);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
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


