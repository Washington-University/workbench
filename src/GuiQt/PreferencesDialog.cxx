
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
#include "ConnectivityLoaderManager.h"
#include "EventUpdateAnimationStartTime.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "SessionManager.h"
#include "WuQtUtilities.h"
#include "WuQWidgetObjectGroup.h"

using namespace caret;


    
/**
 * \class PreferencesDialog 
 * \brief Dialog for capturing images.
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
    
    QTabWidget* tabWidget = new QTabWidget();
    tabWidget->addTab(this->createColorsWidget(), "Colors");
//    tabWidget->addTab(this->createIdentificationWidget(), "ID");
    tabWidget->addTab(this->createLoggingWidget(), "Logging");
    tabWidget->addTab(this->createVolumeWidget(), "Volume");
    tabWidget->addTab(this->createTimeCourseWidget(), "TimeCourse");
    this->setCentralWidget(tabWidget);
}

/**
 * Destructor.
 */
PreferencesDialog::~PreferencesDialog()
{
    
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
    
    this->volumeAxesCrosshairsCheckBox->setChecked(prefs->isVolumeAxesCrosshairsDisplayed());
    this->volumeAxesLabelsCheckBox->setChecked(prefs->isVolumeAxesLabelsDisplayed());
    
//    this->identificationContralateralCheckBox->setChecked(prefs->isContralateralIdentificationEnabled());
    
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
    QColor newColor = QColorDialog::getColor(initialColor, this, "Background");
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
    QColor newColor = QColorDialog::getColor(initialColor, this, "Foreground");
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
 * @return
 *    Its widget.
 */
QWidget* 
PreferencesDialog::createColorsWidget()
{
    QPushButton* foregroundPushButton = new QPushButton("Set Foreground...");
    QObject::connect(foregroundPushButton, SIGNAL(clicked()),
                     this, SLOT(foregroundColorPushButtonPressed()));
    this->foregroundColorWidget = new QWidget();
    
    QPushButton* backgroundPushButton = new QPushButton("Set Background...");
    QObject::connect(backgroundPushButton, SIGNAL(clicked()),
                     this, SLOT(backgroundColorPushButtonPressed()));
    this->backgroundColorWidget = new QWidget();
    
    QWidget* w = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(w);
    gridLayout->addWidget(foregroundPushButton, 0, 0);
    gridLayout->addWidget(this->foregroundColorWidget, 0, 1);
    gridLayout->addWidget(backgroundPushButton, 1, 0);
    gridLayout->addWidget(this->backgroundColorWidget, 1, 1);

    return w;
}

/**
 * Creates time course widget.
 * @return
 *    Its widget.
 */
QWidget* 
PreferencesDialog::createTimeCourseWidget()
{
    QLabel* animationStartLabel = new QLabel("Animation Starts at: ");
    this->animationStartDoubleSpinBox = new QDoubleSpinBox();
    
    
   
    
    QObject::connect(this->animationStartDoubleSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(animationStartChanged(double)));
    
    this->allWidgets->add(this->animationStartDoubleSpinBox);
    
    QWidget* w = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(w);
    gridLayout->addWidget(animationStartLabel, 0, 0);
    gridLayout->addWidget(this->animationStartDoubleSpinBox, 0, 1);
    return w;
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
 * @return
 *    Its widget.
 */
QWidget* 
PreferencesDialog::createLoggingWidget()
{
    QLabel* loggingLevelLabel = new QLabel("Logging Level: ");
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
    
    QWidget* w = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(w);
    gridLayout->addWidget(loggingLevelLabel, 0, 0);
    gridLayout->addWidget(this->loggingLevelComboBox, 0, 1);
    return w;
}

/**
 * Creates volume widget.
 * @return
 *    Its widget.
 */
QWidget* 
PreferencesDialog::createVolumeWidget()
{
    this->volumeAxesCrosshairsCheckBox = new QCheckBox("Axes Crosshairs");
    QObject::connect(this->volumeAxesCrosshairsCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(volumeAxesCrosshairsCheckBoxToggled(bool)));
    this->volumeAxesLabelsCheckBox = new QCheckBox("Axes Labels");
    QObject::connect(this->volumeAxesLabelsCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(volumeAxesLabelsCheckBoxToggled(bool)));
    
    
    this->allWidgets->add(this->volumeAxesCrosshairsCheckBox);
    this->allWidgets->add(this->volumeAxesLabelsCheckBox);
    
    QWidget* w = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(w);
    gridLayout->addWidget(this->volumeAxesCrosshairsCheckBox, 0, 0);
    gridLayout->addWidget(this->volumeAxesLabelsCheckBox, 1, 0);
    return w;
}

/**
 * Called when volume crosshairs is toggled.
 * @param value
 *    New value.
 */
void 
PreferencesDialog::volumeAxesCrosshairsCheckBoxToggled(bool value)
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
PreferencesDialog::volumeAxesLabelsCheckBoxToggled(bool value)
{
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->setVolumeAxesLabelsDisplayed(value);    
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Creates identification widget.
 * @return
 *    Its widget.
 */
QWidget* 
PreferencesDialog::createIdentificationWidget()
{
    this->identificationContralateralCheckBox = new QCheckBox("Contralateral");
    WuQtUtilities::setToolTipAndStatusTip(this->identificationContralateralCheckBox, 
                                          "Enables contralateral (right<==>left) identification");
    QObject::connect(this->identificationContralateralCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(identificationContralateralCheckBoxToggled(bool)));
    
    
    this->allWidgets->add(this->identificationContralateralCheckBox);
    
    QWidget* w = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(w);
    gridLayout->addWidget(this->identificationContralateralCheckBox, 0, 0);
    return w;
}

/**
 * Called when contralateral checkbox is toggled.
 * @param value
 *    New value.
 */
void 
PreferencesDialog::identificationContralateralCheckBoxToggled(bool /*value*/)
{
//    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
//    prefs->setContralateralIdentificationEnabled(value);    
}


