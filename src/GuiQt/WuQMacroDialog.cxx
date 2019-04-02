
/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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

#define __WU_Q_MACRO_DIALOG_DECLARE__
#include "WuQMacroDialog.h"
#undef __WU_Q_MACRO_DIALOG_DECLARE__

#include <QAction>
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QInputDialog>
#include <QItemSelectionModel>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMenu>
#include <QMessageBox>
#include <QPainter>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QSplitter>
#include <QStackedWidget>
#include <QTreeView>
#include <QToolButton>
#include <QVBoxLayout>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "ElapsedTimer.h"
#include "MovieRecorder.h"
#include "MovieRecordingDialog.h"
#include "SessionManager.h"
#include "WuQMacro.h"
#include "WuQMacroCopyDialog.h"
#include "WuQMacroCommand.h"
#include "WuQMacroCommandParameterWidget.h"
#include "WuQMacroGroup.h"
#include "WuQMacroExecutor.h"
#include "WuQMacroExecutorMonitor.h"
#include "WuQMacroManager.h"
#include "WuQMacroMouseEventInfo.h"
#include "WuQMacroNewCommandSelectionDialog.h"
#include "WuQMacroShortCutKeyComboBox.h"
#include "WuQMacroStandardItemTypeEnum.h"
#include "WuQtUtilities.h"

using namespace caret;

/**
 * \class caret::WuQMacroDialog 
 * \brief Dialog for managing macros
 * \ingroup WuQMacro
 */

/**
 * Constructor.
 *
 * @param defaultMacroName
 *    Default name for new macro
 * @param parent
 *    The dialog's parent widget.
 */
WuQMacroDialog::WuQMacroDialog(QWidget* parent)
: QDialog(parent)
{
    setWindowTitle("Macros");
    this->setAttribute(Qt::WA_DeleteOnClose, false);
    
    m_macroGroups = WuQMacroManager::instance()->getActiveMacroGroups();
    
    QLabel* macroGroupLabel = new QLabel("Macros in:");
    
    m_macroGroupComboBox = new QComboBox();
    QObject::connect(m_macroGroupComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated),
                     this, &WuQMacroDialog::macroGroupComboBoxActivated);
    
    m_resetMacroGroupToolButton = new QToolButton();
    m_resetMacroGroupToolButton->setText("");
    m_resetMacroGroupToolButton->setToolTip("Reload current scene");
    QPixmap resetPixmap = createEditingToolButtonPixmap(m_resetMacroGroupToolButton,
                                                       EditButton::RESET);
    m_resetMacroGroupToolButton->setIcon(resetPixmap);
    QObject::connect(m_resetMacroGroupToolButton, &QToolButton::clicked,
                     this, &WuQMacroDialog::macroGroupResetToolButtonClicked);
    
    m_macroGroupToolButton = new QToolButton();
    m_macroGroupToolButton->setText("...");
    m_macroGroupToolButton->setToolTip("Export and import macro groups");
    QObject::connect(m_macroGroupToolButton, &QToolButton::clicked,
                     this, &WuQMacroDialog::macroGroupToolButtonClicked);
    
    QSize buttonSize(std::max(m_resetMacroGroupToolButton->sizeHint().width(),
                            m_macroGroupToolButton->sizeHint().width()),
                     std::max(m_resetMacroGroupToolButton->sizeHint().height(),
                              m_macroGroupToolButton->sizeHint().height()));
    m_resetMacroGroupToolButton->setFixedSize(buttonSize);
    m_macroGroupToolButton->setFixedSize(buttonSize);

    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setContentsMargins(0, 0, 0, 0);
    gridLayout->setColumnStretch(0, 0);
    gridLayout->setColumnStretch(1, 100);
    gridLayout->setColumnStretch(2, 0);
    gridLayout->setColumnStretch(3, 0);
    int row = 0;
    gridLayout->addWidget(macroGroupLabel, row, 0);
    gridLayout->addWidget(m_macroGroupComboBox, row, 1);
    gridLayout->addWidget(m_resetMacroGroupToolButton, row, 2);
    gridLayout->addWidget(m_macroGroupToolButton, row, 3);
    row++;
    gridLayout->addWidget(createHorizontalLine(), row, 0, 1, 4);
    row++;
    gridLayout->addWidget(createMacroRunAndEditingToolButtons(), row, 0, 1, 4);
    row++;
    
    for (int32_t iRow = 0; iRow < gridLayout->rowCount(); iRow++) {
        gridLayout->setRowStretch(iRow, 0);
    }
    
    QWidget* macroSelectionWidget = createMacroAndCommandSelectionWidget();
    m_macroWidget = createMacroDisplayWidget();
    m_commandWidget = createCommandDisplayWidget();
    m_emptyWidget = new QWidget();
    m_stackedWidget = new QStackedWidget();
    m_stackedWidget->addWidget(m_macroWidget);
    m_stackedWidget->addWidget(m_commandWidget);
    m_stackedWidget->addWidget(m_emptyWidget);
    m_stackedWidget->setCurrentWidget(m_emptyWidget);
    QScrollArea* stackedScrollArea = new QScrollArea();
    stackedScrollArea->setWidget(m_stackedWidget);
    stackedScrollArea->setWidgetResizable(true);

    m_dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    QObject::connect(m_dialogButtonBox, &QDialogButtonBox::rejected,
                     this, &WuQMacroDialog::close);
    QObject::connect(m_dialogButtonBox, &QDialogButtonBox::clicked,
                     this, &WuQMacroDialog::buttonBoxButtonClicked);
    
    QSplitter* splitter = new QSplitter();
    splitter->setOrientation(Qt::Vertical);
    macroSelectionWidget->setMinimumHeight(50);
    splitter->addWidget(macroSelectionWidget);
    stackedScrollArea->setMinimumHeight(50);
    splitter->addWidget(stackedScrollArea);
    splitter->setStretchFactor(0, 35);
    splitter->setStretchFactor(1, 65);
    
    QVBoxLayout* dialogLayout = new QVBoxLayout(this);
    dialogLayout->addLayout(gridLayout);
    dialogLayout->addWidget(splitter);
    dialogLayout->addWidget(m_dialogButtonBox);
    
    updateDialogContents();
    
    /*
     * Disable auto default for all push buttons
     */
    QList<QPushButton*> allChildPushButtons = findChildren<QPushButton*>(QRegExp(".*"));
    QListIterator<QPushButton*> allChildPushButtonsIterator(allChildPushButtons);
    while (allChildPushButtonsIterator.hasNext()) {
        QPushButton* pushButton = allChildPushButtonsIterator.next();
        pushButton->setAutoDefault(false);
        pushButton->setDefault(false);
    }
}

/**
 * Destructor.
 */
WuQMacroDialog::~WuQMacroDialog()
{
}

/**
 * Called when close event is issuedf
 *
 * @param event
 *    The close event
 */
void
WuQMacroDialog::closeEvent(QCloseEvent* event)
{
    s_previousDialogGeometry = saveGeometry();
    
    QDialog::closeEvent(event);
}

void
WuQMacroDialog::restorePositionAndSize()
{
    if ( ! s_previousDialogGeometry.isEmpty()) {
        restoreGeometry(s_previousDialogGeometry);
    }
}


/**
 * @return Widget with run and editing buttons
 */
QWidget*
WuQMacroDialog::createMacroRunAndEditingToolButtons()
{
    m_runMacroToolButton = new QToolButton();
    m_runMacroToolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_runMacroToolButton->setText("Run");
    QPixmap runPixmap = createEditingToolButtonPixmap(m_runMacroToolButton,
                                                         EditButton::RUN);
    m_runMacroToolButton->setIcon(runPixmap);
    m_runMacroToolButton->setToolTip("Runs the selected macro.  If a command is selected, "
                                     "the macro containing the command is run.");
    QObject::connect(m_runMacroToolButton, &QToolButton::clicked,
                     this, &WuQMacroDialog::runMacroToolButtonClicked);
    
    m_pauseMacroToolButton = new QToolButton();
    m_pauseMacroToolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_pauseMacroToolButton->setText("Pause");
    QPixmap pausePixmap = createEditingToolButtonPixmap(m_pauseMacroToolButton,
                                                       EditButton::PAUSE);
    m_pauseMacroToolButton->setIcon(pausePixmap);
    m_pauseMacroToolButton->setToolTip("Pause or continue a macro.  Button is highlighted "
                                       "when a macro is paused.");
    m_pauseMacroToolButton->setCheckable(true);
    QObject::connect(m_pauseMacroToolButton, &QToolButton::clicked,
                     this, &WuQMacroDialog::pauseContinueMacroToolButtonClicked);

    m_stopMacroToolButton = new QToolButton();
    m_stopMacroToolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_stopMacroToolButton->setText("Stop");
    QPixmap stopPixmap = createEditingToolButtonPixmap(m_stopMacroToolButton,
                                                      EditButton::STOP);
    m_stopMacroToolButton->setIcon(stopPixmap);
    m_stopMacroToolButton->setToolTip("Stop the currently running macro.  Response may not be immediate.");
    QObject::connect(m_stopMacroToolButton, &QToolButton::clicked,
                     this, &WuQMacroDialog::stopMacroToolButtonClicked);

    m_recordMacroToolButton = new QToolButton();
    m_recordMacroToolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_recordMacroToolButton->setText("Record");
    m_recordMacroToolButtonIconOff = createEditingToolButtonPixmap(m_recordMacroToolButton,
                                                                   EditButton::RECORD_OFF);
    m_recordMacroToolButtonIconOn  = createEditingToolButtonPixmap(m_recordMacroToolButton,
                                                                   EditButton::RECORD_ON);
    m_recordMacroToolButton->setIcon(m_recordMacroToolButtonIconOff);
    m_recordMacroToolButton->setToolTip("Record a new macro or record new commands");
    QObject::connect(m_recordMacroToolButton, &QToolButton::clicked,
                     this, &WuQMacroDialog::recordMacroToolButtonClicked);
    
    m_editingMoveUpToolButton = new QToolButton();
    QPixmap moveUpPixmap = createEditingToolButtonPixmap(m_editingMoveUpToolButton,
                                                         EditButton::MOVE_UP);
    m_editingMoveUpToolButton->setIcon(moveUpPixmap);
    m_editingMoveUpToolButton->setToolTip("Move selected macro/command up");
    QObject::connect(m_editingMoveUpToolButton, &QToolButton::clicked,
                     this, &WuQMacroDialog::editingMoveUpToolButtonClicked);
    
    m_editingMoveDownToolButton = new QToolButton();
    QPixmap moveDownPixmap = createEditingToolButtonPixmap(m_editingMoveDownToolButton,
                                                         EditButton::MOVE_DOWN);
    m_editingMoveDownToolButton->setIcon(moveDownPixmap);
    m_editingMoveDownToolButton->setToolTip("Move selected macro/command down");
    QObject::connect(m_editingMoveDownToolButton, &QToolButton::clicked,
                     this, &WuQMacroDialog::editingMoveDownToolButtonClicked);
    
    m_editingDeleteToolButton = new QToolButton();
    QPixmap deletePixmap = createEditingToolButtonPixmap(m_editingDeleteToolButton,
                                                         EditButton::DELETER);
    m_editingDeleteToolButton->setIcon(deletePixmap);
    m_editingDeleteToolButton->setToolTip("Delete the selected macro/command");
    QObject::connect(m_editingDeleteToolButton, &QToolButton::clicked,
                     this, &WuQMacroDialog::editingDeleteToolButtonClicked);
    
    m_editingInsertToolButton = new QToolButton();
    QPixmap insertPixmap = createEditingToolButtonPixmap(m_editingInsertToolButton,
                                                         EditButton::INSERTER);
    m_editingInsertToolButton->setIcon(insertPixmap);
    m_editingInsertToolButton->setToolTip("Insert a new macro or macro command below the selected item");
    QObject::connect(m_editingInsertToolButton, &QToolButton::clicked,
                     this, &WuQMacroDialog::editingInsertToolButtonClicked);
    
    const int spaceAmount(7);
    QWidget* widget = new QWidget();
    QHBoxLayout* toolButtonLayout = new QHBoxLayout(widget);
    toolButtonLayout->setContentsMargins(0, 0, 0, 0);
    toolButtonLayout->addWidget(m_stopMacroToolButton);
    toolButtonLayout->addWidget(m_runMacroToolButton);
    toolButtonLayout->addWidget(m_pauseMacroToolButton);
    toolButtonLayout->addWidget(m_recordMacroToolButton);
    toolButtonLayout->addSpacing(3 * spaceAmount);
    toolButtonLayout->addStretch();
    toolButtonLayout->addWidget(m_editingInsertToolButton);
    toolButtonLayout->addSpacing(spaceAmount);
    toolButtonLayout->addWidget(m_editingMoveUpToolButton);
    toolButtonLayout->addWidget(m_editingMoveDownToolButton);
    toolButtonLayout->addSpacing(spaceAmount);
    toolButtonLayout->addWidget(m_editingDeleteToolButton);
    
    return widget;
}

