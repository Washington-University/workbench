
/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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

#define __PROGRESS_REPORTING_BAR_DECLARE__
#include "ProgressReportingBar.h"
#undef __PROGRESS_REPORTING_BAR_DECLARE__

#include <QApplication>
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>

#include "CaretAssert.h"
#include "ProgressReportingFromEvent.h"

using namespace caret;


    
/**
 * \class caret::ProgressReportingBar 
 * \brief Shows a progress bar with a message from EventProgressUpdate events.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
ProgressReportingBar::ProgressReportingBar(QWidget* parent)
: QWidget(parent)
{
    m_progressBar = new QProgressBar();
    m_progressBar->setTextVisible(false);

    m_messageLabel = new QLabel();
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_progressBar);
    layout->addWidget(m_messageLabel);
    
    m_progressFromEvent = new ProgressReportingFromEvent(this);
    m_progressFromEvent->setEventReceivingEnabled(false);
    
    reset();
    
    QObject::connect(m_progressFromEvent, SIGNAL(reportProgressRange(const int, const int)),
                     m_progressBar, SLOT(setRange(int, int)));
    QObject::connect(m_progressFromEvent, SIGNAL(reportProgressValue(const int)),
                     m_progressBar, SLOT(setValue(int)));
    QObject::connect(m_progressFromEvent, SIGNAL(reportProgressMessage(const QString&)),
                     m_messageLabel, SLOT(setText(const QString&)));
}

/**
 * Destructor.
 */
ProgressReportingBar::~ProgressReportingBar()
{
}

/**
 * THIS METHOD MUST BE CALLED BY USER TO ENABLE PROGRESS REPORTING as
 * update receiving is OFF by default.
 *
 * Set enabled for updates.  Since the progress bar may be in dialogs that
 * are not closed, we don't want to receive events for other operations.
 * Use this method to enabled and disable the progress bar so that it
 * is only enabled at the appropriate time.
 *
 * @param enabledForUpdates
 *     New status.
 */
void
ProgressReportingBar::setEnabledForUpdates(const bool enabledForUpdates)
{
    m_progressFromEvent->setEventReceivingEnabled(enabledForUpdates);
}

/**
 * Set message for progress.  Note: May get overridden by progress update.
 *
 * @param text
 *     New text for message.
 */
void
ProgressReportingBar::setMessage(const QString& text)
{
    m_messageLabel->setText(text);
    QApplication::processEvents();
}

/**
 * Reset the progress bar.
 */
void
ProgressReportingBar::reset()
{
    m_progressBar->setRange(0, 1);
    m_progressBar->setValue(0);
    m_messageLabel->setText("");
    QApplication::processEvents();
}

