
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

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretPreferences.h"
#include "EnumComboBoxTemplate.h"
#include "EventManager.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "PreferencesDialog.h"
#include "SessionManager.h"
#include "WuQtUtilities.h"

using namespace caret;

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
    PreferencesDialog::addWidgetToLayout(gridLayout,
                                         "Appearance",
                                         m_darkLightThemeModeEnumComboBox->getWidget());

    const AString msg("When the appearance is changed, some toolbar buttons in open windows "
                      "may not correctly change to the correct colors.  This can be fixed by "
                      "opening a new window and closing the existing window or restarting "
                      "wb_view.");
    QLabel* msgLabel(new QLabel(msg));
    msgLabel->setWordWrap(true);
    
    /*
     * Layouts
     */
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addLayout(gridLayout);
    layout->addWidget(msgLabel);
    layout->addStretch();
    
#ifdef CARET_OS_MACOSX
    setEnabled(true);
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
    
    const GuiDarkLightThemeModeEnum::Enum darkLightMode  = GuiManager::getCurrentDarkLightTheme();
    m_darkLightThemeModeEnumComboBox->setSelectedItem<GuiDarkLightThemeModeEnum,GuiDarkLightThemeModeEnum::Enum>(darkLightMode);
}

/**
 * Called when graphics minification filter changed
 */
void
PreferencesGeneralWidget::darkLightThemeModeEnumComboBoxItemActivated()
{
    CaretAssert(m_preferences);

    const GuiDarkLightThemeModeEnum::Enum darkLightThemeMode = m_darkLightThemeModeEnumComboBox->getSelectedItem<GuiDarkLightThemeModeEnum,GuiDarkLightThemeModeEnum::Enum>();
    
    GuiManager::applyDarkLightTheme(darkLightThemeMode);
    
    m_preferences->setDarkLightThemMode(darkLightThemeMode);
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