/**
 * @return the macro and command selection widget
 */
QWidget*
WuQMacroDialog::createMacroAndCommandSelectionWidget()
{
    m_treeView = new QTreeView();
    m_treeView->setHeaderHidden(true);
    QObject::connect(m_treeView, &QTreeView::clicked,
                     this, &WuQMacroDialog::treeViewItemClicked);
    m_treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(m_treeView, &QTreeView::customContextMenuRequested,
                     this, &WuQMacroDialog::treeViewCustomContextMenuRequested);

    return m_treeView;
}

/**
 * @return The widget displayed when a macro is selected
 */
QWidget*
WuQMacroDialog::createMacroDisplayWidget()
{
    QLabel* macroNameLabel = new QLabel("Name:");
    m_macroNameLineEdit = new QLineEdit();
    QObject::connect(m_macroNameLineEdit, &QLineEdit::textEdited,
                     this, &WuQMacroDialog::macroNameLineEditTextEdited);
    
    QLabel* shortCutKeyLabel = new QLabel("Short Cut Key:");
    QLabel* shortCutKeyMaskLabel = new QLabel(WuQMacroManager::getShortCutKeysMask());
    m_macroShortCutKeyComboBox = new WuQMacroShortCutKeyComboBox(this);
    QObject::connect(m_macroShortCutKeyComboBox, &WuQMacroShortCutKeyComboBox::shortCutKeySelected,
                     this, &WuQMacroDialog::macroShortCutKeySelected);

    QLabel* descriptionLabel = new QLabel("Description:");
    m_macroDescriptionTextEdit = new QPlainTextEdit();
    m_macroDescriptionTextEdit->setFixedHeight(100);
    QObject::connect(m_macroDescriptionTextEdit, &QPlainTextEdit::textChanged,
                     this, &WuQMacroDialog::macroDescriptionTextEditChanged);
    
    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setContentsMargins(0, 0, 0, 0);
    gridLayout->setColumnStretch(0, 0);
    gridLayout->setColumnStretch(1, 0);
    gridLayout->setColumnStretch(2, 100);
    int row = 0;
    gridLayout->addWidget(macroNameLabel, row, 0);
    gridLayout->addWidget(m_macroNameLineEdit, row, 1, 1, 2);
    row++;
    gridLayout->addWidget(shortCutKeyLabel, row, 0);
    gridLayout->addWidget(shortCutKeyMaskLabel, row, 1);
    gridLayout->addWidget(m_macroShortCutKeyComboBox->getWidget(), row, 2, Qt::AlignLeft);
    row++;
    gridLayout->addWidget(descriptionLabel, row, 0, Qt::AlignTop);
    gridLayout->addWidget(m_macroDescriptionTextEdit, row, 1, 1, 2);
    row++;

    QHBoxLayout* titleLayout = new QHBoxLayout();
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->addWidget(new QLabel("Macro "));
    titleLayout->addWidget(createHorizontalLine(), 100);
    
    QHBoxLayout* runOptionsTitleLayout = new QHBoxLayout();
    runOptionsTitleLayout->setContentsMargins(0, 0, 0, 0);
    runOptionsTitleLayout->addWidget(new QLabel("Run Macro Options "));
    runOptionsTitleLayout->addWidget(createHorizontalLine(), 100);
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->setSpacing(layout->spacing() / 2);
    layout->addLayout(titleLayout);
    layout->addLayout(gridLayout);
    layout->addLayout(runOptionsTitleLayout);
    layout->addWidget(createRunOptionsWidget());
    layout->addStretch();

    return widget;
}

/**
 * Called when macro name line edit text changed
 * @param text
 */
void
WuQMacroDialog::macroNameLineEditTextEdited(const QString& text)
{
    WuQMacro* macro = getSelectedMacro();
    if (macro != NULL) {
        macro->setName(text);
        m_macroNameLineEditBlockUpdateFlag = true;
        WuQMacroManager::instance()->macroWasModified(macro);
        m_macroNameLineEditBlockUpdateFlag = false;
    }
}

/**
 * Called when macro short cut key is selected
 *
 * @param shortCutKey
 *     Shortcut key that was selected
 */
void
WuQMacroDialog::macroShortCutKeySelected(const WuQMacroShortCutKeyEnum::Enum shortCutKey)
{
    WuQMacro* macro = getSelectedMacro();
    if (macro != NULL) {
        macro->setShortCutKey(shortCutKey);
        WuQMacroManager::instance()->macroWasModified(macro);
        updateMacroWidget(macro);
    }
}

/**
 * Called when macro description text edit is changed
 */
void
WuQMacroDialog::macroDescriptionTextEditChanged()
{
    WuQMacro* macro = getSelectedMacro();
    if (macro != NULL) {
        const QString text = m_macroDescriptionTextEdit->toPlainText();
        macro->setDescription(text);
        m_macroDescriptionTextEditBlockUpdateFlag = true;
        WuQMacroManager::instance()->macroWasModified(macro);
        m_macroDescriptionTextEditBlockUpdateFlag = false;
    }
}

/**
 * @return New instance of widget containing macro run options
 */
QWidget*
WuQMacroDialog::createRunOptionsWidget()
{
    QWidget* widget = new QWidget();
    
    QLabel* windowLabel = new QLabel("Window");
    m_runOptionsWindowComboBox = new QComboBox();
    m_runOptionsWindowComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    
    m_runOptionLoopCheckBox = new QCheckBox("Loop");
    m_runOptionLoopCheckBox->setChecked(false);
    m_runOptionLoopCheckBox->setToolTip("Run macro in a loop until stopped by user");
    QObject::connect(m_runOptionLoopCheckBox, &QCheckBox::clicked,
                     this, &WuQMacroDialog::runOptionLoopCheckBoxClicked);

    m_runOptionMoveMouseCheckBox = new QCheckBox("Move mouse to to highlight controls");
    m_runOptionMoveMouseCheckBox->setChecked(true);
    m_runOptionMoveMouseCheckBox->setToolTip("As macro runs, the mouse is moved to\n"
                                             "highlight user-interface controls");
    QObject::connect(m_runOptionMoveMouseCheckBox, &QCheckBox::clicked,
                     this, &WuQMacroDialog::runOptionMoveMouseCheckBoxClicked);

    m_runOptionRecordMovieWhileMacroRunsCheckBox = new QCheckBox("Record movie while macro runs");
    m_runOptionRecordMovieWhileMacroRunsCheckBox->setChecked(false);
    m_runOptionRecordMovieWhileMacroRunsCheckBox->setToolTip("While the macro runs, add images to Movie Recorder");
    QObject::connect(m_runOptionRecordMovieWhileMacroRunsCheckBox, &QCheckBox::clicked,
                     this, &WuQMacroDialog::runOptionRecordMovieCheckBoxClicked);

    m_runOptionCreateMovieAfterMacroRunsCheckBox = new QCheckBox("Create movie after macro finishes");
    m_runOptionCreateMovieAfterMacroRunsCheckBox->setChecked(false);
    m_runOptionCreateMovieAfterMacroRunsCheckBox->setToolTip("After macro finishes, create the movie");
    QObject::connect(m_runOptionCreateMovieAfterMacroRunsCheckBox, &QCheckBox::clicked,
                     this, &WuQMacroDialog::runOptionCreateMovieCheckBoxClicked);
    
    m_ignoreDelaysAndDurationsCheckBox = new QCheckBox("Ignore delays and durations");
    const QString ignoreToolTip("Ignore delays and durations and minimize iterations "
                                "to quickly execute macro (for debugging)");
    m_ignoreDelaysAndDurationsCheckBox->setToolTip(ignoreToolTip);
    QObject::connect(m_ignoreDelaysAndDurationsCheckBox, &QCheckBox::clicked,
                     this, &WuQMacroDialog::runOptionIgnoreDelaysAndDurationsCheckBoxClicked);

    QHBoxLayout* windowLayout = new QHBoxLayout();
    windowLayout->setContentsMargins(0, 0, 0, 0);
    windowLayout->addWidget(windowLabel);
    windowLayout->addWidget(m_runOptionsWindowComboBox);
    windowLayout->addStretch();

    /*
     * In layout, create movie option is indented
     */
    QGridLayout* runOptionsLayout = new QGridLayout(widget);
    runOptionsLayout->setColumnMinimumWidth(0, 15);
    runOptionsLayout->setColumnStretch(0, 0);
    runOptionsLayout->setColumnStretch(1, 100);
    int row = 0;
    runOptionsLayout->addLayout(windowLayout, row, 0, 1, 2, Qt::AlignLeft);
    row++;
    runOptionsLayout->addWidget(m_runOptionLoopCheckBox, row, 0, 1, 2, Qt::AlignLeft);
    row++;
    runOptionsLayout->addWidget(m_ignoreDelaysAndDurationsCheckBox, row, 0, 1, 2, Qt::AlignLeft);
    row++;
    runOptionsLayout->addWidget(m_runOptionMoveMouseCheckBox, row, 0, 1, 2, Qt::AlignLeft);
    row++;
    runOptionsLayout->addWidget(m_runOptionRecordMovieWhileMacroRunsCheckBox, row, 0, 1, 2, Qt::AlignLeft);
    row++;
    runOptionsLayout->addWidget(m_runOptionCreateMovieAfterMacroRunsCheckBox, row, 1, Qt::AlignLeft);
    row++;

    return widget;
}

