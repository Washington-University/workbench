
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

#include <QBoxLayout>
#include <QColorDialog>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QTabWidget>

#define __PREFERENCES_DIALOG__H__DECLARE__
#include "PreferencesDialog.h"
#undef __PREFERENCES_DIALOG__H__DECLARE__

#include "Brain.h"
#include "CaretLogger.h"
#include "CaretPreferences.h"
#include "EventUpdateAnimationStartTime.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "GuiManager.h"
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
    this->setDeleteWhenClosed(false);

    /*
     * No apply button
     */
    this->setApplyButtonText("");    
    
    /*
     * Used to block signals in all widgets
     */
    this->allWidgets = new WuQWidgetObjectGroup(this);
    
    QWidget* widget = new QWidget();
    this->gridLayout = new QGridLayout(widget);
    
    this->addColorItems();
    this->addLoggingItems();
    this->addSplashItems();
    this->addTimeCourseItems();
    this->addVolumeItems();
    this->addDevelopItems();
    
    this->setCentralWidget(widget);
}

/**
 * Destructor.
 */
PreferencesDialog::~PreferencesDialog()
{
    
}

/**
 * Add a label in the left column and the widget in the right column.
 * @param labelText
 *    Text for label.
 * @param widget
 *    Widget for right column.
 */
