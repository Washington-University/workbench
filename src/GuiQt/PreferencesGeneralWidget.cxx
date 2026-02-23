
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
#include <QGuiApplication>
#include <QLabel>
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
#include <QStyleHints>
#endif

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretPreferences.h"
#include "EnumComboBoxTemplate.h"
#include "EventManager.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiDarkLightColorSchemeManager.h"
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
    m_darkLightColorSchemeModeEnumComboBox = new EnumComboBoxTemplate(this);
    
    /*
     * System, Dark, and Light on macOS
     */
    m_darkLightColorSchemeModeEnumComboBox->setup<GuiDarkLightColorSchemeModeEnum,GuiDarkLightColorSchemeModeEnum::Enum>();
    QObject::connect(m_darkLightColorSchemeModeEnumComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &PreferencesGeneralWidget::darkLightColorSchemeModeEnumComboBoxItemActivated);

    m_qtColorSchemeLabel = new QLabel();

    PreferencesDialog::addWidgetToLayout(gridLayout,
                                         "Color Scheme",
                                         m_darkLightColorSchemeModeEnumComboBox->getWidget());

    PreferencesDialog::addWidgetToLayout(gridLayout,
                                         "QT Scheme",
                                         m_qtColorSchemeLabel);
    
    const AString msg("Note: Color scheme may not work on all computers");
    QLabel* msgLabel(new QLabel(msg));
    msgLabel->setWordWrap(true);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addLayout(gridLayout);
    layout->addWidget(msgLabel);
    layout->addStretch();
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
    
    const GuiDarkLightColorSchemeModeEnum::Enum darkLightMode = m_preferences->getDarkLightColorSchemeMode();
    m_darkLightColorSchemeModeEnumComboBox->setSelectedItem<GuiDarkLightColorSchemeModeEnum,GuiDarkLightColorSchemeModeEnum::Enum>(darkLightMode);
    
    AString colorSchemeName;
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
    const QStyleHints* styleHints(QGuiApplication::styleHints());
    CaretAssert(styleHints);
    const Qt::ColorScheme colorScheme(styleHints->colorScheme());
    colorSchemeName = "Invalid";
    switch (colorScheme) {
        case Qt::ColorScheme::Unknown:
            colorSchemeName = "Unknown";
            break;
        case Qt::ColorScheme::Light:
            colorSchemeName = "Light";
            break;
        case Qt::ColorScheme::Dark:
            colorSchemeName = "Dark";
            break;
    }
#else
    colorSchemeName = "Not supported Qt Version < 6.8.0";
#endif
    m_qtColorSchemeLabel->setText(colorSchemeName);
}

/**
 * Called when graphics minification filter changed
 */
void
PreferencesGeneralWidget::darkLightColorSchemeModeEnumComboBoxItemActivated()
{
    CaretAssert(m_preferences);

    const GuiDarkLightColorSchemeModeEnum::Enum darkLightColorSchemeMode = m_darkLightColorSchemeModeEnumComboBox->getSelectedItem<GuiDarkLightColorSchemeModeEnum,GuiDarkLightColorSchemeModeEnum::Enum>();
    
    m_preferences->setDarkLightColorSchemeMode(darkLightColorSchemeMode);
    
    GuiDarkLightColorSchemeManager* darkLightColorSchemeManager(GuiManager::get()->getGuiDarkLightColorSchemeManager());
    CaretAssert(darkLightColorSchemeManager);
    darkLightColorSchemeManager->darkLightColorSchemeChangedByPreferencesGeneralWidget();
    
    updateGraphicsAndUserInterface();
    
    updateContent(m_preferences);
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