/**
 * Called when run options move mouse checkbox is changed
 *
 * @param checked
 *     New checked status.
 */
void
WuQMacroDialog::runOptionMoveMouseCheckBoxClicked(bool checked)
{
    WuQMacroExecutorOptions* options = WuQMacroManager::instance()->getExecutorOptions();
    CaretAssert(options);
    options->setShowMouseMovement(checked);
}

/**
 * Called when run options loop checkbox is changed
 *
 * @param checked
 *     New checked status.
 */
void
WuQMacroDialog::runOptionLoopCheckBoxClicked(bool checked)
{
    WuQMacroExecutorOptions* options = WuQMacroManager::instance()->getExecutorOptions();
    CaretAssert(options);
    options->setLooping(checked);
}

/**
 * Called when run options record movie checkbox is changed
 *
 * @param checked
 *     New checked status.
 */
void
WuQMacroDialog::runOptionRecordMovieCheckBoxClicked(bool checked)
{
    WuQMacroExecutorOptions* options = WuQMacroManager::instance()->getExecutorOptions();
    CaretAssert(options);
    options->setRecordMovieDuringExecution(checked);
    updateCreateMovieCheckBox();
}

/**
 * Called when run options create movie checkbox is changed
 *
 * @param checked
 *     New checked status.
 */
void
WuQMacroDialog::runOptionCreateMovieCheckBoxClicked(bool checked)
{
    WuQMacroExecutorOptions* options = WuQMacroManager::instance()->getExecutorOptions();
    CaretAssert(options);
    
    /*
     * If transitioning from OFF to ON, verify file name
     */
    if (checked
        && ( ! options->isCreateMovieAfterMacroExecution())) {
        MovieRecorder* movieRecorder = SessionManager::get()->getMovieRecorder();
        const QString filename = MovieRecordingDialog::getMovieFileNameFromFileDialog(m_runOptionCreateMovieAfterMacroRunsCheckBox);
        if (filename.isEmpty()) {
            return;
        }
        movieRecorder->setMovieFileName(filename);
    }
    options->setCreateMovieAfterMacroExecution(checked);
}

/**
 * Called when run options ignore delays and durations checkbox is changed
 *
 * @param checked
 *     New checked status.
 */
void
WuQMacroDialog::runOptionIgnoreDelaysAndDurationsCheckBoxClicked(bool checked)
{
    WuQMacroExecutorOptions* options = WuQMacroManager::instance()->getExecutorOptions();
    CaretAssert(options);
    options->setIgnoreDelaysAndDurations(checked);
}

/**
 * Called when a button in dialog is clicked
 *
 * @param button
 *     Button that was clicked.
 */
void
WuQMacroDialog::buttonBoxButtonClicked(QAbstractButton* /*button*/)
{
}

/**
 * @return The widget displayed when a commnand is selected
 */
QWidget*
WuQMacroDialog::createCommandDisplayWidget()
{
    QLabel* titleLabel  = new QLabel("Title:");
    m_commandTitleLabel = new QLabel();
    
    QLabel* nameLabel  = new QLabel("Name:");
    m_commandNameLabel = new QLabel();
    
    QLabel* typeLabel = new QLabel("Type:");
    m_commandTypeLabel = new QLabel();

    QLabel* delayLabel = new QLabel("Delay:");
    m_commandDelaySpinBox = new QDoubleSpinBox();
    m_commandDelaySpinBox->setMinimum(0.0);
    m_commandDelaySpinBox->setMaximum(1000.0);
    m_commandDelaySpinBox->setSingleStep(1.0);
    m_commandDelaySpinBox->setDecimals(1);
    m_commandDelaySpinBox->setToolTip("Delay, in seconds, before running command");
    m_commandDelaySpinBox->setSizePolicy(QSizePolicy::Fixed,
                                         m_commandDelaySpinBox->sizePolicy().verticalPolicy());
    QObject::connect(m_commandDelaySpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                     this, &WuQMacroDialog::macroCommandDelaySpinBoxValueChanged);
    QLabel* delayTwoLabel = new QLabel("seconds before command");
    
    QLabel* descriptionLabel = new QLabel("Description:");
    m_commandDescriptionTextEdit = new QPlainTextEdit();
    m_commandDescriptionTextEdit->setMaximumHeight(100);
    QObject::connect(m_commandDescriptionTextEdit, &QPlainTextEdit::textChanged,
                     this, &WuQMacroDialog::macroCommandDescriptionTextEditChanged);

    QGridLayout* layout = new QGridLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setColumnStretch(0, 0);
    layout->setColumnStretch(1, 0);
    layout->setColumnStretch(2, 100);
    int row = 0;
    layout->addWidget(titleLabel, row, 0);
    layout->addWidget(m_commandTitleLabel, row, 1, 1, 2, Qt::AlignLeft);
    row++;
    layout->addWidget(nameLabel, row, 0);
    layout->addWidget(m_commandNameLabel, row, 1, 1, 2, Qt::AlignLeft);
    row++;
    layout->addWidget(typeLabel, row, 0);
    layout->addWidget(m_commandTypeLabel, row, 1, 1, 2, Qt::AlignLeft);
    row++;
    layout->addWidget(delayLabel, row, 0);
    layout->addWidget(m_commandDelaySpinBox, row, 1);
    layout->addWidget(delayTwoLabel, row, 2, Qt::AlignLeft);
    row++;
    layout->addWidget(descriptionLabel, row, 0, (Qt::AlignLeft | Qt::AlignTop));
    layout->addWidget(m_commandDescriptionTextEdit, row, 1, 1, 2);
    row++;
    
    QWidget* parametersWidget = new QWidget();
    m_parameterWidgetsGridLayout = new QGridLayout(parametersWidget);
    int pl(0), pt(0), pr(0), pb(0);
    m_parameterWidgetsGridLayout->getContentsMargins(&pl, &pt, &pr, &pb);
    pt = 0;
    m_parameterWidgetsGridLayout->setContentsMargins(pl, pt, pr, pb);
    m_parameterWidgetsGridLayout->setVerticalSpacing(static_cast<int>(m_parameterWidgetsGridLayout->verticalSpacing() / 1.2));
    m_parameterWidgetsGridLayout->setColumnStretch(0, 0);
    m_parameterWidgetsGridLayout->setColumnStretch(1, 100);
    m_parameterWidgetsGridLayout->setRowStretch(1000, 1000); /* push items up */

    QHBoxLayout* titleLayout = new QHBoxLayout();
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->addWidget(new QLabel("Command"));
    titleLayout->addWidget(createHorizontalLine(), 100);
    
    QHBoxLayout* parametersLayout = new QHBoxLayout();
    parametersLayout->setContentsMargins(0, 0, 0, 0);
    parametersLayout->addWidget(new QLabel("Edit Parameters"));
    parametersLayout->addWidget(createHorizontalLine(), 100);
    
    QWidget* widget = new QWidget();
    QVBoxLayout* widgetLayout = new QVBoxLayout(widget);
    widgetLayout->addLayout(titleLayout);
    widgetLayout->addLayout(layout);
    widgetLayout->addLayout(parametersLayout);
    widgetLayout->addWidget(parametersWidget);
    widgetLayout->addStretch();
    
    return widget;
}

/**
 * Update content of the dialog
 */
void
WuQMacroDialog::updateDialogContents()
{
    QString selectedUniqueIdentifer;
    const QVariant dataSelected = m_macroGroupComboBox->currentData();
    if (dataSelected.isValid()) {
        if (dataSelected.type() == QVariant::String) {
            selectedUniqueIdentifer = dataSelected.toString();
        }
    }

    m_macroGroups = WuQMacroManager::instance()->getActiveMacroGroups();
    
    m_macroGroupComboBox->clear();
    for (auto mg : m_macroGroups) {
        m_macroGroupComboBox->addItem(mg->getName(),
                                      mg->getUniqueIdentifier());
    }
    
    int32_t selectedIndex = m_macroGroupComboBox->findData(selectedUniqueIdentifer);
    if (selectedIndex < 0) {
        selectedIndex = m_macroGroupComboBox->count() - 1;
    }
    if ((selectedIndex >= 0)
        && (selectedIndex < m_macroGroupComboBox->count())) {
        m_macroGroupComboBox->setCurrentIndex(selectedIndex);
    }
    
    m_macroGroupToolButton->setEnabled(m_macroGroupComboBox->count() > 0);
    
    const QString windowText = m_runOptionsWindowComboBox->currentText();
    m_runOptionsWindowComboBox->clear();
    const std::vector<QString> windowIDs = WuQMacroManager::instance()->getMainWindowIdentifiers();
    for (const auto id : windowIDs) {
        m_runOptionsWindowComboBox->addItem(id);
    }
    m_runOptionsWindowComboBox->setCurrentText(windowText);
    
    const WuQMacroExecutorOptions* runOptions = WuQMacroManager::instance()->getExecutorOptions();
    CaretAssert(runOptions);
    m_runOptionMoveMouseCheckBox->setChecked(runOptions->isShowMouseMovement());
    m_runOptionLoopCheckBox->setChecked(runOptions->isLooping());
    m_runOptionRecordMovieWhileMacroRunsCheckBox->setChecked(runOptions->isRecordMovieDuringExecution());
    m_ignoreDelaysAndDurationsCheckBox->setChecked(runOptions->isIgnoreDelaysAndDurations());
    
    updateCreateMovieCheckBox();
    
    macroGroupComboBoxActivated(selectedIndex);
}

