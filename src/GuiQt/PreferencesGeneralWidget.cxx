
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

#define __PREFERENCES_GENERAL_WIDGET_DECLARE__
#include "PreferencesGeneralWidget.h"
#undef __PREFERENCES_GENERAL_WIDGET_DECLARE__

    
/**
 * \class caret::PreferencesGeneralWidget
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
#include "GuiDarkLightThemeModeEnum.h"
#include "ImageFile.h"
#include "PreferencesDialog.h"
#include "WuQMessageBox.h"
#include "WuQTrueFalseComboBox.h"
#include "WuQtUtilities.h"

#ifdef CARET_OS_MACOSX
#include "MacDarkTheme.h"
#endif

using namespace caret;

//Add GuiDarkLightThemeModeEnum.h and call functions in MacDarkTheme.h
//to test and see if it works
//Create a dark theme class that calls o/s specific functions with an "isSupported" method.
//Linux dark theme???

/**
 * Constructor.
 */
PreferencesGeneralWidget::PreferencesGeneralWidget(QWidget* parent)
: QWidget(parent)
{
    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setColumnStretch(2, 100);
    
    /*
     * Image texture minification filter
     */
    m_darkLightThemeModeEnumComboBox = new EnumComboBoxTemplate(this);
    m_darkLightThemeModeEnumComboBox->setup<GuiDarkLightThemeModeEnum,GuiDarkLightThemeModeEnum::Enum>();
    QObject::connect(m_darkLightThemeModeEnumComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &PreferencesGeneralWidget::darkLightThemeModeEnumComboBoxItemActivated);
    //    WuQtUtilities::setWordWrappedToolTip(m_darkLightThemeModeEnumComboBox->getWidget(),
    //                                         GuiDarkLightThemeModeEnum::toToolTip());
    /*QLabel* darkLightThemeLabel =*/ PreferencesDialog::addWidgetToLayout(gridLayout,
                                                                       "Appearance",
                                                                       m_darkLightThemeModeEnumComboBox->getWidget());
    //    WuQtUtilities::setWordWrappedToolTip(darkLightThemeLabel,
    //                                         GuiDarkLightThemeModeEnum::toToolTip());
    
    
    /*
     * Layouts
     */
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addLayout(gridLayout);
    layout->addStretch();
    
#ifdef CARET_OS_MACOSX
#else
    setEnabled(false);
#endif
}

/**
 * Destructor.
 */
PreferencesGeneralWidget::~PreferencesGeneralWidget()
{
}

/*
 * Update the content in this widget
 * @param caretPreferences
 *    The caret preferences
 */
void
PreferencesGeneralWidget::updateContent(CaretPreferences* caretPreferences)
{
    m_preferences = caretPreferences;
    CaretAssert(m_preferences);

//    const GuiDarkLightThemeModeEnum::Enum minFilter  = BrainOpenGLMediaDrawing::getTextureMinificationFilter();
//    m_darkLightThemeModeEnumComboBox->setSelectedItem<GuiDarkLightThemeModeEnum,GuiDarkLightThemeModeEnum::Enum>(minFilter);
}

/**
 * Called when graphics minification filter changed
 */
void
PreferencesGeneralWidget::darkLightThemeModeEnumComboBoxItemActivated()
{
    const GuiDarkLightThemeModeEnum::Enum darkLightModeEnum = m_darkLightThemeModeEnumComboBox->getSelectedItem<GuiDarkLightThemeModeEnum,GuiDarkLightThemeModeEnum::Enum>();
//    BrainOpenGLMediaDrawing::setTextureMinificationFilter(minFilter);
    
#ifdef CARET_OS_MACOSX
    switch (darkLightModeEnum) {
        case GuiDarkLightThemeModeEnum::SYSTEM:
            macSetToAutoTheme();
            break;
        case GuiDarkLightThemeModeEnum::DARK:
            macSetToDarkTheme();
            break;
        case GuiDarkLightThemeModeEnum::LIGHT:
            macSetToLightTheme();
            break;
    }
#endif
    updateGraphicsAndUserInterface();
}

/**
 * Update the graphics and the user interface
 */
void
PreferencesGeneralWidget::updateGraphicsAndUserInterface()
{
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}