void 
PreferencesDialog::addWidgetToLayout(const QString& labelText,
                                     QWidget* widget)
{
    QLabel* label = new QLabel(labelText);
    label->setAlignment(Qt::AlignRight);
    this->addWidgetsToLayout(label, 
                             widget);
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
PreferencesDialog::addWidgetsToLayout(QWidget* leftWidget,
                                   QWidget* rightWidget)
{
    int row = this->gridLayout->rowCount();
    if (rightWidget != NULL) {
        this->gridLayout->addWidget(leftWidget, row, 0);
        this->gridLayout->addWidget(rightWidget, row, 1);
    }
    else {
        this->gridLayout->addWidget(leftWidget, row, 0, 1, 2, Qt::AlignLeft);
    }
}

/**
 * May be called to update the dialog's content.
 */
void 
PreferencesDialog::updateDialog()
{
    this->allWidgets->blockAllSignals(true);
    
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    
    uint8_t backgroundColor[3];
    prefs->getColorBackground(backgroundColor);
    this->backgroundColorWidget->setStyleSheet("background-color: rgb("
                                               + AString::number(backgroundColor[0])
                                               + ", " + AString::number(backgroundColor[1])
                                               + ", " + AString::number(backgroundColor[2])
                                               + ");");

    uint8_t foregroundColor[3];
    prefs->getColorForeground(foregroundColor);
    this->foregroundColorWidget->setStyleSheet("background-color: rgb("
                                               + AString::number(foregroundColor[0])
                                               + ", " + AString::number(foregroundColor[1])
                                               + ", " + AString::number(foregroundColor[2])
                                               + ");");
    
    const LogLevelEnum::Enum loggingLevel = prefs->getLoggingLevel();
    int indx = this->loggingLevelComboBox->findData(LogLevelEnum::toIntegerCode(loggingLevel));
    if (indx >= 0) {
        this->loggingLevelComboBox->setCurrentIndex(indx);
    }
        
    this->volumeAxesCrosshairsComboBox->setStatus(prefs->isVolumeAxesCrosshairsDisplayed());
    this->volumeAxesLabelsComboBox->setStatus(prefs->isVolumeAxesLabelsDisplayed());
    this->volumeAxesMontageCoordinatesComboBox->setStatus(prefs->isVolumeMontageAxesCoordinatesDisplayed());
    this->volumeMontageGapSpinBox->setValue(prefs->getVolumeMontageGap());
    this->splashScreenShowAtStartupComboBox->setStatus(prefs->isSplashScreenEnabled());
    this->developMenuEnabledComboBox->setStatus(prefs->isDevelopMenuEnabled());
    
    this->allWidgets->blockAllSignals(false);
}

/**
 * Called when the background color push button is pressed. 
 */
void 
PreferencesDialog::backgroundColorPushButtonPressed()
{
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    uint8_t backgroundColor[3];
    prefs->getColorBackground(backgroundColor);
    const QColor initialColor(backgroundColor[0],
                              backgroundColor[1],
                              backgroundColor[2]);
    QColor newColor = QColorDialog::getColor(initialColor,
                                             this,
                                             "Background",
                                             QColorDialog::DontUseNativeDialog);
    if (newColor.isValid()) {
        backgroundColor[0] = newColor.red();
        backgroundColor[1] = newColor.green();
        backgroundColor[2] = newColor.blue();
        prefs->setColorBackground(backgroundColor);
        this->updateDialog();
        this->applyButtonPressed();
    }
    
}

/**
 * Called when the foreground color push button is pressed. 
 */
void 
PreferencesDialog::foregroundColorPushButtonPressed()
{
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    uint8_t foregroundColor[3];
    prefs->getColorForeground(foregroundColor);
    const QColor initialColor(foregroundColor[0],
                              foregroundColor[1],
                              foregroundColor[2]);
    QColor newColor = QColorDialog::getColor(initialColor,
                                             this,
                                             "Foreground",
                                             QColorDialog::DontUseNativeDialog);
    if (newColor.isValid()) {
        foregroundColor[0] = newColor.red();
        foregroundColor[1] = newColor.green();
        foregroundColor[2] = newColor.blue();
        prefs->setColorForeground(foregroundColor);
        this->updateDialog();
        this->applyButtonPressed();
    }
}

/**
 * Called when the logging level is changed.
 * @param int indx
 *   New index of logging level combo box.
 */
void 
PreferencesDialog::loggingLevelComboBoxChanged(int indx)
{    
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    const int32_t logLevelIntegerCode = this->loggingLevelComboBox->itemData(indx).toInt();
    prefs->setLoggingLevel(LogLevelEnum::fromIntegerCode(logLevelIntegerCode, NULL));
}

/**
 * Called when the apply button is pressed.
 */
void PreferencesDialog::applyButtonPressed()
{
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Creates colors widget.
 */
void
PreferencesDialog::addColorItems()
{
    QPushButton* foregroundPushButton = new QPushButton("Foreground Color...");
    QObject::connect(foregroundPushButton, SIGNAL(clicked()),
                     this, SLOT(foregroundColorPushButtonPressed()));
    this->foregroundColorWidget = new QWidget();
    
    QPushButton* backgroundPushButton = new QPushButton("Background Color...");
    QObject::connect(backgroundPushButton, SIGNAL(clicked()),
                     this, SLOT(backgroundColorPushButtonPressed()));
    this->backgroundColorWidget = new QWidget();
    
    this->addWidgetsToLayout(backgroundPushButton, 
                             this->backgroundColorWidget);
    this->addWidgetsToLayout(foregroundPushButton, 
                             this->foregroundColorWidget);
}

/**
 * Creates time course widget.
 */
void 
PreferencesDialog::addTimeCourseItems()
{
    this->animationStartDoubleSpinBox = WuQFactory::newDoubleSpinBox();
    QObject::connect(this->animationStartDoubleSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(animationStartChanged(double)));
    
    this->allWidgets->add(this->animationStartDoubleSpinBox);

    /*double time;
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->getAnimationStartTime(time);
    this->animationStartDoubleSpinBox->blockSignals(true);
    this->animationStartDoubleSpinBox->setValue(time);    
    this->animationStartDoubleSpinBox->blockSignals(false);

    this->addWidgetToLayout("Timecourse Animation Starts at: ", 
                             this->animationStartDoubleSpinBox);*/
}

/**
 * Slot for animationStartDoubleSpinBox valueChanged
 */
void
PreferencesDialog::animationStartChanged(double value)
{
   CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
   prefs->setAnimationStartTime(value); 
   EventUpdateAnimationStartTime e;
   e.setStartTime(value);
   EventManager::get()->sendEvent(e.getPointer());
   
}
/**
 * Creates logging widget.
 */
void
PreferencesDialog::addLoggingItems()
{
    this->loggingLevelComboBox = new QComboBox();
    
    std::vector<LogLevelEnum::Enum> loggingLevels;
    LogLevelEnum::getAllEnums(loggingLevels);
    const int32_t numLogLevels = static_cast<int32_t>(loggingLevels.size());
    for (int32_t i = 0; i < numLogLevels; i++) {
        const LogLevelEnum::Enum logLevel = loggingLevels[i];
        this->loggingLevelComboBox->addItem(LogLevelEnum::toGuiName(logLevel));
        this->loggingLevelComboBox->setItemData(i, LogLevelEnum::toIntegerCode(logLevel));
    }
    QObject::connect(this->loggingLevelComboBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(loggingLevelComboBoxChanged(int)));
    
    this->allWidgets->add(this->loggingLevelComboBox);
    
    this->addWidgetToLayout("Logging Level: ", this->loggingLevelComboBox);
}

/**
 * Creates volume widget.
 */
void
PreferencesDialog::addVolumeItems()
{
    this->volumeAxesCrosshairsComboBox = new WuQTrueFalseComboBox("On", "Off", this);
    QObject::connect(this->volumeAxesCrosshairsComboBox, SIGNAL(statusChanged(bool)),
                     this, SLOT(volumeAxesCrosshairsComboBoxToggled(bool)));
    this->volumeAxesLabelsComboBox = new WuQTrueFalseComboBox("On", "Off", this);
    QObject::connect(this->volumeAxesLabelsComboBox, SIGNAL(statusChanged(bool)),
                     this, SLOT(volumeAxesLabelsComboBoxToggled(bool)));
    
    this->volumeAxesMontageCoordinatesComboBox = new WuQTrueFalseComboBox("On", "Off", this);
    QObject::connect(this->volumeAxesMontageCoordinatesComboBox, SIGNAL(statusChanged(bool)),
                     this, SLOT(volumeAxesMontageCoordinatesComboBoxToggled(bool)));
    
    this->volumeMontageGapSpinBox = WuQFactory::newSpinBoxWithMinMaxStepSignalInt(0,
                                                                                  100000,
                                                                                  1,
                                                                                  this,
                                                                                  SLOT(volumeMontageGapValueChanged(int)));
    this->allWidgets->add(this->volumeAxesCrosshairsComboBox);
    this->allWidgets->add(this->volumeAxesLabelsComboBox);
    this->allWidgets->add(this->volumeAxesMontageCoordinatesComboBox);
    this->allWidgets->add(this->volumeMontageGapSpinBox);
    
    this->addWidgetToLayout("Volume Axes Crosshairs: ", 
                            this->volumeAxesCrosshairsComboBox->getWidget());
    this->addWidgetToLayout("Volume Axes Labels: ",
                            this->volumeAxesLabelsComboBox->getWidget());
    this->addWidgetToLayout("Volume Montage Slice Coord: ",
                            this->volumeAxesMontageCoordinatesComboBox->getWidget());
    this->addWidgetToLayout("Volume Montage Gap: ",
                             this->volumeMontageGapSpinBox);
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
 * Called when volume montage gap value is changed.
 */
void
PreferencesDialog::volumeMontageGapValueChanged(int value)
{
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->setVolumeMontageGap(value);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Add splash screen items.
 */
void PreferencesDialog::addSplashItems()
{
    this->splashScreenShowAtStartupComboBox = new WuQTrueFalseComboBox("On",
                                                                       "Off",
                                                                       this);
    QObject::connect(this->splashScreenShowAtStartupComboBox, SIGNAL(statusChanged(bool)),
                     this, SLOT(splashScreenShowAtStartupComboBoxChanged(bool)));
    this->addWidgetToLayout("Show Splash Screen at Startup: ", 
                            this->splashScreenShowAtStartupComboBox->getWidget());
}

/**
 * Called when show splash screen option changed.
 * @param value
 *   New value.
 */
void PreferencesDialog::splashScreenShowAtStartupComboBoxChanged(bool value)
{
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->setSplashScreenEnabled(value);
}

/**
 * Add develop items.
 */
void
PreferencesDialog::addDevelopItems()
{
    this->developMenuEnabledComboBox = new WuQTrueFalseComboBox("On",
                                                                "Off",
                                                                this);
    QObject::connect(this->developMenuEnabledComboBox, SIGNAL(statusChanged(bool)),
                     this, SLOT(developMenuEnabledComboBoxChanged(bool)));
    this->addWidgetToLayout("Show Develop Menu in Menu Bar: ",
                            this->developMenuEnabledComboBox->getWidget());
}

/**
 * Called when show develop menu option changed.
 * @param value
 *   New value.
 */
void
PreferencesDialog::developMenuEnabledComboBoxChanged(bool value)
{
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->setDevelopMenuEnabled(value);
    
    const AString msg = ("The Develop menu will "
                         + QString((value ? "appear" : " not appear"))
                         + " in newly opened windows.");
    WuQMessageBox::informationOk(this->developMenuEnabledComboBox->getWidget(),
                                 msg);
}