/**
 * Update the create movie checkbox status
 */
void
WuQMacroDialog::updateCreateMovieCheckBox()
{
    const WuQMacroExecutorOptions* runOptions = WuQMacroManager::instance()->getExecutorOptions();
    CaretAssert(runOptions);
    m_runOptionCreateMovieAfterMacroRunsCheckBox->setChecked(runOptions->isCreateMovieAfterMacroExecution());
    m_runOptionCreateMovieAfterMacroRunsCheckBox->setEnabled(runOptions->isRecordMovieDuringExecution());
}

/**
 * Called when an item in the tree view is clicked by user
 *
 * @param modelIndex
 *     Model index of item selected
 */
void
WuQMacroDialog::treeViewItemClicked(const QModelIndex& /*modelIndex*/)
{
    /*
     * Unused at this time.  The signal only works when user
     * click's an item and not when an arrow key is used
     * to select an item.   Replacement is for the selection
     * model to connect to selectionModelRowChanged().
     */
}

/**
 * Called to display custom context menu for the tree view
 */
void
WuQMacroDialog::treeViewCustomContextMenuRequested(const QPoint& pos)
{
    QModelIndex modelIndex = m_treeView->indexAt(pos);
    if (modelIndex.isValid()) {
        WuQMacroCommand* command = getSelectedMacroCommand();
        if (command != NULL) {
            QMenu menu(this);

            menu.addAction("Run this Command",
                           this, &WuQMacroDialog::runOnlySelectedCommandMenuItemSelected);
            
            menu.addSeparator();
            
            menu.addAction("Run Macro and Start With this Command",
                           this, &WuQMacroDialog::runAndStartWithSelectedCommandMenuItemSelected);
            
            menu.addAction("Run Macro and Start With this Command Without Delays/Durations",
                           this, &WuQMacroDialog::runAndStartWithNoDelayDurationSelectedCommandMenuItemSelected);
            
            menu.addSeparator();

            menu.addAction("Run Macro and Stop After this Command",
                           this, &WuQMacroDialog::runAndStopAfterSelectedCommandMenuItemSelected);

            menu.addAction("Run Macro and Stop After this Command Without Delays/Durations",
                           this, &WuQMacroDialog::runAndStopAfterWithNoDelayDurationSelectedCommandMenuItemSelected);

            menu.exec(m_treeView->mapToGlobal(pos));
        }
    }
}

/**
 * Called when 'run and start with...' is selected from
 * context (pop-up) menu.  Runs macro starting with selected command.
 */
void
WuQMacroDialog::runAndStartWithSelectedCommandMenuItemSelected()
{
    WuQMacroCommand* command = getSelectedMacroCommand();
    if (command != NULL) {
        runSelectedMacro(command,
                         NULL);
    }
    else {
        QMessageBox::warning(this, "Error", "No macro command is selected");
    }
}

/**
 * Called when 'run this command only...' is selected from
 * context (pop-up) menu.  Runs macro to selected command.
 */
void
WuQMacroDialog::runOnlySelectedCommandMenuItemSelected()
{
    WuQMacroCommand* command = getSelectedMacroCommand();
    if (command != NULL) {
        runSelectedMacro(command,
                         command);
    }
    else {
        QMessageBox::warning(this, "Error", "No macro command is selected");
    }
}

/**
 * Called when 'run and start with without delay duration...' is selected from
 * context (pop-up) menu.  Runs macro to selected command.
 */
void
WuQMacroDialog::runAndStartWithNoDelayDurationSelectedCommandMenuItemSelected()
{
    WuQMacroCommand* command = getSelectedMacroCommand();
    if (command != NULL) {
        WuQMacroManager* macroManager = WuQMacroManager::instance();
        WuQMacroExecutorOptions* runOptions = macroManager->getExecutorOptions();
        const bool savedIgnoreDelaysFlag    = runOptions->isIgnoreDelaysAndDurations();
        runOptions->setIgnoreDelaysAndDurations(true);
        runSelectedMacro(command,
                         NULL);
        runOptions->setIgnoreDelaysAndDurations(savedIgnoreDelaysFlag);
    }
    else {
        QMessageBox::warning(this, "Error", "No macro command is selected");
    }
}

/**
 * Called when 'run and stop after...' is selected from
 * context (pop-up) menu.  Runs macro to selected command.
 */
void
WuQMacroDialog::runAndStopAfterSelectedCommandMenuItemSelected()
{
    WuQMacroCommand* command = getSelectedMacroCommand();
    if (command != NULL) {
        WuQMacroManager* macroManager = WuQMacroManager::instance();
        WuQMacroExecutorOptions* runOptions = macroManager->getExecutorOptions();
        const bool savedStopAfterOptionFlag = runOptions->isStopAfterSelectedCommand();
        runOptions->setStopAfterSelectedCommand(true);
        runSelectedMacro(NULL,
                         command);
        runOptions->setStopAfterSelectedCommand(savedStopAfterOptionFlag);
    }
    else {
        QMessageBox::warning(this, "Error", "No macro command is selected");
    }
}

/**
 * Called when 'run and stop after without delay duration...' is selected from
 * context (pop-up) menu.  Runs macro to selected command.
 */
void
WuQMacroDialog::runAndStopAfterWithNoDelayDurationSelectedCommandMenuItemSelected()
{
    WuQMacroCommand* command = getSelectedMacroCommand();
    if (command != NULL) {
        WuQMacroManager* macroManager = WuQMacroManager::instance();
        WuQMacroExecutorOptions* runOptions = macroManager->getExecutorOptions();
        const bool savedStopAfterOptionFlag = runOptions->isStopAfterSelectedCommand();
        const bool savedIgnoreDelaysFlag    = runOptions->isIgnoreDelaysAndDurations();
        runOptions->setStopAfterSelectedCommand(true);
        runOptions->setIgnoreDelaysAndDurations(true);
        runSelectedMacro(NULL,
                         command);
        runOptions->setStopAfterSelectedCommand(savedStopAfterOptionFlag);
        runOptions->setIgnoreDelaysAndDurations(savedIgnoreDelaysFlag);
    }
    else {
        QMessageBox::warning(this, "Error", "No macro command is selected");
    }
}

/**
 * Called when an item in the tree is selected in some way
 * (mouse click, arrow key, etc)
 *
 * @param modelIndex
 *     Model index of item selected
 */
void
WuQMacroDialog::treeItemSelected(const QModelIndex& modelIndex)
{
    QStandardItemModel* selectedModel = NULL;
    if (modelIndex.isValid()) {
        const QAbstractItemModel* abstractModel = modelIndex.model();
        if (abstractModel != NULL) {
            const QStandardItemModel* constModel = qobject_cast<const QStandardItemModel*>(abstractModel);
            if (constModel != NULL) {
                selectedModel = const_cast<QStandardItemModel*>(constModel);
            }
        }
    }
    
    WuQMacro* macro(NULL);
    WuQMacroCommand* macroCommand(NULL);
    
    if (selectedModel != NULL) {
        QStandardItem* selectedItem = selectedModel->itemFromIndex(modelIndex);
        bool validFlag(false);
        const WuQMacroStandardItemTypeEnum::Enum itemType = WuQMacroStandardItemTypeEnum::fromIntegerCode(selectedItem->type(),
                                                                                                          &validFlag);
        if (validFlag) {
            switch (itemType) {
                case WuQMacroStandardItemTypeEnum::INVALID:
                    CaretAssertMessage(0, "Type should never be invalid");
                    break;
                case WuQMacroStandardItemTypeEnum::MACRO:
                    macro = dynamic_cast<WuQMacro*>(selectedItem);
                    CaretAssert(macro);
                    m_stackedWidget->setCurrentWidget(m_macroWidget);
                    break;
                case WuQMacroStandardItemTypeEnum::MACRO_COMMAND:
                    macroCommand = dynamic_cast<WuQMacroCommand*>(selectedItem);
                    CaretAssert(macroCommand);
                    m_stackedWidget->setCurrentWidget(m_commandWidget);
                    break;
            }
        }
        else {
            m_stackedWidget->setCurrentWidget(m_emptyWidget);
            CaretAssertMessage(0,
                               ("Invalid StandardItemModel type=" + AString::number(selectedItem->type())));
        }
    }
    else {
        m_stackedWidget->setCurrentWidget(m_emptyWidget);
    }
    
    updateMacroWidget(macro);
    updateCommandWidget(macroCommand);
    updateEditingToolButtons();
}

/**
 * Called when macro group combo box selection is made
 */
void
WuQMacroDialog::macroGroupComboBoxActivated(int)
{
    WuQMacroGroup* selectedGroup = getSelectedMacroGroup();

    if (selectedGroup != NULL) {
        QModelIndex selectedIndex;
        if (m_treeView->model() != selectedGroup) {
            /*
             * Model has changed, select first macro
             */
            if (selectedGroup->getNumberOfMacros() > 0) {
                selectedIndex = selectedGroup->indexFromItem(selectedGroup->getMacroAtIndex(0));
            }
        }
        else {
            selectedIndex = m_treeView->currentIndex();
        }
        
        m_blockSelectionModelRowChangedFlag = true;
        
        m_treeView->setModel(selectedGroup);
        
        {
            /*
             * Need to (re)connect the selection model's row changed signal
             * since setModel() was called.
             *
             * From the Qt Documentation for QAbstractItemView::setSelectionModel():
             *     Note that, if you call setModel() after this function, the given
             *     selectionModel will be replaced by one created by the view.
             */
            QItemSelectionModel* selectionModel = m_treeView->selectionModel();
            if (selectionModel != NULL) {
                /*
                 * Use the option Qt::UniqueConnection to avoid creating
                 * a duplicate connection
                 */
                QObject::connect(selectionModel, &QItemSelectionModel::currentRowChanged,
                                 this, &WuQMacroDialog::selectionModelRowChanged,
                                 Qt::UniqueConnection);
            }
        }
        m_blockSelectionModelRowChangedFlag = false;
    
        if (selectedIndex.isValid()) {
            m_treeView->setCurrentIndex(selectedIndex);
        }
        treeItemSelected(m_treeView->currentIndex());
    }
    else {
        m_treeView->setModel(new QStandardItemModel());
        treeItemSelected(QModelIndex());
    }
}

