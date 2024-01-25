
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

#define __PREFERENCES_DISPLAY_DECLARE__
#include "PreferencesDisplayWidget.h"
#undef __PREFERENCES_DISPLAY_DECLARE__

    
/**
 * \class caret::PreferencesDisplayWidget
 * \brief Widget for recent file properties in preferences
 * \ingroup GuiQt
 */

#include <QDesktopServices>
#include <QGridLayout>
#include <QGuiApplication>
#include <QLabel>
#include <QScreen>
#include <QUrl>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretPreferences.h"
#include "EnumComboBoxTemplate.h"
#include "EventManager.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;

/**
 * Constructor.
 */
PreferencesDisplayWidget::PreferencesDisplayWidget(QWidget* parent)
: QWidget(parent)
{
    QLabel* highDpiLabel = new QLabel("High DPI Mode");
    m_displayHighDpiModeEnumComboBox = new EnumComboBoxTemplate(this);
    m_displayHighDpiModeEnumComboBox->setup<DisplayHighDpiModeEnum,DisplayHighDpiModeEnum::Enum>();
    QObject::connect(m_displayHighDpiModeEnumComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &PreferencesDisplayWidget::displayHighDpiModeEnumComboBoxItemActivated);
    
    QLabel* descriptionLabel = new QLabel();
    descriptionLabel->setTextFormat(Qt::RichText);
    descriptionLabel->setWordWrap(true);
    descriptionLabel->setText(DisplayHighDpiModeEnum::getToolTip());
    
    QLabel* htmlLabel = new QLabel("<html>For more information: <a href=\"https://doc.qt.io/qt-6/highdpi.html\">https://doc.qt.io/qt-6/highdpi.html</a><html>");
    QObject::connect(htmlLabel, &QLabel::linkActivated,
                     this, &PreferencesDisplayWidget::htmlLinkClicked);
    
    QLabel* highDpiAutoModeLabel = new QLabel("High DPI Detected by O/S and Qt Version: "
                                              + QString(DisplayHighDpiModeEnum::isHighDpiEnabledForAutoMode()
                                                        ? "Yes"
                                                        : "No"));

    /*
     * Layouts
     */
    QGridLayout* layout = new QGridLayout(this);
    layout->setColumnStretch(0, 0);
    layout->setColumnStretch(1, 100);
    layout->setRowStretch(100, 100);
    int32_t row(0);
    layout->addWidget(highDpiLabel,
                      row, 0);
    layout->addWidget(m_displayHighDpiModeEnumComboBox->getWidget(),
                      row, 1, Qt::AlignLeft);
    row++;
    layout->addWidget(descriptionLabel,
                      row, 0, 1, 2);
    row++;
    layout->addWidget(htmlLabel,
                      row, 0, 1, 2);
    row++;
    layout->addWidget(getDisplayInfoTextWidget(),
                      row, 0, 1, 2);
    row++;
    layout->addWidget(highDpiAutoModeLabel,
                      row, 0, 1, 2);
}

/**
 * Destructor.
 */
PreferencesDisplayWidget::~PreferencesDisplayWidget()
{
}

/*
 * Update the content in this widget
 * @param caretPreferences
 *    The caret preferences
 */
void
PreferencesDisplayWidget::updateContent(CaretPreferences* caretPreferences)
{
    m_preferences = caretPreferences;
    CaretAssert(m_preferences);
    
    const DisplayHighDpiModeEnum::Enum highDpiMode = m_preferences->getDisplayHighDpiMode();
    m_displayHighDpiModeEnumComboBox->setSelectedItem<DisplayHighDpiModeEnum,DisplayHighDpiModeEnum::Enum>(highDpiMode);
}

/**
 * Called when high dpi combo box is changed
 */
void
PreferencesDisplayWidget::displayHighDpiModeEnumComboBoxItemActivated()
{
    CaretAssert(m_preferences);
    const DisplayHighDpiModeEnum::Enum highDpiMode = m_displayHighDpiModeEnumComboBox->getSelectedItem<DisplayHighDpiModeEnum,DisplayHighDpiModeEnum::Enum>();
    m_preferences->setDisplayHighDpiMode(highDpiMode);
    
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}

/**
 * Called when HTML link clicked in a label
 */
void
PreferencesDisplayWidget::htmlLinkClicked(const QString& htmlLink)
{
    QUrl url(htmlLink);
    if ( ! QDesktopServices::openUrl(url)) {
        WuQMessageBox::errorOk(this,
                               "Unable to open link.  Either it is invalid or failed to open in Web Browser (copy link to your Web Browser).");
    }
}

/**
 * @return Information about the displays
 */
QWidget*
PreferencesDisplayWidget::getDisplayInfoTextWidget() const
{
    QWidget* widget = new QWidget();
    QGridLayout* layout = new QGridLayout(widget);
    
    int col(0);
    const int COLUMN_NAME(col++);
    const int COLUMN_WIDTH(col++);
    const int COLUMN_HEIGHT(col++);
    const int COLUMN_LOGICAL_DPI(col++);
    const int COLUMN_PHSYICAL_DPI(col++);
    const int COLUMN_PIXEL_RATIO(col++);
    const int COLUMN_HIGH_DPI(col++);
    
    QList<QScreen*> screens = QGuiApplication::screens();
    const int32_t numScreens = screens.size();
    
    int row(0);
    for (int iScreen = 0; iScreen < numScreens; iScreen++) {
        CaretAssertVectorIndex(screens, iScreen);
        const QScreen* screen(screens[iScreen]);
        CaretAssert(screen);

        if (iScreen == 0) {
            layout->addWidget(new QLabel("Display\nName"),
                              row, COLUMN_NAME);
            layout->addWidget(new QLabel("Width"),
                              row, COLUMN_WIDTH);
            layout->addWidget(new QLabel("Height"),
                              row, COLUMN_HEIGHT);
            layout->addWidget(new QLabel("Logical\nDPI"),
                              row, COLUMN_LOGICAL_DPI);
            layout->addWidget(new QLabel("Physical\nDPI"),
                              row, COLUMN_PHSYICAL_DPI);
            layout->addWidget(new QLabel("Pixel\nRatio"),
                              row, COLUMN_PIXEL_RATIO);
            layout->addWidget(new QLabel("High\nDPI"),
                              row, COLUMN_HIGH_DPI);
            row++;
        }
       
        const QString highDpiText((screen->devicePixelRatio() >= 2)
                                  ? "Yes"
                                  : "No");

        layout->addWidget(new QLabel(screen->name()),
                          row, COLUMN_NAME);
        layout->addWidget(new QLabel(AString::number(screen->size().width())),
                          row, COLUMN_WIDTH);
        layout->addWidget(new QLabel(AString::number(screen->size().height())),
                          row, COLUMN_HEIGHT);
        layout->addWidget(new QLabel(AString::number(screen->logicalDotsPerInch())),
                          row, COLUMN_LOGICAL_DPI);
        layout->addWidget(new QLabel(AString::number(screen->physicalDotsPerInch())),
                          row, COLUMN_PHSYICAL_DPI);
        layout->addWidget(new QLabel(AString::number(screen->devicePixelRatio())),
                          row, COLUMN_PIXEL_RATIO);
        layout->addWidget(new QLabel(highDpiText),
                          row, COLUMN_HIGH_DPI);
        row++;
    }

    return widget;
}