/**
 * Called when selection model's row is changed
 *
 * @param current
 *     Model index of current item
 * @parm previous
 *     Model index of previous item
 */
void
WuQMacroDialog::selectionModelRowChanged(const QModelIndex& current,
                                         const QModelIndex& /*previous*/)
{
    if (m_blockSelectionModelRowChangedFlag) {
        return;
    }
    
    treeItemSelected(current);
}

/**
 * Update the macro widget with the given macro
 *
 * @param macro
 *     The macro (may be NULL)
 */
void
WuQMacroDialog::updateMacroWidget(WuQMacro* macro)
{
    QString name;
    WuQMacroShortCutKeyEnum::Enum shortCutKey = WuQMacroShortCutKeyEnum::Key_None;
    QString text;
    if (macro != NULL) {
        name = macro->getName();
        text = macro->getDescription();
        shortCutKey = macro->getShortCutKey();
    }

    if ( ! m_macroNameLineEditBlockUpdateFlag) {
        m_macroNameLineEdit->setText(name);
    }
    
    m_macroShortCutKeyComboBox->setSelectedShortCutKey(shortCutKey);
    if ( ! m_macroDescriptionTextEditBlockUpdateFlag) {
        QSignalBlocker DescriptionBlocker(m_macroDescriptionTextEdit);
        m_macroDescriptionTextEdit->setPlainText(text);
    }
}

/**
 * Update the command widget with the given macommandcro
 *
 * @param command
 *     The command (may be NULL)
 */
void
WuQMacroDialog::updateCommandWidget(WuQMacroCommand* command)
{
    QString title;
    QString name;
    QString type;
    QString toolTip;
    float delay(0.0f);
    if (command != NULL) {
        title = command->text();
        name  = command->getObjectName();
        switch (command->getCommandType()) {
            case WuQMacroCommandTypeEnum::CUSTOM_OPERATION:
            {
                const QString operationName = command->getCustomOperationTypeName();
                type = operationName;
            }
                break;
            case WuQMacroCommandTypeEnum::MOUSE:
            {
                WuQMacroMouseEventInfo* mouseInfo = command->getMouseEventInfo();
                CaretAssert(mouseInfo);
                WuQMacroMouseEventTypeEnum::Enum mouseEventType = mouseInfo->getMouseEventType();
                type = WuQMacroMouseEventTypeEnum::toGuiName(mouseEventType);
            }
                break;
            case WuQMacroCommandTypeEnum::WIDGET:
                type = WuQMacroWidgetTypeEnum::toGuiName(command->getWidgetType());
                break;
        }
        toolTip = command->getObjectToolTip();
        delay   = command->getDelayInSeconds();
    }
    m_commandTitleLabel->setText(title);
    m_commandNameLabel->setText(name);
    m_commandTypeLabel->setText(type);

    QSignalBlocker delayBlocker(m_commandDelaySpinBox);
    m_commandDelaySpinBox->setValue(delay);

    if ( ! m_macroDescriptionCommandTextEditBlockUpdateFlag) {
        QSignalBlocker descriptionBlocker(m_commandDescriptionTextEdit);
        m_commandDescriptionTextEdit->setPlainText(toolTip);
    }
    
    /**
     * Update the parameter widgets
     */
    const int32_t numParams = ((command != NULL)
                               ? command->getNumberOfParameters()
                               : 0);
    int32_t numWidgets = static_cast<int32_t>(m_parameterWidgets.size());
    
    for (int32_t i = numWidgets; i < numParams; i++) {
        WuQMacroCommandParameterWidget* cpw = new WuQMacroCommandParameterWidget(i,
                                                                 m_parameterWidgetsGridLayout,
                                                                 this);
        QObject::connect(cpw, &WuQMacroCommandParameterWidget::dataChanged,
                         this, &WuQMacroDialog::commandParamaterDataChanged);
        m_parameterWidgets.push_back(cpw);
    }
    
    const QString windowID = m_runOptionsWindowComboBox->currentText();
    int32_t windowIndex = windowID.toInt();
    if (windowIndex > 0) {
        --windowIndex;  /* Range 1..N but need 0..N-1 */
    }
    for (int32_t i = 0; i < numParams; i++) {
        m_parameterWidgets[i]->updateContent(windowIndex,
                                             command,
                                             command->getParameterAtIndex(i));
    }
    
    numWidgets = static_cast<int32_t>(m_parameterWidgets.size());
    for (int32_t i = numParams; i < numWidgets; i++) {
        m_parameterWidgets[i]->updateContent(-1,
                                             NULL,
                                             NULL);
    }
}

/**
 * Called when a command parameter is changed
 *
 * @param index
 *     Index of the parameter
 */
void
WuQMacroDialog::commandParamaterDataChanged(int)
{
    WuQMacro* macro = getSelectedMacro();
    if (macro != NULL) {
        WuQMacroManager::instance()->macroWasModified(macro);
    }
    updateDialogContents();
}

/**
 * @return Pointer to selected macro group or NULL if none available.
 */
WuQMacroGroup*
WuQMacroDialog::getSelectedMacroGroup()
{
    const int32_t selectedGroupIndex = m_macroGroupComboBox->currentIndex();
    if ((selectedGroupIndex >= 0)
        && (selectedGroupIndex < m_macroGroupComboBox->count())) {
        CaretAssertVectorIndex(m_macroGroups, selectedGroupIndex);
        WuQMacroGroup* group = m_macroGroups[selectedGroupIndex];
        return group;
    }
    return NULL;
}

/**
 * @return Pointer to selected macro.  If a macro command is selected, its parent
 * macro is returned (NULL if no macro is selected)
 */
WuQMacro*
WuQMacroDialog::getSelectedMacro()
{
    WuQMacro* macro(NULL);
    
    QStandardItem* selectedItem = getSelectedItem();
    if (selectedItem != NULL) {
        bool validFlag(false);
        const WuQMacroStandardItemTypeEnum::Enum itemType = WuQMacroStandardItemTypeEnum::fromIntegerCode(selectedItem->type(),
                                                                                                          &validFlag);
        if (validFlag) {
            switch (itemType) {
                case WuQMacroStandardItemTypeEnum::INVALID:
                    CaretAssertMessage(0, "Type should never be invalid");
                    break;
                case WuQMacroStandardItemTypeEnum::MACRO_COMMAND:
                {
                    /*
                     * Parent should be WuQMacro
                     */
                    QStandardItem* selectedItemParent = selectedItem->parent();
                    CaretAssert(selectedItemParent);
                    macro = dynamic_cast<WuQMacro*>(selectedItemParent);
                    CaretAssert(macro);
                }
                    break;
                case WuQMacroStandardItemTypeEnum::MACRO:
                    macro = dynamic_cast<WuQMacro*>(selectedItem);
                    CaretAssert(macro);
                    break;
            }
        }
        else {
            CaretAssertMessage(0,
                               ("Invalid StandardItemModel type=" + AString::number(selectedItem->type())));
        }
    }
    
    return macro;
}

/**
 * @return Pointer to selected macro command (NULL if no macro command is selected)
 */
WuQMacroCommand*
WuQMacroDialog::getSelectedMacroCommand()
{
    WuQMacroCommand* macroCommand(NULL);
    
    QStandardItem* selectedItem = getSelectedItem();
    if (selectedItem != NULL) {
        bool validFlag(false);
        const WuQMacroStandardItemTypeEnum::Enum itemType = WuQMacroStandardItemTypeEnum::fromIntegerCode(selectedItem->type(),
                                                                                                          &validFlag);
        if (validFlag) {
            switch (itemType) {
                case WuQMacroStandardItemTypeEnum::INVALID:
                    CaretAssertMessage(0, "Type should never be invalid");
                    break;
                case WuQMacroStandardItemTypeEnum::MACRO_COMMAND:
                    macroCommand = dynamic_cast<WuQMacroCommand*>(selectedItem);
                    CaretAssert(macroCommand);
                    break;
                case WuQMacroStandardItemTypeEnum::MACRO:
                    break;
            }
        }
        else {
            CaretAssertMessage(0,
                               ("Invalid StandardItemModel type=" + AString::number(selectedItem->type())));
        }
    }
    
    return macroCommand;
}

/**
 * @return The selected item (NULL if invalid)
 */
QStandardItem*
WuQMacroDialog::getSelectedItem() const
{
    QModelIndex modelIndex = m_treeView->currentIndex();
    if (modelIndex.isValid()) {
        QAbstractItemModel* abstractModel = m_treeView->model();
        if (abstractModel != NULL) {
            QStandardItemModel* model = qobject_cast<QStandardItemModel*>(abstractModel);
            if (model != NULL) {
                QStandardItem* item = model->itemFromIndex(modelIndex);
                return item;
            }
        }
    }
    
    return NULL;
}

/**
 * @return The selected item type
 */
WuQMacroStandardItemTypeEnum::Enum
WuQMacroDialog::getSelectedItemType() const
{
    WuQMacroStandardItemTypeEnum::Enum itemType = WuQMacroStandardItemTypeEnum::INVALID;
    QStandardItem* item = getSelectedItem();
    if (item != NULL) {
        bool validFlag(false);
        itemType = WuQMacroStandardItemTypeEnum::fromIntegerCode(item->type(),
                                                                 &validFlag);
    }
    
    return itemType;
}

/**
 * Called to pause/continue a macro
 */
void
WuQMacroDialog::pauseContinueMacroToolButtonClicked()
{
    WuQMacroManager::instance()->pauseContinueMacro();
    updateEditingToolButtons();
}

/**
 * Called when run button is clicked
 */
void
WuQMacroDialog::runMacroToolButtonClicked()
{
    runSelectedMacro(NULL,
                     NULL);
}
/**
 * Called when run button is clicked
 *
 * @param macroCommandToStartAt
 *     Macro command at which execution should begin.  If NULL, start
 *     with the first command in the macro
 * @param macroCommandToStopAfter
 *     Macro command that the executor may stop after, depending upon options
 */
void
WuQMacroDialog::runSelectedMacro(const WuQMacroCommand* macroCommandToStartAt,
                                 const WuQMacroCommand* macroCommandToStopAfter)
{
    switch (WuQMacroManager::instance()->getMode()) {
        case WuQMacroModeEnum::OFF:
            break;
        case WuQMacroModeEnum::RECORDING_INSERT_COMMANDS:
        case WuQMacroModeEnum::RECORDING_NEW_MACRO:
        {
            QMessageBox::critical(m_runMacroToolButton,
                                  "Error",
                                  "A macro is being recorded.  Finish recording of macro.",
                                  QMessageBox::Ok,
                                  QMessageBox::NoButton);
            return;
        }            break;
        case WuQMacroModeEnum::RUNNING:
            break;
    }
    
    WuQMacro* macro = getSelectedMacro();
    if (macro == NULL) {
        return;
    }
    if (macro->getNumberOfMacroCommands() <= 0) {
        QMessageBox::critical(m_runMacroToolButton,
                              "Error",
                              "Macro does not contain any commands",
                              QMessageBox::Ok,
                              QMessageBox::NoButton);
        return;
    }
    
    QWidget* window = getWindow();

    /*
     * While macro runs, edit buttons are disabled
     */
    m_macroIsRunningFlag = true;
    updateEditingToolButtons();
    QApplication::processEvents();
    ElapsedTimer timer;
    timer.start();
    WuQMacro* lastMacroRun = WuQMacroManager::instance()->runMacro(window,
                                                                   macro,
                                                                   macroCommandToStartAt,
                                                                   macroCommandToStopAfter);
    // may use this later when testing    std::cout << "Time to run macro: " << timer.getElapsedTimeSeconds() << std::endl;
    m_macroIsRunningFlag = false;
    
    if (lastMacroRun != getSelectedMacro()) {
        QModelIndex modelIndex = macro->index();
        m_treeView->setCurrentIndex(modelIndex);
        treeItemSelected(modelIndex);
    }
    updateEditingToolButtons();
}

/**
 * @return the parent main window
 */
QWidget*
WuQMacroDialog::getWindow()
{
    const QString windowID = m_runOptionsWindowComboBox->currentText();
    QWidget* window = WuQMacroManager::instance()->getMainWindowWithIdentifier(windowID);
    if (window == NULL) {
        window = this;
        while (window != NULL) {
            if (qobject_cast<QMainWindow*>(window) != NULL) {
                break;
            }
            else {
                window = window->parentWidget();
            }
        }
        if (window == NULL) {
            window = parentWidget();
        }
    }
    
    return window;
}

/**
 * Called when stop button is clicked
 */
void
WuQMacroDialog::stopMacroToolButtonClicked()
{
    /*
     * If recording, STOP button stops recording
     */
    switch (WuQMacroManager::instance()->getMode()) {
        case WuQMacroModeEnum::OFF:
            break;
        case WuQMacroModeEnum::RECORDING_INSERT_COMMANDS:
        case WuQMacroModeEnum::RECORDING_NEW_MACRO:
            recordMacroToolButtonClicked();
            return;
            break;
        case WuQMacroModeEnum::RUNNING:
            break;
    }

    WuQMacroManager::instance()->stopMacro();
    updateEditingToolButtons();
}

/**
 * Called when record button is clicked
 */
void
WuQMacroDialog::recordMacroToolButtonClicked()
{
    bool startRecordingValid(false);
    bool stopRecordingValid(false);
    switch (WuQMacroManager::instance()->getMode()) {
        case WuQMacroModeEnum::OFF:
            startRecordingValid = true;
            break;
        case WuQMacroModeEnum::RECORDING_INSERT_COMMANDS:
        case WuQMacroModeEnum::RECORDING_NEW_MACRO:
            stopRecordingValid = true;
            break;
        case WuQMacroModeEnum::RUNNING:
            break;
    }
    
    if (startRecordingValid) {
        QMenu menu(m_editingInsertToolButton);
        QAction* newCommandAction = menu.addAction("Record and Insert New Commands Below",
                                                   this,
                                                   &WuQMacroDialog::insertMenuRecordNewMacroCommandSelected);
        newCommandAction->setEnabled(getSelectedMacro() != NULL);
        
        menu.addSeparator();
        
        QAction* newMacroAction = menu.addAction("Record and Insert New Macro Below...",
                                                 this,
                                                 &WuQMacroDialog::recordAndInsertNewMacroSelected);
        newMacroAction->setEnabled(getSelectedMacroGroup() != NULL);

        menu.exec(m_recordMacroToolButton->mapToGlobal(QPoint(0, 0)));
    }
    else if (stopRecordingValid) {
        stopRecordingSelected();
    }
}

/**
 * Called when import item is selected
 */
void
WuQMacroDialog::importMacroGroupActionTriggered()
{
    WuQMacroGroup* macroGroup = getSelectedMacroGroup();
    if (macroGroup != NULL) {
        if (WuQMacroManager::instance()->importMacros(m_macroGroupToolButton,
                                                      macroGroup)) {
            updateDialogContents();
        }
    }
}

/**
 * Called when export item is selected
 */
void
WuQMacroDialog::exportMacroGroupActionTriggered()
{
    WuQMacroGroup* macroGroup = getSelectedMacroGroup();
    WuQMacro* macro = getSelectedMacro();

    if (macro != NULL) {
        if (WuQMacroManager::instance()->exportMacros(m_macroGroupToolButton,
                                                      macroGroup,
                                                      macro)) {
            updateDialogContents();
        }
    }
}

/**
 * Called when macro command description text edit is changed
 */
void
WuQMacroDialog::macroCommandDescriptionTextEditChanged()
{
    WuQMacroCommand* command = getSelectedMacroCommand();
    if (command != NULL) {
        command->setObjectToolTip(m_commandDescriptionTextEdit->toPlainText());
        m_macroDescriptionCommandTextEditBlockUpdateFlag = true;
        WuQMacroManager::instance()->macroWasModified(getSelectedMacro());
        m_macroDescriptionCommandTextEditBlockUpdateFlag = false;
    }
}

/**
 * Called when macro commands delay value is changed
 *
 * @param value
 *     New value
 */
void
WuQMacroDialog::macroCommandDelaySpinBoxValueChanged(double value)
{
    WuQMacroCommand* command = getSelectedMacroCommand();
    CaretAssert(command);
    command->setDelayInSeconds(value);

    WuQMacro* macro = getSelectedMacro();
    if (macro != NULL) {
        WuQMacroManager::instance()->macroWasModified(macro);
    }
}

/**
 * @return a horizontal line
 */
QWidget*
WuQMacroDialog::createHorizontalLine() const
{
    QFrame* horizontalLine = new QFrame();
    horizontalLine->setMidLineWidth(1);
    horizontalLine->setLineWidth(1);
    horizontalLine->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    return horizontalLine;
}

/**
 * Called when macro group tool button is clicked
 */
void
WuQMacroDialog::macroGroupToolButtonClicked()
{
    QMenu* menu = new QMenu(this);
    
    QAction* importAction = menu->addAction("Import...");
    QObject::connect(importAction, &QAction::triggered,
                     this, &WuQMacroDialog::importMacroGroupActionTriggered);
    
    QAction* exportAction = menu->addAction("Export...");
    QObject::connect(exportAction, &QAction::triggered,
                     this, &WuQMacroDialog::exportMacroGroupActionTriggered);
    
    menu->exec(mapToGlobal(m_macroGroupToolButton->pos()));
    
    delete menu;
}

/**
 * Called when macro group reset tool button is clicked
 */
void
WuQMacroDialog::macroGroupResetToolButtonClicked()
{
    WuQMacro* macro = WuQMacroManager::instance()->resetMacro(getWindow(),
                                                              getSelectedMacro());
    updateDialogContents();
    if (macro != NULL) {
        m_treeView->setCurrentIndex(macro->index());
        treeItemSelected(macro->index());
    }
    
    updateDialogContents();
}

/**
 * Called when editing move up button clicked
 */
void
WuQMacroDialog::editingMoveUpToolButtonClicked()
{
    WuQMacroGroup* macroGroup = getSelectedMacroGroup();
    WuQMacro* macro = getSelectedMacro();
    WuQMacroCommand* command = getSelectedMacroCommand();
    
    switch (getSelectedItemType()) {
        case WuQMacroStandardItemTypeEnum::INVALID:
            break;
        case WuQMacroStandardItemTypeEnum::MACRO:
            if (macroGroup != NULL) {
                if (macro != NULL) {
                    macroGroup->moveMacroUp(macro);
                    m_treeView->setCurrentIndex(macro->index());
                    treeItemSelected(macro->index());
                }
            }
            break;
        case WuQMacroStandardItemTypeEnum::MACRO_COMMAND:
            if ((macro != NULL)
                && (command != NULL)) {
                macro->moveMacroCommandUp(command);
                m_treeView->setCurrentIndex(command->index());
                treeItemSelected(command->index());
            }
            break;
    }
}

/**
 * Called when editing move down button clicked
 */
void
WuQMacroDialog::editingMoveDownToolButtonClicked()
{
    WuQMacroGroup* macroGroup = getSelectedMacroGroup();
    WuQMacro* macro = getSelectedMacro();
    WuQMacroCommand* command = getSelectedMacroCommand();
    
    switch (getSelectedItemType()) {
        case WuQMacroStandardItemTypeEnum::INVALID:
            break;
        case WuQMacroStandardItemTypeEnum::MACRO:
            if (macroGroup != NULL) {
               if (macro != NULL) {
                   macroGroup->moveMacroDown(macro);
                   m_treeView->setCurrentIndex(macro->index());
                   treeItemSelected(macro->index());
               }
            }
            break;
        case WuQMacroStandardItemTypeEnum::MACRO_COMMAND:
            if ((macro != NULL)
                && (command != NULL)) {
                macro->moveMacroCommandDown(command);
                m_treeView->setCurrentIndex(command->index());
                treeItemSelected(command->index());
            }
            break;
    }
}

/**
 * Called when editing delete button clicked
 */
void
WuQMacroDialog::editingDeleteToolButtonClicked()
{
    WuQMacroGroup* macroGroup = getSelectedMacroGroup();
    WuQMacro* macro = getSelectedMacro();
    WuQMacroCommand* command = getSelectedMacroCommand();
    
    switch (getSelectedItemType()) {
        case WuQMacroStandardItemTypeEnum::INVALID:
            break;
        case WuQMacroStandardItemTypeEnum::MACRO:
            if ((macroGroup != NULL)
                && (macro != NULL)) {
                int32_t macroIndex = macroGroup->getIndexOfMacro(macro);
                if (WuQMacroManager::instance()->deleteMacro(m_editingDeleteToolButton,
                                                             macroGroup,
                                                             macro)) {
                    
                    updateDialogContents();
                    
                    CaretAssert(macroIndex >= 0);
                    if (macroIndex >= macroGroup->getNumberOfMacros()) {
                        macroIndex = macroGroup->getNumberOfMacros() - 1;
                    }
                    if ((macroIndex >= 0)
                        && (macroIndex < macroGroup->getNumberOfMacros())) {
                        QModelIndex modelIndex = macroGroup->getMacroAtIndex(macroIndex)->index();
                        m_treeView->setCurrentIndex(modelIndex);
                        treeItemSelected(modelIndex);
                    }
                }
            }
            break;
        case WuQMacroStandardItemTypeEnum::MACRO_COMMAND:
            if ((macro != NULL)
                && (command != NULL)) {
                if (WuQMacroManager::instance()->deleteMacroCommand(m_editingDeleteToolButton,
                                                                    macroGroup,
                                                                    macro,
                                                                    command)) {
                    updateDialogContents();
                }
            }
            break;
    }
}

/**
 * Called when editing insert button clicked
 */
void
WuQMacroDialog::editingInsertToolButtonClicked()
{
    WuQMacroGroup* macroGroup = getSelectedMacroGroup();
    if (macroGroup == NULL) {
        return;
    }
    const WuQMacro* selectedMacro = getSelectedMacro();
    
    WuQMacroManager* macroManager = WuQMacroManager::instance();
    
    /*
     * Copy is valid when there is at least one macro, in any macro group
     * and the macro is not the selected macro
     */
    bool copyValidFlag(false);
    const std::vector<const WuQMacroGroup*> allGroups = macroManager->getAllMacroGroups();
    for (const auto mg : allGroups) {
        const int32_t nm = mg->getNumberOfMacros();
        for (int32_t i = 0; i < nm; i++) {
            if (mg->getMacroAtIndex(i) != selectedMacro) {
                copyValidFlag = true;
            }
        }
    }
    
    bool insertMacroValidFlag(false);
    bool insertMacroCommandValidFlag(false);
    switch (getSelectedItemType()) {
        case WuQMacroStandardItemTypeEnum::INVALID:
            insertMacroValidFlag = true;
            break;
        case WuQMacroStandardItemTypeEnum::MACRO:
            insertMacroValidFlag = true;
            insertMacroCommandValidFlag = true;
            break;
        case WuQMacroStandardItemTypeEnum::MACRO_COMMAND:
            insertMacroValidFlag = true;
            insertMacroCommandValidFlag = true;
            break;
    }
    
    switch (macroManager->getMode()) {
        case WuQMacroModeEnum::OFF:
            break;
        case WuQMacroModeEnum::RECORDING_INSERT_COMMANDS:
        case WuQMacroModeEnum::RECORDING_NEW_MACRO:
            insertMacroValidFlag = false;
            copyValidFlag = false;
            insertMacroCommandValidFlag = false;
            break;
        case WuQMacroModeEnum::RUNNING:
            insertMacroValidFlag = false;
            copyValidFlag = false;
            insertMacroCommandValidFlag = false;
            break;
    }
    
    const bool showRecordItemsFlag(false);
    
    QMenu menu(m_editingInsertToolButton);

    /*
     * Macro command items
     */
    QAction* insertNewCommandAction = menu.addAction("Insert New Command Below...",
                                                     this,
                                                     &WuQMacroDialog::insertMenuNewMacroCommandSelected);
    insertNewCommandAction->setEnabled(insertMacroCommandValidFlag);

    if (showRecordItemsFlag) {
        QAction* insertRecordNewCommandAction = menu.addAction("Record and Insert New Commands Below",
                                                               this,
                                                               &WuQMacroDialog::insertMenuRecordNewMacroCommandSelected);
        insertRecordNewCommandAction->setEnabled(insertMacroCommandValidFlag);
    }
    
    /*
     * Macro items
     */
    if (menu.actions().count() > 0) {
        menu.addSeparator();
    }
    
    QAction* copyMacroAction = menu.addAction("Copy and Insert Macro Below...",
                                              this,
                                              &WuQMacroDialog::insertMenuCopyMacroSelected);
    copyMacroAction->setEnabled(copyValidFlag
                                && insertMacroValidFlag);
    
    QAction* insertMenuAction = menu.addAction("Insert New Macro Below...",
                                               this,
                                               &WuQMacroDialog::insertMenuNewMacroSelected);
    insertMenuAction->setEnabled(insertMacroValidFlag);
    
    if (showRecordItemsFlag) {
        QAction* recordNewMacroAction = menu.addAction("Record and Insert Macro Below...",
                                                       this,
                                                       &WuQMacroDialog::recordAndInsertNewMacroSelected);
        recordNewMacroAction->setEnabled(insertMacroValidFlag);
    }
    
    menu.exec(m_editingInsertToolButton->mapToGlobal(QPoint(0, 0)));
}

/**
 * Called to stop recording
 */
void
WuQMacroDialog::stopRecordingSelected()
{
    WuQMacroManager::instance()->stopRecordingNewMacro();
}

/**
 * Called when copy and insert macro menu item is selected
 */
void
WuQMacroDialog::insertMenuCopyMacroSelected()
{
    WuQMacroCopyDialog dialog(this);
    if (dialog.exec() == WuQMacroCopyDialog::Accepted) {
        const WuQMacro* macro = dialog.getMacroToCopy();
        if (macro != NULL) {
            WuQMacro* newMacro = new WuQMacro(*macro);
            newMacro->setName("Copy of "
                              + macro->getName());
            insertNewMacro(newMacro);
        }
    }
}

/**
 * Called when copy and record and insert macro menu item is selected
 */
void
WuQMacroDialog::recordAndInsertNewMacroSelected()
{
    WuQMacroGroup* macroGroup = getSelectedMacroGroup();
    CaretAssert(macroGroup);

    WuQMacro* newMacro = WuQMacroManager::instance()->startRecordingNewMacro(m_editingInsertToolButton,
                                                                             macroGroup,
                                                                             getSelectedMacro());
    if (newMacro != NULL) {
        const QModelIndex modelIndex = newMacro->index();
        m_treeView->setCurrentIndex(modelIndex);
        treeItemSelected(modelIndex);
    }
}


/**
 * Insert a new macro
 *
 * @param macro
 *    Macro to insert, must be valid
 */
void
WuQMacroDialog::insertNewMacro(WuQMacro* macro)
{
    CaretAssert(macro);
    
    const WuQMacro* selectedMacro = getSelectedMacro();
    WuQMacroGroup* macroGroup = getSelectedMacroGroup();
    CaretAssert(macroGroup);
    
    if (selectedMacro != NULL) {
        const int32_t index = macroGroup->getIndexOfMacro(selectedMacro);
        macroGroup->insertMacroAtIndex(index + 1,
                                       macro);
    }
    else {
        macroGroup->addMacro(macro);
    }
    updateDialogContents();
    
    QModelIndex selectedIndex = macroGroup->indexFromItem(macro);
    if (selectedIndex.isValid()) {
        m_treeView->setCurrentIndex(selectedIndex);
        updateDialogContents();
    }
    
    WuQMacroManager::instance()->macroWasModified(macro);
}

/**
 * Called when insert new macro menu item selected
 */
void
WuQMacroDialog::insertMenuNewMacroSelected()
{
    bool okFlag(false);
    const QString defaultName = WuQMacroManager::instance()->getNewMacroDefaultName();
    const QString macroName = QInputDialog::getText(m_editingInsertToolButton,
                                                    "Create Macro",
                                                    "New Macro Name",
                                                    QLineEdit::Normal,
                                                    defaultName,
                                                    &okFlag);
    if (okFlag) {
        if ( ! macroName.isEmpty()) {
            WuQMacro* macro = new WuQMacro();
            macro->setName(macroName);
            insertNewMacro(macro);
        }
    }
}

/**
 * Called when insert new macro command menu item selected
 */
void
WuQMacroDialog::insertMenuNewMacroCommandSelected()
{
    WuQMacroNewCommandSelectionDialog dialog(this);
    QObject::connect(&dialog, &WuQMacroNewCommandSelectionDialog::signalNewMacroCommandCreated,
                     this, &WuQMacroDialog::addNewMacroCommand);
    if (dialog.exec() == WuQMacroNewCommandSelectionDialog::Accepted) {
    }
}

/**
 * Called when insert and record a new macro command menu item selected
 */
void
WuQMacroDialog::insertMenuRecordNewMacroCommandSelected()
{
    WuQMacro* macro = getSelectedMacro();
    if (macro == NULL) {
        QMessageBox::warning(m_editingInsertToolButton,
                             "Error",
                             "No macro is selected for recording new commands");
        return;
    }
    
    WuQMacroManager::instance()->startRecordingNewCommandInsertion(macro,
                                                                getSelectedMacroCommand());
}

/**
 * Select the given command (if it is not NULL) or the macro (if it is not NULL)
 *
 * @param macro
 *     The macro
 * @param command
 *     The macro command
 */
void
WuQMacroDialog::selectMacroCommand(const WuQMacro* macro,
                                   const WuQMacroCommand* command)
{
    QModelIndex selectedIndex;
    if (command != NULL) {
        selectedIndex = command->index();
    }
    else if (macro != NULL) {
        if (macro != NULL) {
            m_treeView->setExpanded(macro->index(),
                                    true);
        }
        selectedIndex = macro->index();
    }
    
    if (selectedIndex.isValid()) {
        m_treeView->setCurrentIndex(selectedIndex);
    }
}

/**
 * Add a new macro command
 */
void
WuQMacroDialog::addNewMacroCommand(WuQMacroCommand* command)
{
    if (command == NULL) {
        return;
    }
    
    WuQMacro* macro(NULL);
    switch (getSelectedItemType()) {
        case WuQMacroStandardItemTypeEnum::INVALID:
            CaretAssert(0);
            break;
        case WuQMacroStandardItemTypeEnum::MACRO:
        {
            macro = getSelectedMacro();
            CaretAssert(macro);
            macro->insertMacroCommandAtIndex(0,
                                             command);
        }
            break;
        case WuQMacroStandardItemTypeEnum::MACRO_COMMAND:
        {
            macro = getSelectedMacro();
            CaretAssert(macro);
            const WuQMacroCommand* selectedCommand = getSelectedMacroCommand();
            CaretAssert(selectedCommand);
            const int32_t selectedIndex = macro->getIndexOfMacroCommand(selectedCommand);
            macro->insertMacroCommandAtIndex(selectedIndex + 1,
                                             command);
        }
    }
    
    WuQMacroGroup* selectedGroup = getSelectedMacroGroup();
    CaretAssert(selectedGroup);
    QModelIndex selectedIndex = selectedGroup->indexFromItem(command);
    m_treeView->setCurrentIndex(selectedIndex);
    
    updateDialogContents();
    
    if (macro != NULL) {
        WuQMacroManager::instance()->macroWasModified(macro);
    }
}

/**
 * Update editing buttons after item selected
 * in macro/command tree view
 */
void
WuQMacroDialog::updateEditingToolButtons()
{
    WuQMacroGroup* macroGroup = getSelectedMacroGroup();
    WuQMacro* macro = getSelectedMacro();
    WuQMacroCommand* command = getSelectedMacroCommand();
    
    bool resetValid(false);
    bool runValid(false);
    bool stopValid(false);
    bool insertValid(false);
    bool deleteValid(false);
    bool moveUpValid(false);
    bool moveDownValid(false);
    bool pauseValid(false);
    bool pauseChecked(false);
    bool recordValid(false);

    if (m_macroIsRunningFlag) {
        pauseValid = true;
        switch (WuQMacroManager::instance()->getMacroExecutorMonitor()->getMode()) {
            case WuQMacroExecutorMonitor::Mode::PAUSE:
                pauseChecked = true;
                break;
            case WuQMacroExecutorMonitor::Mode::RUN:
                break;
            case WuQMacroExecutorMonitor::Mode::STOP:
                break;
        }
        stopValid  = true;
    }
    else {
        insertValid = (macroGroup != NULL);
        recordValid = (macroGroup != NULL);
        resetValid  = (macroGroup != NULL);
        
        switch (getSelectedItemType()) {
            case WuQMacroStandardItemTypeEnum::INVALID:
                break;
            case WuQMacroStandardItemTypeEnum::MACRO:
                if (macroGroup != NULL) {
                    if (macro != NULL) {
                        const int32_t macroIndex = macroGroup->getIndexOfMacro(macro);
                        deleteValid = true;
                        moveUpValid = (macroIndex > 0);
                        moveDownValid = (macroIndex < (macroGroup->getNumberOfMacros() - 1));
                        runValid = true;
                    }
                }
                break;
            case WuQMacroStandardItemTypeEnum::MACRO_COMMAND:
            {
                if ((macro != NULL)
                    && (command != NULL)) {
                    const int32_t commandIndex = macro->getIndexOfMacroCommand(command);
                    deleteValid   = true;
                    moveUpValid   = (commandIndex > 0);
                    moveDownValid = (commandIndex < (macro->getNumberOfMacroCommands() - 1));
                    runValid = true;
                }
            }
                break;
        }
    }
    
    bool recordingFlag(false);
    switch (WuQMacroManager::instance()->getMode()) {
        case WuQMacroModeEnum::OFF:
            break;
        case WuQMacroModeEnum::RECORDING_INSERT_COMMANDS:
            recordingFlag = true;
            break;
        case WuQMacroModeEnum::RECORDING_NEW_MACRO:
            recordingFlag = true;
            break;
        case WuQMacroModeEnum::RUNNING:
            break;
    }
    
    if (recordingFlag) {
        m_recordMacroToolButton->setIcon(m_recordMacroToolButtonIconOn);
        runValid  = false;
        stopValid = true;
    }
    else {
        m_recordMacroToolButton->setIcon(m_recordMacroToolButtonIconOff);
    }

    m_pauseMacroToolButton->setEnabled(pauseValid);
    m_pauseMacroToolButton->setChecked(pauseChecked);
    m_resetMacroGroupToolButton->setEnabled(resetValid);
    m_macroGroupToolButton->setEnabled(resetValid);
    m_runMacroToolButton->setEnabled(runValid);
    m_stopMacroToolButton->setEnabled(stopValid);
    m_recordMacroToolButton->setEnabled(recordValid);
    m_editingDeleteToolButton->setEnabled(deleteValid);
    m_editingInsertToolButton->setEnabled(insertValid);
    m_editingMoveDownToolButton->setEnabled(moveDownValid);
    m_editingMoveUpToolButton->setEnabled(moveUpValid);
    
}

/**
 * Create a pixmap for the given editing tool button
 *
 * @param editButton
 *     The edit button identifier
 * @return
 *     Pixmap for the given button
 */
QPixmap
WuQMacroDialog::createEditingToolButtonPixmap(const QWidget* widget,
                                              const EditButton editButton)
{
    CaretAssert(widget);
    const qreal pixmapSize = 22.0;
    const qreal maxValue = pixmapSize / 2.0 - 1.0;
    const qreal arrowTip = maxValue * (2.0 / 3.0);

    uint32_t pixmapOptions(static_cast<uint32_t>(WuQtUtilities::PixMapCreationOptions::TransparentBackground));
    switch (editButton) {
        case EditButton::DELETER:
            break;
        case EditButton::INSERTER:
            break;
        case EditButton::MOVE_DOWN:
            break;
        case EditButton::MOVE_UP:
            break;
        case EditButton::PAUSE:
            break;
        case EditButton::RECORD_OFF:
            break;
        case EditButton::RECORD_ON:
            /* allow background */
            pixmapOptions = 0;
            break;
        case EditButton::RESET:
            /* allow background */
            pixmapOptions = 0;
            break;
        case EditButton::RUN:
            /* allow background */
            pixmapOptions = 0;
            break;
        case EditButton::STOP:
            /* allow background */
            pixmapOptions = 0;
            break;
    }

    QPixmap pixmap(static_cast<int>(pixmapSize),
                   static_cast<int>(pixmapSize));
    QSharedPointer<QPainter> painter = WuQtUtilities::createPixmapWidgetPainterOriginCenter(widget,
                                                                                            pixmap,
                                                                                            pixmapOptions);
    QPen pen(painter->pen());
    pen.setWidth(3);
    painter->setPen(pen);
    
    switch (editButton) {
        case EditButton::DELETER:
            /*
             * 'X' symbol
             */
            pen.setColor(Qt::red);
            painter->setPen(pen);
            painter->drawLine(QPointF(-maxValue,  maxValue),  QPointF(maxValue, -maxValue));
            painter->drawLine(QPointF(-maxValue,  -maxValue), QPointF(maxValue, maxValue));
            break;
        case EditButton::INSERTER:
            /*
             * Plus symbol
             */
            painter->drawLine(QPointF(0,  maxValue), QPointF(0, -maxValue));
            painter->drawLine(QPointF(-maxValue, 0), QPointF(maxValue, 0));
            break;
        case EditButton::MOVE_DOWN:
            /*
             * Down arrow
             */
            painter->drawLine(QPointF(0, maxValue), QPointF(0, -maxValue));
            painter->drawLine(QPointF(0, -maxValue), QPointF(arrowTip,  -maxValue + arrowTip));
            painter->drawLine(QPointF(0, -maxValue), QPointF(-arrowTip, -maxValue + arrowTip));
            break;
        case EditButton::MOVE_UP:
            /*
             * Up arrow
             */
            painter->drawLine(QPointF(0, maxValue), QPointF(0, -maxValue));
            painter->drawLine(QPointF(0, maxValue), QPointF(arrowTip,  maxValue - arrowTip));
            painter->drawLine(QPointF(0, maxValue), QPointF(-arrowTip, maxValue - arrowTip));
            break;
        case EditButton::PAUSE:
        {
            const qreal x = maxValue / 3;
            const qreal y = maxValue * 0.667;
            /*
             * Parallel vertical lines
             */
            painter->drawLine(QPointF(-x,  y), QPointF(-x, -y));
            painter->drawLine(QPointF( x,  y), QPointF( x, -y));
        }
            break;
        case EditButton::RECORD_OFF:
        {
            painter->setPen(Qt::red);
            painter->drawEllipse(QPointF(0, 0),
                                 maxValue, maxValue);
        }
            break;
        case EditButton::RECORD_ON:
        {
            painter->setPen(Qt::red);
            painter->setBrush(Qt::red);
            painter->drawEllipse(QPointF(0, 0),
                                 maxValue, maxValue);
        }
            break;
        case EditButton::RESET:
        {
            QPen pen = painter->pen();
            pen.setWidth(2);
            painter->setPen(pen);
            /*
             * From drawArc() documentation
             *  Zero is at 3 o'clock
             *  Units are 1/16th of a degree
             *  Positive is counter-clockwise
             * BUT perhaps due to us transforming coordinate system positive is clockwise
             */
            const int sz(4);
            const int widthHeight(maxValue * 2 - sz);
            QRect rectangle(-maxValue, -maxValue, widthHeight, widthHeight);
            const int startAngle(0);
            const int spanAngle(270 * 16);
            painter->drawArc(rectangle,
                             startAngle,
                             spanAngle);
            
            const int ts(sz + 2);
            const int x(-maxValue + widthHeight/2);
            const int y(-maxValue + widthHeight);
            QPoint trianglePoints[3] = {
                { x, y + ts },
                { x + ts, y },
                { x, y - ts }
            };
            painter->setBrush(pen.color());
            painter->drawPolygon(trianglePoints, 3);
        }
            break;
        case EditButton::RUN:
        {
            /*
             * Triangle
             */
            const qreal y = maxValue * 0.85;
            painter->setBrush(pen.color());
            const qreal p = pixmapSize / 3;
            const QPointF points[3] = {
                QPointF(-p, -y),
                QPointF( p, 0.0),
                QPointF(-p,  y)
            };
            painter->drawConvexPolygon(points, 3);
        }
            break;
        case EditButton::STOP:
        {
            /*
             * Square
             */
            painter->setBrush(pen.color());
            const qreal a = maxValue * 0.85;
            const QPointF points[4] = {
                QPointF(-a, -a),
                QPointF( a, -a),
                QPointF( a,  a),
                QPointF(-a,  a)
            };
            painter->drawConvexPolygon(points, 4);
        }
            break;
    }
    
    return pixmap;
}

